#include "DescriptorPool.h"

#include <stdexcept>
#include <vector>

#include "Buffer.h"

DescriptorPool::DescriptorPool(VkDevice device, int frameCount, const std::vector<VkDescriptorType>& types,
                               VkDescriptorSetLayout descriptorSetLayout,
                               const std::vector<std::unique_ptr<Buffer>>& buffers) :
    m_Device(device)
{
    CreatePool(frameCount, types);
    CreateSets(frameCount, types, descriptorSetLayout, buffers);
}

void DescriptorPool::CreatePool(int frameCount, const std::vector<VkDescriptorType>& types)
{
    std::vector<VkDescriptorPoolSize> poolSizes{};
    poolSizes.reserve(types.size());

    for(size_t typeIndex{}; typeIndex < types.size(); ++typeIndex)
        poolSizes.emplace_back(types[typeIndex], static_cast<uint32_t>(frameCount));


    const VkDescriptorPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .maxSets = static_cast<uint32_t>(frameCount),
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };

    if(vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor pool!");
}

void DescriptorPool::CreateSets(int frameCount, const std::vector<VkDescriptorType>& types,
                                VkDescriptorSetLayout descriptorSetLayout,
                                const std::vector<std::unique_ptr<Buffer>>& buffers)
{
    std::vector<VkDescriptorSetLayout> layouts(frameCount, descriptorSetLayout);

    const VkDescriptorSetAllocateInfo descriptorAllocateInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                                              .descriptorPool = m_DescriptorPool,
                                                              .descriptorSetCount = static_cast<uint32_t>(frameCount),
                                                              .pSetLayouts = layouts.data() };


    m_DescriptorSets.resize(frameCount);
    if(vkAllocateDescriptorSets(m_Device, &descriptorAllocateInfo, m_DescriptorSets.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate descriptor sets! For General Descriptor Pool");

    for(size_t i = 0; i < frameCount; i++)
    {
        const VkDescriptorBufferInfo bufferInfo{ .buffer = *buffers[i], .offset = 0, .range = VK_WHOLE_SIZE };


        const VkDescriptorImageInfo imageInfo{
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };


        std::vector<VkWriteDescriptorSet> descriptorWrites{};
        descriptorWrites.reserve(types.size());

        for(size_t typeIndex{}; typeIndex < types.size(); ++typeIndex)
        {
            VkWriteDescriptorSet descriptorSet{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_DescriptorSets[i],
                .dstBinding = static_cast<uint32_t>(typeIndex),
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = types[typeIndex],
                .pImageInfo = nullptr,
                .pBufferInfo = nullptr,
                .pTexelBufferView = nullptr  // Optional
            };

            if(types[typeIndex] == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                descriptorSet.pImageInfo = &imageInfo;
            else if(types[typeIndex] == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                descriptorSet.pBufferInfo = &bufferInfo;

            descriptorWrites.emplace_back(descriptorSet);
        }

        vkUpdateDescriptorSets(
            m_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

DescriptorPool::~DescriptorPool() { vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr); }
