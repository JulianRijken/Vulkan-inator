#pragma once


#include <vector>

#include "vulkan/vulkan_core.h"

class Texture;

class Material final
{
public:
    Material(const std::vector<const Texture*>& textures);

    static void CreateMaterialPool(int maxMaterialCount, int maxTexturesPerMaterial);
    static void Cleanup();

    [[nodiscard]] VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

    [[nodiscard]] static VkDescriptorSetLayout GetMaterialSetLayout() { return g_MaterialSetLayout; }

private:
    std::vector<const Texture*> m_Texture;

    VkDescriptorSet m_DescriptorSet{};

    inline static VkDescriptorSetLayout g_MaterialSetLayout{};
    inline static VkDescriptorPool g_MaterialPool{};
};

