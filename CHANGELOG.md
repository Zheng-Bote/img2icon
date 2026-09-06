<!-- DOCTOC SKIP -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2026-09-06

### Added

- Added support for HEIF (`.heic`) input images.
- Implemented image caching during processing to halve the number of resizing operations, resulting in significantly faster conversions.
- SVG output optimization: Automatically scales down excessively large images to a maximum of 512x512 pixels before embedding them in SVG, preventing bloated file sizes.
- Restructured project layout to standard C++ directories (`src/` for source files, `include/` for headers).
- Modernized internal code using C++23 features (`std::expected` for error handling, `std::views` for string transformations) and generalized image writing functions.

## [1.1.0] - 2026-04-03

### Added

- AVIF support: The tool now generates AVIF variants (92x92, 256x256, 512x512) by default.
- `--avif-quality` option: Control AVIF output quality (default: 75).

## [1.0.0] - 2026-03-14

### Added

- `-o / --output` option to specify the output directory.
- `img2icon` executable installation instructions to system directories via CMake (`make install`).
- CLI options `-v` and `--version` for returning application and tool version respectively.
- Background removal (`--no-bg` along with customizable `--fuzz`).
- Automated multi-resolution conversion outputs `.ico`, `.svg`, `.png` combinations (92x92, 256x256, 512x512) and `.webp`.
