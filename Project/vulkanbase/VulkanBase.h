#pragma once

#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_WAYLAND_KHR
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// #define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


#include <vector>

#include "VulkanUtil.h"

#include "jul/Pipeline.h"
#include "jul/CommandBuffer.h"

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };


struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanBase
{

public:
    void Run();

private:

    void InitVulkan();
    void MainLoop();
    void Cleanup();
    void InitWindow();

    void CreateSurface();
    void createFrameBuffers();
    void createSwapChain();
    void createImageViews();
    void createInstance();
    void createSyncObjects();
    void createLogicalDevice();

    void pickPhysicalDevice();

    void DrawFrame();

    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();
    std::vector<const char*> getRequiredExtensions();

    GLFWwindow* window;

    std::unique_ptr<Pipeline> m_TestPipline{};
    std::unique_ptr<CommandBuffer> m_CommandBufferUPtr{};

    // TODO: Should the pipline own the render pass?
    void createRenderPass(VkFormat swapChainImageFormat);
    VkRenderPass m_RenderPass;


    // TODO: Abstract swap chain
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkSwapchainKHR swapChain;
    std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;


	VkQueue graphicsQueue;
	VkQueue presentQueue;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface;

    VkFence inFlightFence;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
};
