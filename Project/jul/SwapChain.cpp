#include "SwapChain.h"

#include <algorithm>
#include <stdexcept>

#include "jul/RenderPass.h"
#include "vulkanbase/VulkanGlobals.h"
#include "vulkanbase/VulkanUtil.h"

SwapChain::SwapChain(VkSurfaceKHR surface, const glm::ivec2& extents)
{
    CreateSwapChain(VulkanGlobals::GetPhysicalDevice(), surface, extents);
    CreateImageViews();
}

SwapChain::~SwapChain()
{
    for(auto&& framebuffer : m_SwapChainFramebuffers)
        vkDestroyFramebuffer(VulkanGlobals::GetDevice(), framebuffer, nullptr);

    for (auto&& imageView : m_SwapChainImageViews)
        vkDestroyImageView(VulkanGlobals::GetDevice(), imageView, nullptr);

    vkDestroySwapchainKHR(VulkanGlobals::GetDevice(), m_SwapChain, nullptr);
}


void SwapChain::CreateFrameBuffers(RenderPass* renderPass)
{
    m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
    {
        const VkImageView attachments[] =
            {
                m_SwapChainImageViews[i]
            };

        VkFramebufferCreateInfo frameBufferInfo{};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = *renderPass;
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = m_SwapChainExtent.width;
        frameBufferInfo.height = m_SwapChainExtent.height;
        frameBufferInfo.layers = 1;

        if(vkCreateFramebuffer(VulkanGlobals::GetDevice(), &frameBufferInfo, nullptr, &m_SwapChainFramebuffers[i]) !=
           VK_SUCCESS)
            throw std::runtime_error("failed to create framebuffer!");
    }
}

SwapChain::operator VkSwapchainKHR() { return m_SwapChain; }

int SwapChain::GetImageCount() { return static_cast<int>(m_SwapChainImages.size()); }

void SwapChain::CreateSwapChain(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const glm::ivec2& extents)
{
    SwapChainSupportDetails const swapChainSupport = QuerySwapChainSupport(physicalDevice, surface);
    VkSurfaceFormatKHR const surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR const presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D const extend = ChooseSwapExtent(swapChainSupport.capabilities, extents);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;


    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extend;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkUtils::QueueFamilyIndices indices = VkUtils::FindQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(VulkanGlobals::GetDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
        throw std::runtime_error("failed to create swap chain!");


    vkGetSwapchainImagesKHR(VulkanGlobals::GetDevice(), m_SwapChain, &imageCount, nullptr);
    m_SwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(VulkanGlobals::GetDevice(), m_SwapChain, &imageCount, m_SwapChainImages.data());

    m_SwapChainImageFormat = surfaceFormat.format;
    m_SwapChainExtent = extend;
}




void SwapChain::CreateImageViews()
{
    m_SwapChainImageViews.resize(m_SwapChainImages.size());

    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_SwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_SwapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if(vkCreateImageView(VulkanGlobals::GetDevice(), &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create image views!");
    }
}

SwapChainSupportDetails SwapChain::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if(formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

// Chose V-Sync option
VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    // Try to enable mailbox if possible
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const glm::ivec2& extents)
{
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
        return capabilities.currentExtent;


    VkExtent2D actualExtent = { static_cast<uint32_t>(extents.x), static_cast<uint32_t>(extents.y) };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}

