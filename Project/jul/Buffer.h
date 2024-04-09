#pragma once

#include "vulkan/vulkan_core.h"

class Buffer
{
public:
    Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDevice device,
           VkPhysicalDevice physicalDevice);

    ~Buffer();

    Buffer(Buffer&&) = delete;
    Buffer(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    void Upload(void* uploadDataPtr, uint32_t size);
    void Map(uint32_t size);
    void Unmap();

    operator VkBuffer() { return m_Buffer; }
    operator VkDeviceMemory() { return m_BufferMemory; }

private:
    VkBuffer m_Buffer{};
    VkDeviceMemory m_BufferMemory{};
    VkDevice m_Device;

    void* m_BufferDataPtr = nullptr;
};

