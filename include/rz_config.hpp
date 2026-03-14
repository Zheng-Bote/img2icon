/**
 * @file config.hpp.in
 * @author ZHENG Robert (robert.hase-zheng.net)
 * @brief configuration
 * @version 2.0.0
 * @date 2025-11-15
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 */
#pragma once

#include <cstdint>
#include <string>

static const std::string PROJECT_NAME = "img2icon";
static const std::string PROG_LONGNAME = "Convert a JPG or PNG into all common icon/logo formats in one step.";
static const std::string PROJECT_DESCRIPTION = "Convert a JPG or PNG into all common icon/logo formats in one step.";

static const std::string PROJECT_EXECUTABLE = "img2icon";

static const std::string PROJECT_VERSION = "1.0.0";
static const std::int32_t PROJECT_VERSION_MAJOR { 1 };
static const std::int32_t PROJECT_VERSION_MINOR { 0 };
static const std::int32_t PROJECT_VERSION_PATCH { 0 };

static const std::string PROJECT_HOMEPAGE_URL = "https://github.com/Zheng-Bote/img2icon";
static const std::string PROG_AUTHOR = "ZHENG Bote";
static const std::string PROG_CREATED = "2026";
static const std::string PROG_ORGANIZATION_NAME = "ZHENG Robert";
static const std::string PROG_ORGANIZATION_DOMAIN =
    "net.hase-zheng";

static const std::string CMAKE_CXX_STANDARD = "c++23";
static const std::string CMAKE_CXX_COMPILER =
    "GNU 15.2.0";
static const std::string CMAKE_QT_VERSION = "";
