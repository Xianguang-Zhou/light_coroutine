project(light_coroutine)
cmake_minimum_required(VERSION 2.6)

aux_source_directory(. SRC_LIST)
install(FILES light_coroutine.h DESTINATION include)

set(LIBRARY_NAME ${PROJECT_NAME})
add_library(${LIBRARY_NAME} SHARED ${SRC_LIST})

set(STATIC_LIBRARY_NAME "${LIBRARY_NAME}_static")
add_library(${STATIC_LIBRARY_NAME} STATIC ${SRC_LIST})

install(TARGETS ${LIBRARY_NAME} ${STATIC_LIBRARY_NAME} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
