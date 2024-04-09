#include "DescriptorPool.h"

#include <stdexcept>
#include <vector>

#include "Buffer.h"

DescriptorPool::DescriptorPool(VkDevice device, int frameCount, VkDescriptorType type,
                               VkDescriptorSetLayout descriptorSetLayout,
                               const std::vector<std::unique_ptr<Buffer>>& buffers) :
    m_Device(device)
{
    CreatePool(frameCount, type);
    CreateSets(frameCount, type, descriptorSetLayout, buffers);
}

void DescriptorPool::CreatePool(int frameCount, VkDescriptorType type)
{
    const VkDescriptorPoolSize poolSize{ .type = type, .descriptorCount = static_cast<uint32_t>(frameCount) };

    const VkDescriptorPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .maxSets = static_cast<uint32_t>(frameCount),
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
    };

    if(vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor pool!");
}

void DescriptorPool::CreateSets(int frameCount, VkDescriptorType type, VkDescriptorSetLayout descriptorSetLayout,
                                const std::vector<std::unique_ptr<Buffer>>& buffers)
{
    std::vector<VkDescriptorSetLayout> layouts(frameCount, descriptorSetLayout);

    const VkDescriptorSetAllocateInfo descriptorAllocateInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                                              .descriptorPool = m_DescriptorPool,
                                                              .descriptorSetCount = static_cast<uint32_t>(frameCount),
                                                              .pSetLayouts = layouts.data() };


    m_DescriptorSets.resize(frameCount);
    if(vkAllocateDescriptorSets(m_Device, &descriptorAllocateInfo, m_DescriptorSets.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate descriptor sets!");

    for(size_t i = 0; i < frameCount; i++)
    {
        const VkDescriptorBufferInfo bufferInfo{ .buffer = *buffers[i], .offset = 0, .range = VK_WHOLE_SIZE };

        const VkWriteDescriptorSet descriptorWrite{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = m_DescriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = nullptr,  // Optional
            .pBufferInfo = &bufferInfo,
            .pTexelBufferView = nullptr  // Optional
        };

        vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
    }
}

DescriptorPool::~DescriptorPool() { vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr); }
