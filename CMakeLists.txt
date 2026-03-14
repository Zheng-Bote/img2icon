cmake_minimum_required(VERSION 3.25)
project(img2icon
  VERSION 1.0.0
  DESCRIPTION "Convert a JPG or PNG into all common icon/logo formats in one step."
  HOMEPAGE_URL "https://github.com/Zheng-Bote/img2icon"
  LANGUAGES CXX
)

# ── C++23 ────────────────────────────────────────────────────────────────────
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# --- Configure (für rz_config.hpp) ---
set(EXECUTABLE_NAME "${PROJECT_NAME}")
set(PROG_LONGNAME "Convert a JPG or PNG into all common icon/logo formats in one step.")
set(PROG_CREATED "2026")
set(PROG_AUTHOR "ZHENG Bote")
set(PROG_ORGANIZATION_NAME "ZHENG Robert")
set(PROG_ORGANIZATION_DOMAIN "net.hase-zheng")
add_subdirectory(configure) 

# ── Magick++ — try all known pkg-config names in order ───────────────────────
#
#   Ubuntu / Debian package names (libmagick++-dev):
#     ImageMagick 6:  Magick++-6.Q16HDRI  |  Magick++-6.Q16
#     ImageMagick 7:  Magick++-7.Q16HDRI  |  Magick++-7.Q16  |  Magick++
#
find_package(PkgConfig REQUIRED)

set(_MAGICK_PC_NAMES
    Magick++-7.Q16HDRI
    Magick++-7.Q16
    Magick++-6.Q16HDRI
    Magick++-6.Q16
    Magick++
    ImageMagick++
)

set(MAGICKPP_FOUND FALSE)
foreach(_name IN LISTS _MAGICK_PC_NAMES)
  pkg_check_modules(MAGICKPP QUIET ${_name})
  if(MAGICKPP_FOUND)
    message(STATUS "Found Magick++ via pkg-config name: ${_name}")
    add_library(Magick++::Magick++ INTERFACE IMPORTED)
    target_compile_options(Magick++::Magick++ INTERFACE ${MAGICKPP_CFLAGS})
    target_link_libraries (Magick++::Magick++ INTERFACE ${MAGICKPP_LDFLAGS})
    break()
  endif()
endforeach()

# Last-resort fallback: ask Magick++-config directly
if(NOT MAGICKPP_FOUND)
  find_program(MAGICKPP_CONFIG NAMES Magick++-config MagickCore-config)
  if(MAGICKPP_CONFIG)
    execute_process(COMMAND ${MAGICKPP_CONFIG} --cxxflags
                    OUTPUT_VARIABLE _MC_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${MAGICKPP_CONFIG} --libs
                    OUTPUT_VARIABLE _MC_LIBS     OUTPUT_STRIP_TRAILING_WHITESPACE)

    separate_arguments(_MC_CXXFLAGS_LIST UNIX_COMMAND "${_MC_CXXFLAGS}")
    separate_arguments(_MC_LIBS_LIST     UNIX_COMMAND "${_MC_LIBS}")

    add_library(Magick++::Magick++ INTERFACE IMPORTED)
    target_compile_options(Magick++::Magick++ INTERFACE ${_MC_CXXFLAGS_LIST})
    target_link_libraries (Magick++::Magick++ INTERFACE ${_MC_LIBS_LIST})

    set(MAGICKPP_FOUND TRUE)
    message(STATUS "Found Magick++ via config script: ${MAGICKPP_CONFIG}")
  endif()
endif()

if(NOT MAGICKPP_FOUND)
  message(FATAL_ERROR
    "Magick++ not found. Install it with:\n"
    "  Ubuntu/Debian:  sudo apt install libmagick++-dev\n"
    "  macOS (Homebrew): brew install imagemagick\n"
    "Then re-run cmake.")
endif()

# ── CLI11 — header-only ───────────────────────────────────────────────────────
find_package(CLI11 QUIET)
if(NOT CLI11_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG        v2.4.2
  )
  FetchContent_MakeAvailable(CLI11)
endif()

# ── Target ───────────────────────────────────────────────────────────────────
add_executable(img2icon img2icon.cpp)

target_link_libraries(img2icon
  PRIVATE
    Magick++::Magick++
    CLI11::CLI11
)

target_compile_options(img2icon PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

# ── Install (optional) ───────────────────────────────────────────────────────
include(GNUInstallDirs)
install(TARGETS img2icon DESTINATION ${CMAKE_INSTALL_BINDIR})


# CMake File API Query (Datei erzeugen)
set(QUERY_DIR "${CMAKE_BINARY_DIR}/.cmake/api/v1/query/client-project")
file(MAKE_DIRECTORY "${QUERY_DIR}")
file(WRITE "${QUERY_DIR}/codemodel-v2" "") 

# libs.txt Generierung (Timing-sicher via GENERATE)
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/libs.txt" 
     CONTENT "$<TARGET_PROPERTY:${PROJECT_NAME},LINK_LIBRARIES>")

