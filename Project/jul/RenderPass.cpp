#include "RenderPass.h"

#include <array>
#include <stdexcept>

#include "vulkanbase/VulkanUtil.h"

RenderPass::RenderPass(VkDevice device,VkFormat swapChainImageFormat) :
      m_Divice(device)
{
    const VkAttachmentDescription colorAttachment{
        .format = swapChainImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    const VkAttachmentDescription depthAttachment{
        .format = vulkanUtil::FindDepthFormat(),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    const VkAttachmentReference depthAttachmentRef{
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    const VkAttachmentReference colorAttachmentRef{
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    const VkSubpassDescription subpass{ .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        .colorAttachmentCount = 1,
                                        .pColorAttachments = &colorAttachmentRef,
                                        .pDepthStencilAttachment = &depthAttachmentRef };


    // TODO Look in to if this is the right place to create this
    const VkSubpassDependency dependency{
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    };

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    const VkRenderPassCreateInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
        throw std::runtime_error("failed to create render pass!");
}

RenderPass::~RenderPass() { vkDestroyRenderPass(m_Divice, m_RenderPass, nullptr); }

void RenderPass::Begin(VkFramebuffer swapChainFramebuffers, VkExtent2D swapChainExtent, VkCommandBuffer commandBuffer)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::End(VkCommandBuffer commandBuffer) { vkCmdEndRenderPass(commandBuffer); }

RenderPass::operator VkRenderPass() { return m_RenderPass; }
