cmake_minimum_required(VERSION 3.13)
project(ad-part VERSION 0.0.1)

# 设置项目包含目录、库目录和可执行文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
link_directories(${PROJECT_SOURCE_DIR}/third_party/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# PDG 数据生成和编译
if(NOT PDG_PATH)
    set(PDG_PATH ${PROJECT_SOURCE_DIR}/share/cache)
endif()
execute_process(COMMAND ${PROJECT_SOURCE_DIR}/bin/cache ${PDG_PATH} RESULT_VARIABLE RETCODE)
if(NOT RETCODE EQUAL 0)
    message(FATAL_ERROR "error generating pdginfo")
endif()
add_compile_definitions(PDG_PATH="${PDG_PATH}")

# 优化、调试和通用编译选项
option(ENABLE_SANITIZER "compile and link with sanitizing flags" OFF)
option(ENABLE_GPROF "compile and link with gprof flags" OFF)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif()
if(CMAKE_BUILD_TYPE STREQUAL DEBUG)
    add_compile_options(-O0 -fno-omit-frame-pointer)
    if(ENABLE_SANITIZER)
        set(SANITIZER_OPTIONS -fno-sanitize-recover=all)
        list(APPEND SANITIZER_OPTIONS -fsanitize=address)
        list(APPEND SANITIZER_OPTIONS -fsanitize=leak)
        list(APPEND SANITIZER_OPTIONS -fsanitize=undefined)
        list(APPEND SANITIZER_OPTIONS -fsanitize=float-divide-by-zero)
        add_compile_options(${SANITIZER_OPTIONS})
        add_link_options(${SANITIZER_OPTIONS})
    endif()
else()
    add_compile_options(-O3)
endif()
if(ENABLE_GPROF)
    set(GPROF_OPTIONS -pg)
    add_compile_options(${GPROF_OPTIONS})
    add_link_options(${GPROF_OPTIONS})
endif()
add_compile_options(-Wall -Wshadow -Wextra)
add_compile_definitions(_GNU_SOURCE)

# 以下为动态库文件构建

add_library(particle SHARED src/particle.cpp)

# 以下为可执行文件构建

add_executable(particle-example src/particle-example.cpp)
target_link_libraries(particle-example PUBLIC particle)
