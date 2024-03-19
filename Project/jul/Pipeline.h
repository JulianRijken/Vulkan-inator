#pragma once

#include "jul/Mesh.h"
#include "jul/Shader.h"
#include "vulkan/vulkan_core.h"

class Pipeline
{
public:
    Pipeline();
    ~Pipeline();
    void Record(const VkExtent2D& swapChainExtent,VkFramebuffer swapChainFramebuffers, VkCommandBuffer commandBuffer, uint32_t imageIndex);

private:

    void DrawScene(VkCommandBuffer commandBuffer);

    VkRenderPass m_RenderPass;
    VkPipeline m_Pipeline{};
    Shader m_Shader{};
    std::vector<Mesh> m_Meshes{};

};

