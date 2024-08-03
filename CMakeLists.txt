cmake_minimum_required(VERSION 3.10)

# Set the project name
project(EventRingProject)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add sub directories
add_subdirectory(src)
add_subdirectory(tests)

# Find nlohmann_json library
find_package(nlohmann_json 3.11.3 REQUIRED)

# Add include directories
include_directories(${nlohmann_json_INCLUDE_DIRS})

# Link nlohmann_json library
target_link_libraries(EventRingProject nlohmann_json::nlohmann_json)
