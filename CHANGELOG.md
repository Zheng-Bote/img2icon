<!-- DOCTOC SKIP -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-03-14

### Added

- `-o / --output` option to specify the output directory.
- `img2icon` executable installation instructions to system directories via CMake (`make install`).
- CLI options `-v` and `--version` for returning application and tool version respectively.
- Background removal (`--no-bg` along with customizable `--fuzz`).
- Automated multi-resolution conversion outputs `.ico`, `.svg`, `.png` combinations (92x92, 256x256, 512x512) and `.webp`.
