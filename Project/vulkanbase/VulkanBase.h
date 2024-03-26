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
#include "jul/RenderPass.h"

const std::array<const char*,1> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };
const std::array<const char*,1> DEVICE_EXTENSIONS = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };


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
    void CreateFrameBuffers();
    void CreateSwapChain();
    void CreateImageViews();
    void CreateInstance();
    void CreateSyncObjects();
    void CreateLogicalDevice();

    void PickPhysicalDevice();

    void DrawFrame();

    bool IsDeviceSuitable(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void SetupDebugMessenger();
    std::vector<const char*> GetRequiredExtensions();

    GLFWwindow* m_window;

    std::unique_ptr<Pipeline<Mesh::Vertex2D>> m_Pipline2D{};
    std::unique_ptr<Pipeline<Mesh::Vertex3D>> m_Pipline3D{};

    std::unique_ptr<CommandBuffer> m_CommandBufferUPtr{};
    std::unique_ptr<RenderPass> m_RenderPass{};

    // TODO: Abstract swap chain
    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;
    VkSwapchainKHR m_SwapChain;
    std::vector<VkFramebuffer> m_SwapChainFramebuffers;
    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;


    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    VkDevice m_Device = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface;

    VkFence m_InFlightFence;
    VkSemaphore m_ImageAvailableSemaphore;
    VkSemaphore m_RenderFinishedSemaphore;
};
