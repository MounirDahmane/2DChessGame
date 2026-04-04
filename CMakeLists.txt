cmake_minimum_required(VERSION 3.15)
project(E2E4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point to local SFML
set(SFML_DIR "${CMAKE_SOURCE_DIR}/thirdparty/SFML-2.6.1/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# Include the headers folder
include_directories(include)

# Automatically grab all .cpp files directly inside the src/ folder
file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(ChessGame ${SOURCES})

if(WIN32)
    target_compile_definitions(ChessGame PRIVATE SFML_STATIC)
endif()

target_link_libraries(ChessGame PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# Sync assets
add_custom_command(TARGET ChessGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets")