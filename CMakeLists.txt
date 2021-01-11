cmake_minimum_required(VERSION 3.0)
project(sylar)

include(cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -fPIC -ggdb -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

include_directories(.)

find_library(YAMLCPP yaml-cpp)
message("***", ${YAMLCPP})

set(LIB_SRC
    sylar/log.cc
    sylar/util.cc
    sylar/config.cc
    sylar/thread.cc
    sylar/fiber.cc
    )

add_library(sylar SHARED ${LIB_SRC})
redefine_file_macro(sylar)
#add_library(sylar_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (sylar_static PROPERTIES OUTPUT_NAME "sylar")

set(LIBS
    sylar
    pthread
    ${YAMLCPP}
    )

add_executable(main tests/test.cc)
add_dependencies(main sylar)
redefine_file_macro(main)
target_link_libraries(main sylar ${LIBS})

add_executable(main_config tests/test_config.cc)
add_dependencies(main_config sylar)
redefine_file_macro(main_config)
target_link_libraries(main_config sylar ${LIBS})

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread sylar)
redefine_file_macro(test_thread)
target_link_libraries(test_thread sylar ${LIBS})

add_executable(test_util tests/test_util.cc)
add_dependencies(test_util sylar)
redefine_file_macro(test_util)
target_link_libraries(test_util sylar ${LIBS})

add_executable(test_fiber tests/test_fiber.cc)
add_dependencies(test_fiber sylar)
redefine_file_macro(test_fiber)
target_link_libraries(test_fiber sylar ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)