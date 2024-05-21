#include "Material.h"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkanbase/VulkanGlobals.h>

#include <stdexcept>
#include <vector>

#include "Texture.h"

using namespace std::string_literals;

Material::Material(const std::vector<const Texture*>& textures) :
    m_Texture(textures)
{
    const VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = g_MaterialPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &g_MaterialSetLayout,
    };

    auto result = vkAllocateDescriptorSets(VulkanGlobals::GetDevice(), &allocInfo, &m_DescriptorSet);
    if(result != VK_SUCCESS)
        throw std::runtime_error{ "failed to allocate descriptor sets! For Material, error: "s +
                                  string_VkResult(result) };


    std::vector<VkWriteDescriptorSet> descriptorWrites;
    descriptorWrites.reserve(m_Texture.size());

    for(size_t textureIndex{}; textureIndex < m_Texture.size(); ++textureIndex)
    {
        const VkDescriptorImageInfo& descriptorInfo = m_Texture[textureIndex]->GetDescriptorInfo();

        descriptorWrites.emplace_back(VkWriteDescriptorSet{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = m_DescriptorSet,
            .dstBinding = static_cast<uint32_t>(textureIndex),
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

void Material::CreateMaterialPool(int maxMaterialCount, int maxTexturesPerMaterial)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings{};

    for(size_t textureIndex{}; textureIndex < maxTexturesPerMaterial; ++textureIndex)
    {
        bindings.push_back({
            .binding = static_cast<uint32_t>(textureIndex),
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        });
    }

    const VkDescriptorSetLayoutCreateInfo setInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                                                   .bindingCount = static_cast<uint32_t>(bindings.size()),
                                                   .pBindings = bindings.data() };


    if(vkCreateDescriptorSetLayout(VulkanGlobals::GetDevice(), &setInfo, nullptr, &g_MaterialSetLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create vkCreateDescriptorSetLayout!");


    const VkDescriptorPoolSize poolSize{ .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         .descriptorCount = static_cast<uint32_t>(bindings.size())

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
