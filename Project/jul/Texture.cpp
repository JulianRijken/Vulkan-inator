#include "Texture.h"

#include <external/stb/stb_image.h>

#include <glm/vec2.hpp>
#include <stdexcept>

#include "Buffer.h"
#include "jul/CommandBuffer.h"
#include "vulkan/vulkan_core.h"
#include "vulkanbase/VulkanGlobals.h"
#include "vulkanbase/VulkanUtil.h"

Texture::Texture(const std::string& filePath)
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
                            m_Image,
                            m_ImageMemory);

    TransitionImageLayout(m_Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(imageStagingBuffer, m_Image, imageSize.x, imageSize.y);

    TransitionImageLayout(m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    m_ImageView = vulkanUtil::CreateImageView(m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

    CreateTextureSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT);
}

void Texture::TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = 0, // Set later
            .dstAccessMask = 0, // Set later
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange =
            {            
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            }
    };

    VkPipelineStageFlags sourceStage{};
    VkPipelineStageFlags destinationStage{};

    if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("Unsupported layout transition!");
    }


    CommandBuffer transitionBuffer{ VulkanGlobals::GetDevice() };
    transitionBuffer.BeginBuffer();
    {
        vkCmdPipelineBarrier(transitionBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }
    transitionBuffer.EndBuffer();
}

void Texture::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    const VkBufferImageCopy region{
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = 
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .imageOffset = {    0,      0, 0},
        .imageExtent = {width, height, 1},
    };

    CommandBuffer imageCopyBuffer{ VulkanGlobals::GetDevice() };
    imageCopyBuffer.BeginBuffer();
    {
        vkCmdCopyBufferToImage(imageCopyBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }
    imageCopyBuffer.EndBuffer();
}

void Texture::CreateTextureSampler(VkSamplerAddressMode addressMode)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(VulkanGlobals::GetPhysicalDevice(), &properties);

    VkSamplerCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = addressMode,
        .addressModeV = addressMode,
        .addressModeW = addressMode,
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .minLod = 0.0f,
        .maxLod = 0.0f,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };

    if(vkCreateSampler(VulkanGlobals::GetDevice(), &info, nullptr, &m_Sampler) != VK_SUCCESS)
        throw std::runtime_error("failed to create texture sampler!");
}

Texture::~Texture()
{
    vkDestroySampler(VulkanGlobals::GetDevice(), m_Sampler, nullptr);
    vkDestroyImageView(VulkanGlobals::GetDevice(), m_ImageView, nullptr);
    vkDestroyImage(VulkanGlobals::GetDevice(), m_Image, nullptr);
    vkFreeMemory(VulkanGlobals::GetDevice(), m_ImageMemory, nullptr);
}
