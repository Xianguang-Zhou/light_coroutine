# Copyright (c) 2017, 2018, Xianguang Zhou <xianguang.zhou@outlook.com>. All rights reserved.
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(light_coroutine)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost is not found.")
endif()

install(FILES light_coroutine.h light_coroutine.hpp DESTINATION include)

aux_source_directory(. CPP_SRC_LIST)
set(C_SRC_LIST ${CPP_SRC_LIST})
list(REMOVE_ITEM C_SRC_LIST "./light_coroutine.cpp")

set(C_LIBRARY_NAME ${PROJECT_NAME})
add_library(${C_LIBRARY_NAME} SHARED ${C_SRC_LIST})

set(C_STATIC_LIBRARY_NAME "${C_LIBRARY_NAME}_static")
add_library(${C_STATIC_LIBRARY_NAME} STATIC ${C_SRC_LIST})

set(CPP_LIBRARY_NAME "${PROJECT_NAME}_cpp")
add_library(${CPP_LIBRARY_NAME} SHARED ${CPP_SRC_LIST})

set(CPP_STATIC_LIBRARY_NAME "${CPP_LIBRARY_NAME}_static")
add_library(${CPP_STATIC_LIBRARY_NAME} STATIC ${CPP_SRC_LIST})

install(TARGETS ${C_LIBRARY_NAME} ${C_STATIC_LIBRARY_NAME} ${CPP_LIBRARY_NAME} ${CPP_STATIC_LIBRARY_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
