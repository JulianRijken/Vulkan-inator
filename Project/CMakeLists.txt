# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)


file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)


set(IMGUI_SOURCE_FILES
    ${EXTERNAL_DIR}/imgui/imgui_demo.cpp

    ${EXTERNAL_DIR}/imgui/imgui.cpp
    ${EXTERNAL_DIR}/imgui/imgui_draw.cpp
    ${EXTERNAL_DIR}/imgui/imgui_widgets.cpp
    ${EXTERNAL_DIR}/imgui/imgui_tables.cpp

    ${EXTERNAL_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${EXTERNAL_DIR}/imgui/backends/imgui_impl_vulkan.cpp
)


set(SOURCES
    main.cpp
    vulkanbase/VulkanUtil.cpp  vulkanbase/VulkanUtil.h
    vulkanbase/VulkanBase.cpp  vulkanbase/VulkanBase.h
    vulkanbase/VulkanGlobals.h

    ${EXTERNAL_DIR}/HeaderLibs.cpp
    ${EXTERNAL_DIR}/stb/stb_image.h

    jul/GUI.h               jul/GUI.cpp
    jul/Texture.h           jul/Texture.cpp
    jul/Pipeline.cpp        jul/Pipeline.h
    jul/Shader.cpp          jul/Shader.h
    jul/CommandBuffer.cpp   jul/CommandBuffer.h
    jul/Mesh.cpp            jul/Mesh.h
                            jul/Pipeline.h
    jul/RenderPass.cpp      jul/RenderPass.h
    jul/SwapChain.cpp       jul/SwapChain.h
    jul/Buffer.cpp          jul/Buffer.h
    jul/DescriptorPool.cpp  jul/DescriptorPool.h
    jul/Camera.cpp          jul/Camera.h
    jul/Input.cpp           jul/Input.h
    jul/GameTime.cpp        jul/GameTime.h
    jul/Game.cpp            jul/Game.h
                            jul/MathExtensions.h
    jul/Material.h          jul/Material.cpp
)

# Create the executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${IMGUI_SOURCE_FILES}
    ${GLSL_SOURCE_FILES})

# target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wpedantic)

add_dependencies(${PROJECT_NAME} Shaders)
# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw glm::glm tinyobjloader)


if(WIN32)
    set(SYMLINK_COMMAND copy_directory)
else()
    set(SYMLINK_COMMAND create_symlink)
endif()

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                COMMAND ${CMAKE_COMMAND} -E ${SYMLINK_COMMAND}
                "${CMAKE_SOURCE_DIR}/resources"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources")

