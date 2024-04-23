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

    [[nodiscard]] VkImageView GetImageView() { return m_ImageView; }

    [[nodiscard]] VkSampler GetSampler() { return m_Sampler; }

private:
    static void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
    static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void CreateTextureSampler(VkSamplerAddressMode addressMode);

    VkSampler m_Sampler{};
    VkImageView m_ImageView{};
    VkImage m_Image{};
    VkDeviceMemory m_ImageMemory{};
};
