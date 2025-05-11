cmake_minimum_required(VERSION 3.25)
project(flappy-bird)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)

# Check compiler vendor and version
if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_C_COMPILER_VERSION VERSION_LESS "18.0")
        message(FATAL_ERROR "Clang 18 or newer is required for full C23 support (nullptr). Found: ${CMAKE_C_COMPILER_VERSION}")
    endif()
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_C_COMPILER_VERSION VERSION_LESS "14.0")
        message(FATAL_ERROR "GCC 14 or newer is required for full C23 support (nullptr). Found: ${CMAKE_C_COMPILER_VERSION}")
    endif()
elseif (CMAKE_C_COMPILER_ID STREQUAL "Intel")
    message(FATAL_ERROR "Intel compilers are not yet verified for full C23 nullptr support.")
else()
    message(FATAL_ERROR "Unsupported or unknown compiler: ${CMAKE_C_COMPILER_ID}. Full C23 support required.")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")
include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_mixer REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME}
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
    SDL3_ttf
)
