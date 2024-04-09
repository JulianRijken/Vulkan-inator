#pragma once

#include <memory>
#include <vector>

#include "Buffer.h"
#include "vulkan/vulkan_core.h"

class DescriptorPool
{
public:
    DescriptorPool(VkDevice device, int frameCount, VkDescriptorType type, VkDescriptorSetLayout descriptorSetLayout,
                   const std::vector<std::unique_ptr<Buffer>>& buffers);
    ~DescriptorPool();

    DescriptorPool(DescriptorPool&&) = delete;
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    VkDescriptorSet* GetDescriptorSet(int index) { return &m_DescriptorSets[index]; }

private:
    void CreatePool(int frameCount, VkDescriptorType type);
    void CreateSets(int frameCount, VkDescriptorType type, VkDescriptorSetLayout descriptorSetLayout,
                    const std::vector<std::unique_ptr<Buffer>>& buffers);

    VkDevice m_Device{};
    VkDescriptorPool m_DescriptorPool{};
    std::vector<VkDescriptorSet> m_DescriptorSets{};
};

