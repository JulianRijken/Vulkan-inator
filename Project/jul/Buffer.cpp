#include "Buffer.h"

#include <stdexcept>
#include <cstring>

#include "vulkanbase/VulkanGlobals.h"

uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    return -1;
}

Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(VulkanGlobals::GetDevice(), &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
        throw std::runtime_error("failed to create vertex buffer!");


    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(VulkanGlobals::GetDevice(), m_Buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex =
        FindMemoryType(memRequirements.memoryTypeBits, properties, VulkanGlobals::GetPhysicalDevice());

    if(vkAllocateMemory(VulkanGlobals::GetDevice(), &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate vertex buffer memory!");

    vkBindBufferMemory(VulkanGlobals::GetDevice(), m_Buffer, m_BufferMemory, 0);
}

Buffer::~Buffer()
{
    vkDestroyBuffer(VulkanGlobals::GetDevice(), m_Buffer, nullptr);
    vkFreeMemory(VulkanGlobals::GetDevice(), m_BufferMemory, nullptr);
}

void Buffer::Upload(void* uploadDataPtr, uint32_t size)
{
    if(m_BufferDataPtr == nullptr)
    {
        void* tempBufferDataPtr = nullptr;
        vkMapMemory(VulkanGlobals::GetDevice(), m_BufferMemory, 0, size, 0, &tempBufferDataPtr);
        memcpy(tempBufferDataPtr, uploadDataPtr, (size_t)size);
        vkUnmapMemory(VulkanGlobals::GetDevice(), m_BufferMemory);
    }
    else
    {
        memcpy(m_BufferDataPtr, uploadDataPtr, (size_t)size);
    }
}

void Buffer::Map(uint32_t size)
{
    vkMapMemory(VulkanGlobals::GetDevice(), m_BufferMemory, 0, size, 0, &m_BufferDataPtr);
}

void Buffer::Unmap()
{
    vkUnmapMemory(VulkanGlobals::GetDevice(), m_BufferMemory);
    m_BufferDataPtr = nullptr;
}
