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
#include "jul/SwapChain.h"

const std::array<const char*,1> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };
const std::array<const char*,1> DEVICE_EXTENSIONS = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

class VulkanBase
{

public:
    void Run();

private:

    void InitVulkan();
        void InitWindow();
        void PickPhysicalDevice();
        void CreateSurface();
        void CreateInstance();

    void MainLoop();
        void DrawFrame();

    void Cleanup();



    void CreateSyncObjects();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void SetupDebugMessenger();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> GetRequiredExtensions();


    GLFWwindow* m_window;

    std::unique_ptr<Pipeline<Mesh::Vertex2D>> m_Pipline2D{};
    std::unique_ptr<Pipeline<Mesh::Vertex3D>> m_Pipline3D{};
    std::unique_ptr<CommandBuffer> m_CommandBufferUPtr{};
    std::unique_ptr<RenderPass> m_RenderPass{};
    std::unique_ptr<SwapChain> m_SwapChain{};



    void CreateLogicalDevice();
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
