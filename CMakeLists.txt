cmake_minimum_required(VERSION 3.13)

# Project Name
project(VulkanLab01)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY      https://github.com/glfw/glfw.git
    GIT_SHALLOW TRUE
    GIT_TAG 3.3.8
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY	https://github.com/g-truc/glm.git
    GIT_SHALLOW TRUE
    GIT_TAG 1.0.1
)

FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY	https://github.com/tinyobjloader/tinyobjloader.git
    GIT_SHALLOW TRUE
    GIT_TAG v2.0.0rc13
)


FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(tinyobjloader)

# Find the required packages
find_package(Vulkan REQUIRED)

# Include Directories
include_directories(${Vulkan_INCLUDE_DIRS})

add_subdirectory(Project)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()
