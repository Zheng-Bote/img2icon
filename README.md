<div id="top" align="center">
<h1>img2icon</h1>

<p>Converts a JPG or PNG into all common icon/logo formats in one step.</p>

[Report Issue](https://github.com/Zheng-Bote/img2icony/issues) · [Request Feature](https://github.com/Zheng-Bote/img2icon/pulls)

![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey.svg)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Zheng-Bote/img2icon?logo=GitHub)](https://github.com/Zheng-Bote/img2icon/releases)

</div>

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Description

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)]()
[![CMake](https://img.shields.io/badge/CMake-3.23+-blue.svg)]()
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey.svg)

Converts a **JPG** or **PNG** into all common icon/logo formats in one step.

| Output              | Description                                       |
| ------------------- | ------------------------------------------------- |
| `logo.ico`          | Multi-resolution ICO (16 · 32 · 48 · 64 · 256 px) |
| `logo_92x92.png`    | PNG — app icon, small                             |
| `logo_256x256.png`  | PNG — app icon, medium                            |
| `logo_512x512.png`  | PNG — app icon, large                             |
| `logo_512x512.webp` | WebP — app icon, large (default quality 85)       |
| `logo.svg`          | SVG wrapper with embedded base64 PNG data-URI     |

All PNG/ICO outputs are **center-padded** to exact square dimensions
(aspect ratio is preserved, excess area is transparent).

---

## Prerequisites

| Dependency      | macOS                      | Ubuntu / Debian                    |
| --------------- | -------------------------- | ---------------------------------- |
| ImageMagick ≥ 7 | `brew install imagemagick` | `sudo apt install libmagick++-dev` |
| CMake ≥ 3.25    | `brew install cmake`       | `sudo apt install cmake`           |
| C++23 compiler  | Xcode 15 / clang 17        | GCC 14 / clang 17                  |
| CLI11           | auto-fetched by CMake      | auto-fetched by CMake              |

---

## Build

### Option A — CMake (recommended)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
# optional:
sudo cmake --install build
```

### Option B — Single-command (no CMake)

```bash
c++ -std=c++23 \
    $(Magick++-config --cxxflags --libs) \
    -ICLI11/include \
    img2icon.cpp -o img2icon
```

> Grab CLI11 header: `curl -Lo CLI11/include/CLI/CLI.hpp \`
> `https://github.com/CLIUtils/CLI11/releases/download/v2.4.2/CLI11.hpp`

---

## Usage

```
img2icon [OPTIONS]

Options:
  -i, --input    PATH   Input image *.jpg | *.jpeg | *.png   [required]
  -o, --output   DIR    Output directory (created if absent) [required]
  -n, --name     NAME   Base filename for outputs            [default: logo]
      --no-bg           Remove background → transparent
      --fuzz     0-100  Tolerance for background removal     [default: 15]
  --webp-quality 0-100  WebP quality                         [default: 85]
  -h, --help            Show this help
```

### Examples

```bash
# Basic conversion
./img2icon -i photo.jpg -o ./icons

# Custom base name
./img2icon -i brand.png -o ./dist -n myapp

# Transparent background (e.g. white background photo)
./img2icon -i logo.png -o ./icons --no-bg

# Aggressive background removal for noisy scans
./img2icon -i scan.jpg -o ./icons --no-bg --fuzz 25
```

---

## Background removal notes

`--no-bg` performs a **corner flood-fill**: it samples each of the four
corner pixels and flood-fills outward using the specified `--fuzz` tolerance.
This works well for images with a solid or near-solid background colour.

- `--fuzz 0` — exact colour match only (rarely useful)
- `--fuzz 15` — default; good for clean white/solid backgrounds
- `--fuzz 25+` — for scanned or slightly graduated backgrounds

For complex backgrounds (photos, gradients), consider pre-processing the
image in a dedicated editor before passing it to `img2icon`.

---

## SVG output

The `.svg` file embeds the full-resolution source as a **base64 PNG
data-URI**. This is a _raster-in-vector_ format — it is not a true vector
graphic, but it is valid, self-contained SVG that renders at any scale in
browsers and design tools without external file dependencies.

---

<p align="right">(<a href="#top">back to top</a>)</p>

# Authors and License

**License**
Distributed under the MIT License. See LICENSE for more information.

Copyright (c) 2026 ZHENG Robert

**Authors**

- [![Zheng Robert - Core Development](https://img.shields.io/badge/Github-Zheng_Robert-black?logo=github)](https://www.github.com/Zheng-Bote)

### Code Contributors

![Contributors](https://img.shields.io/github/contributors/Zheng-Bote/web-gallery_webserver-admin?color=dark-green)

[![Zheng Robert](https://img.shields.io/badge/Github-Zheng_Robert-black?logo=github)](https://www.github.com/Zheng-Bote)

---

:vulcan_salute:
