#pragma once

#include "jul/Mesh.h"
#include "jul/Shader.h"

class Pipeline
{
public:
    Pipeline(VkDevice device, VkRenderPass m_RenderPass);
    ~Pipeline();

    void Draw(const VkExtent2D& swapChainExtent,const std::vector<VkFramebuffer>& swapChainFramebuffers, VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void InitScene(VkPhysicalDevice physicalDevice,VkQueue graphicsQueue);

private:

    void DrawScene(VkCommandBuffer commandBuffer);

    void createGraphicsPipeline();


    VkPipeline m_Pipeline{};
    VkPipelineLayout m_PipelineLayout{};
    std::vector<Mesh> m_Meshes{};

    VkRenderPass m_RenderPass;
    Shader m_GradientShader;

    VkDevice m_Device;
};

