#include "vulkanbase/VulkanBase.h"

#include <algorithm>
#include <map>
#include <set>

#include "jul/GameTime.h"
#include "jul/GUI.h"
#include "jul/Input.h"
#include "jul/Material.h"
#include "vulkanbase/VulkanGlobals.h"
#include "vulkanbase/VulkanUtil.h"

void VulkanBase::Run()
{
    InitWindow();
    InitVulkan();
    GUI::Init(m_Window);
    MainLoop();
    Cleanup();
}

void VulkanBase::InitVulkan()
{
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();

    PickPhysicalDevice();
    CreateLogicalDevice();

    glm::ivec2 windowSize{};
    glfwGetFramebufferSize(m_Window, &windowSize.x, &windowSize.y);
    m_SwapChainUPtr = std::make_unique<SwapChain>(m_Surface, windowSize);
    VulkanGlobals::s_SwapChainPtr = m_SwapChainUPtr.get();

    m_RenderPassUPtr = std::make_unique<RenderPass>(m_Device, m_SwapChainUPtr->GetImageFormat());
    VulkanGlobals::s_RenderPassPtr = m_RenderPassUPtr.get();

    vulkanUtil::QueueFamilyIndices indices = vulkanUtil::FindQueueFamilies(m_PhysicalDevice);
    m_CommandBufferUPtr = std::make_unique<CommandBuffer>(m_Device, indices.graphicsFamily.value());

    CreateDepthResources();

    m_SwapChainUPtr->CreateFrameBuffers(m_RenderPassUPtr.get(), m_DepthImageView);
    Material::CreateMaterialPool(7, 5);
    CreateSyncObjects();
}

void VulkanBase::MainLoop()
{
    m_GameUPtr = std::make_unique<Game>();

    while(not glfwWindowShouldClose(m_Window))
    {
        jul::GameTime::Update();

        Input::Update();
        glfwPollEvents();

        m_GameUPtr->Update();

        GUI::NewFrame();
        DrawFrame();
        GUI::EndFrame();

        jul::GameTime::AddToFrameCount();
    }
    vkDeviceWaitIdle(m_Device);
}

void VulkanBase::Cleanup()
{
    GUI::Cleanup();

    vkDestroySemaphore(m_Device, m_RenderFinishedSemaphore, nullptr);
    vkDestroySemaphore(m_Device, m_ImageAvailableSemaphore, nullptr);
    vkDestroyFence(m_Device, m_InFlightFence, nullptr);

    Material::Cleanup();

    m_CommandBufferUPtr.reset();
    m_GameUPtr.reset();
    m_RenderPassUPtr.reset();
    m_SwapChainUPtr.reset();

    vkDestroyImageView(VulkanGlobals::GetDevice(), m_DepthImageView, nullptr);
    vkDestroyImage(VulkanGlobals::GetDevice(), m_DepthImage, nullptr);
    vkFreeMemory(VulkanGlobals::GetDevice(), m_DepthImageMemory, nullptr);


    vkDestroyDevice(m_Device, nullptr);

    if(enableValidationLayers)
        vulkanUtil::DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    vkDestroyInstance(m_Instance, nullptr);

    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void VulkanBase::InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    m_Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(m_Window, this);

    glfwSetKeyCallback(m_Window,
                       [](GLFWwindow*, int key, int, int action, int)
                       {
                           if(action == GLFW_PRESS)
                               Input::OnKeyDown(key);
                           else if(action == GLFW_RELEASE)
                               Input::OnKeyUp(key);
                       });
    glfwSetCursorPosCallback(m_Window,
                             [](GLFWwindow*, double xpos, double ypos) {
                                 Input::OnMouseMove({ xpos, ypos });
                             });

    glfwSetScrollCallback(m_Window,
                          [](GLFWwindow*, double xpos, double ypos) {
                              Input::OnMouseScroll({ xpos, ypos });
                          });

    glfwSetFramebufferSizeCallback(m_Window,
                                   [](GLFWwindow* window, int, int)
                                   {
                                       auto* app = reinterpret_cast<VulkanBase*>(glfwGetWindowUserPointer(window));
                                       app->m_NeedsWindowResize = true;
                                   });
}

void VulkanBase::CreateSurface()
{
    if(glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");

    VulkanGlobals::s_Surface = m_Surface;
}

void VulkanBase::PickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

    if(deviceCount == 0)
        throw std::runtime_error("failed to find GPUs with Vulkan support!");


    std::vector<VkPhysicalDevice> devices{ deviceCount };
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

    if(deviceCount == 0)
        throw std::runtime_error("failed to find GPUs with Vulkan support!");


    std::multimap<int, VkPhysicalDevice> candidates;
    for(auto&& device : devices)
        candidates.insert(std::make_pair(RateDeviceSuitability(device), device));


    auto&& max = std::ranges::max_element(candidates);

    // If there is no GPU or No GPU is found
    if(candidates.empty() or max->first == 0)
        throw std::runtime_error("failed to find a suitable GPU!");


    m_PhysicalDevice = max->second;
    VulkanGlobals::s_PhysicalDevice = m_PhysicalDevice;
}

bool VulkanBase::IsDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    vulkanUtil::QueueFamilyIndices indices = vulkanUtil::FindQueueFamilies(device);
    return indices.IsComplete() and CheckDeviceExtensionSupport(device) and supportedFeatures.samplerAnisotropy;
}

void VulkanBase::CreateLogicalDevice()
{
    vulkanUtil::QueueFamilyIndices queueFamilyIndices = vulkanUtil::FindQueueFamilies(m_PhysicalDevice);
    std::set<uint32_t> uniqueQueueFamilies = { queueFamilyIndices.graphicsFamily.value(),
                                               queueFamilyIndices.presentFamily.value() };


    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    {
        queueCreateInfos.reserve(uniqueQueueFamilies.size());


        const float queuePriority = 1.0f;
        for(uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
    }


    VkDeviceCreateInfo createInfo{};
    {
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        const VkPhysicalDeviceFeatures deviceFeatures{ .samplerAnisotropy = VK_TRUE };
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
        createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

        if(enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
            createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        // Create the actual device
        if(vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
            throw std::runtime_error("failed to create logical device!");

        VulkanGlobals::s_Device = m_Device;
    }

    vkGetDeviceQueue(m_Device, queueFamilyIndices.graphicsFamily.value(), 0, &m_GraphicsQueue);
    VulkanGlobals::s_GraphicsQueue = m_GraphicsQueue;

    vkGetDeviceQueue(m_Device, queueFamilyIndices.presentFamily.value(), 0, &m_PresentQueue);
}

void VulkanBase::CreateDepthResources()
{
    VkFormat depthFormat = vulkanUtil::FindDepthFormat();

    vulkanUtil::CreateImage(VulkanGlobals::GetSwapChain().GetExtent().width,
                            VulkanGlobals::GetSwapChain().GetExtent().height,
                            depthFormat,
                            VK_IMAGE_TILING_OPTIMAL,
                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                            m_DepthImage,
                            m_DepthImageMemory);

    m_DepthImageView = vulkanUtil::CreateImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}


uint32_t VulkanBase::RateDeviceSuitability(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties properties{};
    VkPhysicalDeviceFeatures features{};

    vkGetPhysicalDeviceProperties(device, &properties);
    vkGetPhysicalDeviceFeatures(device, &features);

    // we need these
    if(not features.geometryShader or not features.samplerAnisotropy)
        return 0;

    uint32_t score{ 0 };

    if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        // we want this one :P
        score += 100'000'000'000;
    }

    auto&& indices{ vulkanUtil::FindQueueFamilies(device) };
    bool extensionsSupported = CheckDeviceExtensionSupport(device);
    if(not indices.IsComplete() or not extensionsSupported /*or not swapChainAdequate*/)
        return 0;

    VkPhysicalDeviceMemoryProperties memoryProps{};
    vkGetPhysicalDeviceMemoryProperties(device, &memoryProps);

    auto&& heapsPointer = memoryProps.memoryHeaps;
    std::vector<VkMemoryHeap> heaps{ heapsPointer, heapsPointer + memoryProps.memoryHeapCount };

    for(const auto& heap : heaps)
        if(heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            score += heap.size;


    return score;
}

#include <jul/GUI.h>
#include <jul/Material.h>

#include <cstring>
#include <iostream>
#include <set>

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


    vkResetCommandBuffer(*m_CommandBufferUPtr, /*VkCommandBufferResetFlagBits*/ 0);

    m_CommandBufferUPtr->BeginBuffer();
    m_RenderPassUPtr->Begin(
        m_SwapChainUPtr->GetFrameBuffer(imageIndex), m_SwapChainUPtr->GetExtent(), *m_CommandBufferUPtr);
    m_GameUPtr->Draw(*m_CommandBufferUPtr, imageIndex);
    m_RenderPassUPtr->End(*m_CommandBufferUPtr);
    m_CommandBufferUPtr->EndBuffer();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    VkCommandBuffer commandBuffer = *m_CommandBufferUPtr;
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
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_Window, &width, &height);
        while(width == 0 || height == 0)
        {
            glfwGetFramebufferSize(m_Window, &width, &height);
            glfwWaitEvents();
        }


        m_NeedsWindowResize = false;
        vkDeviceWaitIdle(VulkanGlobals::GetDevice());

        m_SwapChainUPtr.reset();


        glm::ivec2 windowSize{};
        glfwGetFramebufferSize(m_Window, &windowSize.x, &windowSize.y);
        m_SwapChainUPtr = std::make_unique<SwapChain>(m_Surface, windowSize);
        VulkanGlobals::s_SwapChainPtr = m_SwapChainUPtr.get();

        vkDestroyImageView(VulkanGlobals::GetDevice(), m_DepthImageView, nullptr);
        vkDestroyImage(VulkanGlobals::GetDevice(), m_DepthImage, nullptr);
        vkFreeMemory(VulkanGlobals::GetDevice(), m_DepthImageMemory, nullptr);

        CreateDepthResources();


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

    VulkanGlobals::s_Instance = m_Instance;
}
