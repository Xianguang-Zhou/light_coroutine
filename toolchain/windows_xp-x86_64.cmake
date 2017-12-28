list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/toolchain")
include(windows-x86_64)
add_definitions(-D_WIN32_WINNT=0x0501)
