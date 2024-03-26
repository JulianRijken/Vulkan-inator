#include "vulkanbase/VulkanBase.h"

void VulkanBase::CreateFrameBuffers()
{
    m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
{
		const VkImageView attachments[] = 
		{
            m_SwapChainImageViews[i]
		};

		VkFramebufferCreateInfo frameBufferInfo{};
		frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = *m_RenderPass;
		frameBufferInfo.attachmentCount = 1;
		frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = m_SwapChainExtent.width;
        frameBufferInfo.height = m_SwapChainExtent.height;
		frameBufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_Device, &frameBufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to create framebuffer!");
	}
}
