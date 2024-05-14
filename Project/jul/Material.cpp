#include "Material.h"

#include <vulkanbase/VulkanGlobals.h>

#include <stdexcept>
#include <vector>

#include "Texture.h"

Material::Material(const std::vector<const Texture*>& textures) :
    m_Texture(textures)
{


    const VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = g_MaterialPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &g_MaterialSetLayout,
    };

    if(vkAllocateDescriptorSets(VulkanGlobals::GetDevice(), &allocInfo, &m_DescriptorSet) != VK_SUCCESS)
        throw std::runtime_error{ "failed to allocate descriptor sets!" };


    std::vector<VkWriteDescriptorSet> descriptorWrites;
    descriptorWrites.reserve(textures.size());

    for(const Texture* texture : m_Texture)
    {
        auto descriptorInfo = texture->GetDescriptorInfo();

        descriptorWrites.emplace_back(VkWriteDescriptorSet{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = m_DescriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,

            .pImageInfo = &descriptorInfo,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr,
        });
    }

    vkUpdateDescriptorSets(
        VulkanGlobals::GetDevice(), uint32_t(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void Material::CreateMaterialPool(int maxMaterialCount)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings{};

    // Add Albedo
    bindings.push_back({
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = nullptr,
    });

    // Add Normal
    bindings.push_back({
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = nullptr,
    });


    const VkDescriptorSetLayoutCreateInfo setInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                                                   .bindingCount = static_cast<uint32_t>(bindings.size()),
                                                   .pBindings = bindings.data() };


    if(vkCreateDescriptorSetLayout(VulkanGlobals::GetDevice(), &setInfo, nullptr, &g_MaterialSetLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create vkCreateDescriptorSetLayout!");


    const VkDescriptorPoolSize poolSize{ .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         .descriptorCount = static_cast<uint32_t>(maxMaterialCount)

    };

    const VkDescriptorPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .maxSets = static_cast<uint32_t>(maxMaterialCount),
        .poolSizeCount = static_cast<uint32_t>(1),
        .pPoolSizes = &poolSize,
    };

    if(vkCreateDescriptorPool(VulkanGlobals::GetDevice(), &poolInfo, nullptr, &g_MaterialPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create vkCreateDescriptorPool!");
}

void Material::Cleanup()
{
    vkDestroyDescriptorSetLayout(VulkanGlobals::GetDevice(), g_MaterialSetLayout, nullptr);
    vkDestroyDescriptorPool(VulkanGlobals::GetDevice(), g_MaterialPool, nullptr);
}
