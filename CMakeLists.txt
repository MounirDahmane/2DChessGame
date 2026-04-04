cmake_minimum_required(VERSION 3.15)
project(E2E4 LANGUAGES CXX)

# ============================================================================
# COMPILER SETTINGS
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# DEPENDENCIES (SFML)
# ============================================================================
# Point to local SFML installation
set(SFML_DIR "${CMAKE_SOURCE_DIR}/thirdparty/SFML-2.6.1/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# ============================================================================
# PROJECT STRUCTURE
# ============================================================================
# Include the headers folder
include_directories(include)

# Automatically grab all .cpp files directly inside the src/ folder
# CONFIGURE_DEPENDS ensures CMake re-runs if files are added or removed
file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# ============================================================================
# EXECUTABLE TARGET
# ============================================================================
add_executable(ChessGame ${SOURCES})

# Windows-specific static linking definitions
if(WIN32)
    target_compile_definitions(ChessGame PRIVATE SFML_STATIC)
endif()

# Link SFML libraries to the executable
target_link_libraries(ChessGame PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system 
    sfml-audio
)

# ============================================================================
# ASSET MANAGEMENT (POST-BUILD)
# ============================================================================
# Copy the assets directory to the build folder so the executable can find them
add_custom_command(TARGET ChessGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets"
    COMMENT "Syncing assets to build directory..."
)