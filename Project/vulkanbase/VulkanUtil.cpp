#include "vulkanbase/VulkanUtil.h"

#include <fstream>
#include <memory>
#include <string>

#include "jul/CommandBuffer.h"
#include "vulkanbase/VulkanGlobals.h"

VkResult vulkanUtil::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                  const VkAllocationCallbacks* pAllocator,
                                                  VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void vulkanUtil::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                               const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr)
        func(instance, debugMessenger, pAllocator);
}

std::vector<char> vulkanUtil::ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open())
        throw std::runtime_error("failed to open file!");

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

vulkanUtil::QueueFamilyIndices vulkanUtil::FindQueueFamilies(VkPhysicalDevice device)
{
    vulkanUtil::QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for(const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, VulkanGlobals::GetSurface(), &presentSupport);

        if(presentSupport)
            indices.presentFamily = i;

        if(indices.IsComplete())
            break;

        i++;
    }

    return indices;
}

void vulkanUtil::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    CommandBuffer commandBuffer{ VulkanGlobals::GetDevice() };

    const VkBufferCopy copyRegion{ .size = size };

    commandBuffer.BeginBuffer();
    {
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    }
    commandBuffer.EndBuffer();
}

VkFormat vulkanUtil::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                         VkFormatFeatureFlags features)
{
    for(const VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(VulkanGlobals::GetPhysicalDevice(), format, &props);

        if(tiling == VK_IMAGE_TILING_LINEAR and (props.linearTilingFeatures & features) == features)
            return format;
        else if(tiling == VK_IMAGE_TILING_OPTIMAL and (props.optimalTilingFeatures & features) == features)
            return format;

        throw std::runtime_error("failed to find supported format!");
    }

    return {};
}

VkFormat vulkanUtil::FindDepthFormat()
{
    return FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool vulkanUtil::HasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

uint32_t vulkanUtil::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(VulkanGlobals::GetPhysicalDevice(), &memProperties);

    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    return -1;
}

VkFormat vulkanUtil::PickBestFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                    VkFormatFeatureFlags features)
{
    for(auto&& format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(VulkanGlobals::GetPhysicalDevice(), format, &props);

        if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;
        if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }

    throw std::runtime_error{ "failed to find a supported format!" };
}

bool vulkanUtil::FasDepthComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void vulkanUtil::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                             VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image,
                             VkDeviceMemory& imageMemory)
{
    const VkImageCreateInfo imageInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = { .width = width, .height = height, .depth = 1, },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

    if(vkCreateImage(VulkanGlobals::GetDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS)
        throw std::runtime_error("failed to create image!");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(VulkanGlobals::GetDevice(), image, &memRequirements);

    const VkMemoryAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties),
    };

    if(vkAllocateMemory(VulkanGlobals::GetDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate image memory!");

    vkBindImageMemory(VulkanGlobals::GetDevice(), image, imageMemory, 0);
}

VkImageView vulkanUtil::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    const  VkImageViewCreateInfo  viewInfo{ .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange = {
                             .aspectMask = aspectFlags,
 .baseMipLevel = 0,
 .levelCount = 1,
 .baseArrayLayer = 0,
 .layerCount = 1,
        } };

    VkImageView imageView{};
    if(vkCreateImageView(VulkanGlobals::GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        throw std::runtime_error("failed to create texture image view!");

    return imageView;
}
