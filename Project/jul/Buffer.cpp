#include "Buffer.h"

#include <stdexcept>

uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    return -1;
}

Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDevice device,
               VkPhysicalDevice physicalDevice) :
    m_Device(device)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
        throw std::runtime_error("failed to create vertex buffer!");


    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, m_Buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);

    if(vkAllocateMemory(device, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate vertex buffer memory!");

    vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);
}

Buffer::~Buffer()
{
    vkDestroyBuffer(m_Device, m_Buffer, nullptr);
    vkFreeMemory(m_Device, m_BufferMemory, nullptr);
}

void Buffer::Upload(void* uploadDataPtr, uint32_t size)
{
    if(m_BufferDataPtr == nullptr)
    {
        void* tempBufferDataPtr = nullptr;
        vkMapMemory(m_Device, m_BufferMemory, 0, size, 0, &tempBufferDataPtr);
        memcpy(tempBufferDataPtr, uploadDataPtr, (size_t)size);
        vkUnmapMemory(m_Device, m_BufferMemory);
    }
    else
    {
        memcpy(m_BufferDataPtr, uploadDataPtr, (size_t)size);
    }
}

void Buffer::Map(uint32_t size) { vkMapMemory(m_Device, m_BufferMemory, 0, size, 0, &m_BufferDataPtr); }

void Buffer::Unmap()
{
    vkUnmapMemory(m_Device, m_BufferMemory);
    m_BufferDataPtr = nullptr;
}
