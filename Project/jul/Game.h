#pragma once
#include <vulkan/vulkan_core.h>

#include "Camera.h"
#include "Mesh.h"
#include "Pipeline.h"

class Game final
{
    struct UniformBufferObject2D
    {
        glm::mat4 proj;
    };

    struct UniformBufferObject3D
    {
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct MeshPushConstants
    {
        glm::mat4 model;
    };

    static_assert(sizeof(MeshPushConstants) <= 128);


public:
    Game();
    ~Game();

    void Update();
    void Draw(VkCommandBuffer commandBuffer, int imageIndex);
    void OnResize();

private:
    const Mesh& AddMesh(Mesh&& mesh) { return m_Meshes.emplace_back(std::move(mesh)); }
    Mesh LoadMesh(const std::string& meshPath);
    Mesh GenerateCircle(glm::vec2 center, glm::vec2 size = { 1, 1 }, uint32_t segmentSize = 64);

    std::unique_ptr<Pipeline> m_Pipline2D{};
    std::unique_ptr<Pipeline> m_Pipline3D{};


    Camera m_Camera{
        glm::vec3{0, 1, -2},
        80
    };

    std::vector<Mesh> m_Meshes{};
    std::unique_ptr<Texture> m_TestTexture;
};
