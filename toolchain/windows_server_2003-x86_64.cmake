# Copyright (c) 2017, 2018, Xianguang Zhou <xianguang.zhou@outlook.com>. All rights reserved.
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/toolchain")
include(windows-x86_64)
add_definitions(-D_WIN32_WINNT=0x0502)
