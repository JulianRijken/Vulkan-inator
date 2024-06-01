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
        glm::mat4 viewProjection;
        glm::vec4 viewPosition;
        int renderMode;
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
    Mesh& AddMesh3D_Cartoon(const std::string& name, Mesh&& mesh)
    {
        return *(m_Meshes3D_Cartoon[name] = std::make_unique<Mesh>(std::move(mesh)));
    }

    Mesh& AddMesh3D_Unlit(const std::string& name, Mesh&& mesh)
    {
        return *(m_Meshes3D_Unlit[name] = std::make_unique<Mesh>(std::move(mesh)));
    }

    Mesh& AddMesh3D(const std::string& name, Mesh&& mesh)
    {
        return *(m_Meshes3D[name] = std::make_unique<Mesh>(std::move(mesh)));
    }

    Mesh& AddMesh2D(const std::string& name, Mesh&& mesh)
    {
        return *(m_Meshes2D[name] = std::make_unique<Mesh>(std::move(mesh)));
    }

    Mesh LoadMesh(const std::string& meshPath, Material* material);
    Mesh GenerateCircle(glm::vec2 center, glm::vec2 size = { 1, 1 }, uint32_t segmentSize = 64);

    std::unique_ptr<Pipeline> m_Pipline2D{};
    std::unique_ptr<Pipeline> m_Pipline3D{};
    std::unique_ptr<Pipeline> m_Pipline3D_Unlit{};
    std::unique_ptr<Pipeline> m_Pipline3D_Cartoon{};


    Camera m_Camera{
        glm::vec3{0, 1, -2},
        80
    };

    int m_RenderMode{};

    float m_Time;

    std::unordered_map<std::string, std::unique_ptr<Mesh>> m_Meshes2D{};
    std::unordered_map<std::string, std::unique_ptr<Mesh>> m_Meshes3D{};
    std::unordered_map<std::string, std::unique_ptr<Mesh>> m_Meshes3D_Unlit{};
    std::unordered_map<std::string, std::unique_ptr<Mesh>> m_Meshes3D_Cartoon{};
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_Textures;
    std::unordered_map<std::string, std::unique_ptr<Material>> m_Materials;
};
