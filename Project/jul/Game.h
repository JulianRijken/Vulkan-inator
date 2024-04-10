#pragma once
#include <vulkan/vulkan_core.h>

#include "Camera.h"
#include "Pipeline.h"

class Game final
{
public:
    Game();

    void Update();
    void Draw(VkCommandBuffer commandBuffer, int imageIndex);

private:
    std::unique_ptr<Pipeline> m_Pipline2D{};
    std::unique_ptr<Pipeline> m_Pipline3D{};


    Camera m_Camera{
        glm::vec3{0, 1, -2},
        80
    };

    // std::vector<Mesh> m_Meshes{};
};
