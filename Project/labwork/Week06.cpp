
#include <iostream>
#include <set>
#include <cstring>

#include "vulkanbase/VulkanBase.h"
#include "vulkanbase/VulkanGlobals.h"
#include "vulkanbase/VulkanUtil.h"


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData)
{
    std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

void VulkanBase::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void VulkanBase::SetupDebugMessenger()
{
    if(not enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    if(vulkanUtil::CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
        throw std::runtime_error("failed to set up debug messenger!");
}

void VulkanBase::CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if(vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore) != VK_SUCCESS ||
       vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) != VK_SUCCESS ||
       vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

void VulkanBase::DrawFrame()
{
    vkWaitForFences(m_Device, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex = 0;
    vkAcquireNextImageKHR(
        m_Device, *m_SwapChainUPtr, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);


    vkResetFences(m_Device, 1, &m_InFlightFence);


    vkResetCommandBuffer(m_CommandBufferUPtr->Get(), /*VkCommandBufferResetFlagBits*/ 0);

    m_CommandBufferUPtr->BeginCommandBuffer();
    m_RenderPassUPtr->Begin(
        m_SwapChainUPtr->GetFrameBuffer(imageIndex), m_SwapChainUPtr->GetExtent(), m_CommandBufferUPtr->Get());
    m_GameUPtr->Draw(m_CommandBufferUPtr->Get(), imageIndex);
    m_RenderPassUPtr->End(m_CommandBufferUPtr->Get());
    m_CommandBufferUPtr->EndCommandBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    auto commandBuffer = m_CommandBufferUPtr->Get();
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFence) != VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { *m_SwapChainUPtr };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    const VkResult presentResult = vkQueuePresentKHR(m_PresentQueue, &presentInfo);
    if(m_NeedsWindowResize or presentResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_window, &width, &height);
        while(width == 0 || height == 0)
        {
            glfwGetFramebufferSize(m_window, &width, &height);
            glfwWaitEvents();
        }


        m_NeedsWindowResize = false;
        vkDeviceWaitIdle(VulkanGlobals::GetDevice());

        m_SwapChainUPtr.reset();


        glm::ivec2 windowSize{};
        glfwGetFramebufferSize(m_window, &windowSize.x, &windowSize.y);
        m_SwapChainUPtr = std::make_unique<SwapChain>(m_Surface, windowSize);
        VulkanGlobals::s_SwapChainPtr = m_SwapChainUPtr.get();

        m_GameUPtr->OnResize();

        m_SwapChainUPtr->CreateFrameBuffers(m_RenderPassUPtr.get(), m_DepthImageView);
        return;
    }
}

bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(const char* layerName : VALIDATION_LAYERS)
    {
        bool layerFound = false;

        for(const auto& layerProperties : availableLayers)
        {
            if(strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
            return false;
    }

    return true;
}

std::vector<const char*> VulkanBase::GetRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if(enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

bool VulkanBase::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

    for(const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);


    return requiredExtensions.empty();
}

void VulkanBase::CreateInstance()
{
    if(enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("validation layers requested, but not available!");


    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if(vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
        throw std::runtime_error("failed to create instance!");
}
