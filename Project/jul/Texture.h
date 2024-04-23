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
        VkImage m_TextureImage;
        VkDeviceMemory m_TextureImageMemory;
    };
}  // namespace jul
