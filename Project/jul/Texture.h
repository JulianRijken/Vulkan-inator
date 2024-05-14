#pragma once

#include <vulkan/vulkan_core.h>

#include <string>


class Texture
{
public:
    Texture(const std::string& filePath);
    ~Texture();

    Texture(Texture&&) = delete;
    Texture(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;
    Texture& operator=(const Texture&) = delete;

    [[nodiscard]] const VkDescriptorImageInfo& GetDescriptorInfo() const { return descriptorImageInfo; }


private:
    static void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
    static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void CreateTextureSampler(VkSamplerAddressMode addressMode);

    VkDescriptorImageInfo descriptorImageInfo{};
    VkImage m_Image{};
    VkDeviceMemory m_ImageMemory{};
};
