#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "vulkan/vulkan_core.h"

#include <vector>

class RenderPass
{
public:
    RenderPass(VkDevice device,VkFormat swapChainImageFormat);
    ~RenderPass();

    void Begin(std::vector<VkFramebuffer> swapChainFramebuffers,int imageIndex,VkExtent2D swapChainExtent,VkCommandBuffer commandBuffer);
    void End(VkCommandBuffer commandBuffer);

    operator VkRenderPass();

private:

    VkRenderPass m_RenderPass{};
    VkDevice m_Divice;
};

#endif // RENDERPASS_H
