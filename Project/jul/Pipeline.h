#pragma once


#include "jul/DescriptorPool.h"
#include "jul/Shader.h"

class Pipeline
{
public:
    Pipeline(const Shader& shader, VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateInfo,
             VkDeviceSize uboSize, uint32_t pushConstantSize = 0, VkCullModeFlagBits cullMode = VK_CULL_MODE_FRONT_BIT,
             VkBool32 depthTestEnable = VK_TRUE, VkBool32 depthWriteEnable = VK_TRUE);

    ~Pipeline();

    void Bind(VkCommandBuffer commandBuffer, int imageIndex);
    void UpdateUBO(int imageIndex, void* uboData, VkDeviceSize uboSize);
    void UpdatePushConstant(VkCommandBuffer commandBuffer, void* pushConstants, uint32_t pushConstantSize);

private:
    void CreateDescriptorSetLayout();
    void CreateUniformbuffers(int maxFramesCount, VkDeviceSize uboBufferSize);


    VkPipeline m_Pipeline{};
    VkPipelineLayout m_PipelineLayout{};

    VkDescriptorSetLayout m_DescriptorSetlayout{};

    std::vector<std::unique_ptr<Buffer>> m_UniformBuffers{};
    std::unique_ptr<DescriptorPool> m_DescriptorPoolUPtr;

    VkRenderPass m_RenderPass;
};
