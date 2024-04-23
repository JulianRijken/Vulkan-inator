#include "Texture.h"

#include <external/stb/stb_image.h>

#include <glm/vec2.hpp>
#include <stdexcept>

#include "Buffer.h"
#include "vulkan/vulkan_core.h"
#include "vulkanbase/VulkanUtil.h"

jul::Texture::Texture(const std::string& filePath)
{
    using namespace std::string_literals;

    glm::ivec2 imageSize{};
    int channelCount{};  // We force STBI_rgb_alpha so always 4

    stbi_uc* pixelsPtr = stbi_load(filePath.c_str(), &imageSize.x, &imageSize.y, &channelCount, STBI_rgb_alpha);
    const VkDeviceSize imageBufferSize = imageSize.x * imageSize.y * 4;

    if(pixelsPtr == nullptr)
        throw std::runtime_error("Failed to load texture image!"s + stbi_failure_reason());

    Buffer imageStagingBuffer = { imageBufferSize,
                                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

    imageStagingBuffer.Upload(pixelsPtr, imageBufferSize);
    stbi_image_free(pixelsPtr);

    vulkanUtil::CreateImage(imageSize.x,
                            imageSize.y,
                            VK_FORMAT_R8G8B8A8_SRGB,
                            VK_IMAGE_TILING_OPTIMAL,
                            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                            m_TextureImage,
                            m_TextureImageMemory);
}

jul::Texture::~Texture() {}
