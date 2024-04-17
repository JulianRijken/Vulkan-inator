#pragma once

#include "vulkan/vulkan_core.h"


class RenderPass
{
public:
    RenderPass(VkDevice device, VkFormat swapChainImageFormat);
    ~RenderPass();

    void Begin(VkFramebuffer swapChainFramebuffers, VkExtent2D swapChainExtent, VkCommandBuffer commandBuffer);
    void End(VkCommandBuffer commandBuffer);

    operator VkRenderPass();

private:
    VkRenderPass m_RenderPass{};
    VkDevice m_Divice;
};
