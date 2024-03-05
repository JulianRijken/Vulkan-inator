cmake_minimum_required(VERSION 3.11)

# Project Name
project(VulkanLab01)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_SHALLOW TRUE
    GIT_TAG        3.3.8
)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_SHALLOW TRUE
	GIT_TAG 	1.0.1
)

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(glm)

# Find the required packages
find_package(Vulkan REQUIRED)

# Include Directories
include_directories(${Vulkan_INCLUDE_DIRS})

add_subdirectory(Project)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# If using validation layers, copy the required JSON files (optional)
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        "${CMAKE_SOURCE_DIR}/path_to_json/VK_LAYER_KHRONOS_validation.json"
#        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
