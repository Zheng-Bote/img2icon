/**
 * SPDX-FileComment: img2icon — Converts JPG/PNG to ICO, PNG variants and SVG.
 * SPDX-FileType: SOURCE
 * SPDX-License-Identifier: MIT
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 *
 * @file   img2icon.cpp
 * @brief  CLI tool: image → *.ico, logo_{92,256,512}px.png, *.svg
 * @version 0.2.0
 * @date 2026-03-10
 *
 * @author ZHENG Robert
 * @license MIT License
 * @std    C++23
 * @deps   Magick++ (ImageMagick ≥ 7), CLI11
 *
 * Build:
 *   c++ -std=c++23 $(Magick++-config --cxxflags --libs) img2icon.cpp -o
 * img2icon
 *
 * Usage:
 *   img2icon -i photo.png -o ./icons [--no-bg] [--fuzz 15] [--name logo]
 */

#include <CLI/CLI.hpp>
#include <Magick++.h>

#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <print>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

// ─────────────────────────────────────────────
//  Base64  (needed for SVG data-URI embedding)
// ─────────────────────────────────────────────
[[nodiscard]]
std::string base64_encode(std::span<const std::uint8_t> data) {
  static constexpr std::string_view kAlpha =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  std::string out;
  out.reserve(((data.size() + 2) / 3) * 4);

  for (std::size_t i = 0; i < data.size(); i += 3) {
    const std::uint32_t v =
        (static_cast<std::uint32_t>(data[i]) << 16) |
        (i + 1 < data.size() ? static_cast<std::uint32_t>(data[i + 1]) << 8
                             : 0u) |
        (i + 2 < data.size() ? static_cast<std::uint32_t>(data[i + 2]) : 0u);

    out += kAlpha[(v >> 18) & 0x3F];
    out += kAlpha[(v >> 12) & 0x3F];
    out += (i + 1 < data.size()) ? kAlpha[(v >> 6) & 0x3F] : '=';
    out += (i + 2 < data.size()) ? kAlpha[v & 0x3F] : '=';
  }
  return out;
}

// ─────────────────────────────────────────────
//  Background removal
// ─────────────────────────────────────────────

/**
 * @brief Removes the image background by flood-filling from all four corners.
 *
 * Works well for images with a solid or near-solid background colour.
 * @param img   Image to modify in place.
 * @param fuzz  Tolerance in % of QuantumRange (0–100).
 */
void remove_background(Magick::Image &img, double fuzz_pct) {
  img.alpha(true); // Ensure alpha channel is active

  // QuantumRange is an unstable C macro ((Quantum) 65535) that requires
  // the Quantum typedef to be in scope — not guaranteed across IM versions.
  // MAGICKCORE_QUANTUM_DEPTH (8 / 16 / 32) is always a plain integer define,
  // so we derive the max quantum value from it safely at compile time.
  static constexpr double kQuantumMax =
      static_cast<double>((1UL << MAGICKCORE_QUANTUM_DEPTH) - 1UL);

  const double fuzz = (fuzz_pct / 100.0) * kQuantumMax;
  img.colorFuzz(fuzz);

  const std::array<std::pair<int, int>, 4> corners{{
      {0, 0},
      {static_cast<int>(img.columns()) - 1, 0},
      {0, static_cast<int>(img.rows()) - 1},
      {static_cast<int>(img.columns()) - 1, static_cast<int>(img.rows()) - 1},
  }};

  // floodFillColor with "transparent" avoids TransparentAlpha — another
  // Quantum-cast macro ((Quantum) 0) — which has the same scope problem.
  const Magick::Color kTransparent("transparent");
  for (auto [x, y] : corners) {
    img.floodFillColor(x, y, kTransparent, false);
  }

  img.colorFuzz(0.0); // Reset so later operations aren't affected
}

// ─────────────────────────────────────────────
//  Resize helper  (fit + center-pad)
// ─────────────────────────────────────────────

/**
 * @brief Scales @p src to fit within @p size × @p size, then centers it on a
 *        transparent canvas of exactly that size.
 */
[[nodiscard]]
Magick::Image fit_and_pad(const Magick::Image &src, std::size_t size) {
  Magick::Image copy = src;

  // Fit inside the box, keeping aspect ratio
  Magick::Geometry fit(size, size);
  fit.aspect(false);
  copy.resize(fit);

  // Extend to exact canvas size (centers the image, fills with transparent)
  Magick::Geometry canvas(size, size);
  copy.alpha(true);
  copy.backgroundColor(Magick::Color("transparent"));
  copy.extent(canvas, Magick::CenterGravity);

  return copy;
}

// ─────────────────────────────────────────────
//  Writers
// ─────────────────────────────────────────────

/**
 * @brief Writes a multi-resolution *.ico file.
 *        Standard sizes: 16, 32, 48, 64, 256 px.
 */
void write_ico(const Magick::Image &src, const fs::path &dest) {
  static constexpr std::array<std::size_t, 5> kIcoSizes{16, 32, 48, 64, 256};

  std::vector<Magick::Image> frames;
  frames.reserve(kIcoSizes.size());

  for (const std::size_t sz : kIcoSizes) {
    Magick::Image frame = fit_and_pad(src, sz);
    frame.magick("ICO");
    frames.push_back(std::move(frame));
  }

  Magick::writeImages(frames.begin(), frames.end(), dest.string());
}

/**
 * @brief Writes a PNG resized to exactly @p size × @p size.
 */
void write_png(const Magick::Image &src, std::size_t size,
               const fs::path &dest) {
  Magick::Image img = fit_and_pad(src, size);
  img.magick("PNG");
  img.write(dest.string());
}

/**
 * @brief Writes an SVG that embeds the full-resolution image as a base64
 *        PNG data-URI.  This preserves every pixel without lossy re-encoding.
 */
void write_svg(const Magick::Image &src, const fs::path &dest) {
  // Export source as PNG blob
  Magick::Image copy = src;
  copy.magick("PNG");
  Magick::Blob blob;
  copy.write(&blob);

  const auto *raw = static_cast<const std::uint8_t *>(blob.data());
  const std::string b64 = base64_encode({raw, blob.length()});

  const std::size_t w = copy.columns();
  const std::size_t h = copy.rows();

  // Emit SVG with embedded raster; viewBox preserves intrinsic dimensions
  const std::string svg = std::format(
      R"(<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg"
     xmlns:xlink="http://www.w3.org/1999/xlink"
     width="{0}" height="{1}"
     viewBox="0 0 {0} {1}">
  <title>Converted Logo</title>
  <image x="0" y="0"
         width="{0}" height="{1}"
         image-rendering="optimizeQuality"
         xlink:href="data:image/png;base64,{2}"/>
</svg>
)",
      w, h, b64);

  std::ofstream out(dest);
  if (!out) {
    throw std::runtime_error(
        std::format("Cannot open for writing: {}", dest.string()));
  }
  out << svg;
}

// ─────────────────────────────────────────────
//  Entry point
// ─────────────────────────────────────────────
int main(int argc, char **argv) {
  Magick::InitializeMagick(*argv);

  CLI::App app{"img2icon — Convert JPG/PNG to ICO, PNG variants, and SVG\n"
               "Requires Magick++ (ImageMagick ≥ 7) to be installed."};

  fs::path input_path;
  fs::path output_dir;
  std::string stem = "logo";
  bool no_bg = false;
  double fuzz = 15.0;

  app.add_option("-i,--input", input_path,
                 "Input image (*.jpg | *.jpeg | *.png)")
      ->required()
      ->check(CLI::ExistingFile);

  app.add_option("-o,--output", output_dir,
                 "Output directory (created if absent)")
      ->required();

  app.add_option("-n,--name", stem,
                 "Base filename for all outputs  [default: logo]");

  app.add_flag("--no-bg", no_bg, "Remove background — make it transparent");

  app.add_option("--fuzz", fuzz,
                 "Background-removal tolerance in % (0–100)  [default: 15]")
      ->check(CLI::Range(0.0, 100.0));

  CLI11_PARSE(app, argc, argv);

  // ── Validate extension ──────────────────────────────────────────────────
  std::string ext = input_path.extension().string();
  std::ranges::transform(ext, ext.begin(),
                         [](unsigned char c) { return std::tolower(c); });

  if (ext != ".jpg" && ext != ".jpeg" && ext != ".png") {
    std::println(stderr,
                 "Error: only *.jpg, *.jpeg and *.png inputs are supported.");
    return 1;
  }

  // ── Prepare output directory ────────────────────────────────────────────
  if (!fs::exists(output_dir)) {
    std::println("Creating output directory: {}", output_dir.string());
    fs::create_directories(output_dir);
  }

  try {
    // ── Load ──────────────────────────────────────────────────────────────
    std::println("Loading:  {}", input_path.string());
    Magick::Image img;
    img.read(input_path.string());

    // ── Optional: remove background ───────────────────────────────────────
    if (no_bg) {
      std::println("Removing background (fuzz: {:.1f}%) …", fuzz);
      remove_background(img, fuzz);
    }

    // ── ICO (multi-resolution: 16, 32, 48, 64, 256 px) ───────────────────
    const auto ico_path = output_dir / (stem + ".ico");
    std::println("Writing:  {}", ico_path.string());
    write_ico(img, ico_path);

    // ── PNG variants ──────────────────────────────────────────────────────
    static constexpr std::array<std::pair<std::size_t, std::string_view>, 3>
        kPngSizes{{
            {92, "92x92"},
            {256, "256x256"},
            {512, "512x512"},
        }};

    for (auto [size, label] : kPngSizes) {
      const auto png_path = output_dir / std::format("{}_{}.png", stem, label);
      std::println("Writing:  {}", png_path.string());
      write_png(img, size, png_path);
    }

    // ── SVG (embedded base64 PNG data-URI) ────────────────────────────────
    const auto svg_path = output_dir / (stem + ".svg");
    std::println("Writing:  {}", svg_path.string());
    write_svg(img, svg_path);

    std::println("\nDone — {} files written to: {}",
                 2 + kPngSizes.size(), // ico + svg + pngs
                 output_dir.string());

  } catch (const Magick::Exception &e) {
    std::println(stderr, "ImageMagick error: {}", e.what());
    return 1;
  } catch (const std::exception &e) {
    std::println(stderr, "Error: {}", e.what());
    return 1;
  }

  return 0;
}
