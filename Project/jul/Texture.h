#include <vulkan/vulkan_core.h>

#include <string>

#pragma once

namespace jul
{
    class Texture
    {
    public:
        Texture(const std::string& filePath);
        ~Texture();

        Texture(Texture&&) = delete;
        Texture(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;
        Texture& operator=(const Texture&) = delete;

    private:
        static void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
        static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);


        VkImage m_TextureImage;
        VkDeviceMemory m_TextureImageMemory;
    };
}  // namespace jul
