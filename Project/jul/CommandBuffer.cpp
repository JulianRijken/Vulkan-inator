#include "CommandBuffer.h"

#include <stdexcept>
#include <vulkan/vulkan_core.h>


CommandBuffer::CommandBuffer(VkDevice device, uint32_t familyIndex) :
	m_Device(device)
{
    s_AliveBuffers++;

    // Init command pull if static is not initialized
    if(s_CommandPool == nullptr)
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = familyIndex;

        if (vkCreateCommandPool(m_Device, &poolInfo, nullptr, &s_CommandPool) != VK_SUCCESS)
            throw std::runtime_error("failed to create command pool!");
    }

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = s_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(m_Device, &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffers!");
}

CommandBuffer::~CommandBuffer()
{
    vkFreeCommandBuffers(m_Device , s_CommandPool , 1 , & m_CommandBuffer);

    s_AliveBuffers--;

    if(s_AliveBuffers <= 0)
    {
        vkDestroyCommandPool(m_Device, s_CommandPool, nullptr);
        s_CommandPool = nullptr;
    }
}

void CommandBuffer::BeginCommandBuffer() const
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin recording command buffer!");
}

void CommandBuffer::EndCommandBuffer() const
{
	if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to record command buffer!");
}

