#pragma once

#include <memory>
#include <vector>

#include "Buffer.h"
#include "vulkan/vulkan_core.h"


class Texture;

class DescriptorPool
{
public:
    DescriptorPool(VkDevice device, int frameCount, const std::vector<VkDescriptorType>& types,
                   VkDescriptorSetLayout descriptorSetLayout, const std::vector<std::unique_ptr<Buffer>>& buffers);
    ~DescriptorPool();

    DescriptorPool(DescriptorPool&&) = delete;
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    VkDescriptorSet* GetDescriptorSet(int index) { return &m_DescriptorSets[index]; }

    operator VkDescriptorPool() const { return m_DescriptorPool; }


private:
    void CreatePool(int frameCount, const std::vector<VkDescriptorType>& types);
    void CreateSets(int frameCount, const std::vector<VkDescriptorType>& types,
                    VkDescriptorSetLayout descriptorSetLayout, const std::vector<std::unique_ptr<Buffer>>& buffers);

    VkDevice m_Device{};
    VkDescriptorPool m_DescriptorPool{};
    std::vector<VkDescriptorSet> m_DescriptorSets{};
};
