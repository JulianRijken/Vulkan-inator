#include "vulkanbase/VulkanBase.h"

#include <map>
#include <set>

#include "jul/GameTime.h"
#include "jul/Input.h"
#include "vulkanbase/VulkanGlobals.h"
#include "vulkanbase/VulkanUtil.h"


void VulkanBase::Run()
{
    InitWindow();
    InitVulkan();
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
    glfwGetFramebufferSize(m_window, &windowSize.x, &windowSize.y);
    m_SwapChainUPtr = std::make_unique<SwapChain>(m_Surface, windowSize);
    VulkanGlobals::s_SwapChainPtr = m_SwapChainUPtr.get();

    m_RenderPassUPtr = std::make_unique<RenderPass>(m_Device, m_SwapChainUPtr->GetImageFormat());
    VulkanGlobals::s_RenderPassPtr = m_RenderPassUPtr.get();


    VkUtils::QueueFamilyIndices indices = VkUtils::FindQueueFamilies(m_PhysicalDevice);
    m_CommandBufferUPtr = std::make_unique<CommandBuffer>(m_Device, indices.graphicsFamily.value());

    m_SwapChainUPtr->CreateFrameBuffers(m_RenderPassUPtr.get());

    CreateSyncObjects();
}

void VulkanBase::MainLoop()
{
    m_GameUPtr = std::make_unique<Game>();

    while(not glfwWindowShouldClose(m_window))
    {
        jul::GameTime::Update();

        Input::Update();
        glfwPollEvents();

        m_GameUPtr->Update();
        DrawFrame();

        jul::GameTime::AddToFrameCount();
    }
    vkDeviceWaitIdle(m_Device);
}

void VulkanBase::Cleanup()
{
    vkDestroySemaphore(m_Device, m_RenderFinishedSemaphore, nullptr);
    vkDestroySemaphore(m_Device, m_ImageAvailableSemaphore, nullptr);
    vkDestroyFence(m_Device, m_InFlightFence, nullptr);

    m_CommandBufferUPtr.reset();
    m_GameUPtr.reset();
    m_RenderPassUPtr.reset();
    m_SwapChainUPtr.reset();

    vkDestroyDevice(m_Device, nullptr);

    if (enableValidationLayers)
        VkUtils::DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    vkDestroyInstance(m_Instance, nullptr);

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void VulkanBase::InitWindow()
{
    glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(m_window, this);

    glfwSetKeyCallback(m_window,
                       [](GLFWwindow*, int key, int, int action, int)
                       {
                           if(action == GLFW_PRESS)
                               Input::OnKeyDown(key);
                           else if(action == GLFW_RELEASE)
                               Input::OnKeyUp(key);
                       });
    glfwSetCursorPosCallback(m_window,
                             [](GLFWwindow*, double xpos, double ypos) {
                                 Input::OnMouseMove({ xpos, ypos });
                             });
    
    glfwSetFramebufferSizeCallback(m_window,
                             [](GLFWwindow* window, int, int) {
                                 auto* app = reinterpret_cast<VulkanBase*>(glfwGetWindowUserPointer(window));
                                 app->m_NeedsWindowResize = true;
                             });
}

void VulkanBase::CreateSurface()
{
    if (glfwCreateWindowSurface(m_Instance, m_window, nullptr, &m_Surface) != VK_SUCCESS)
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
    VkUtils::QueueFamilyIndices indices = VkUtils::FindQueueFamilies(device);
    return indices.IsComplete() && CheckDeviceExtensionSupport(device);
}

void VulkanBase::CreateLogicalDevice()
{
    VkUtils::QueueFamilyIndices queueFamilyIndices = VkUtils::FindQueueFamilies(m_PhysicalDevice);
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

        const VkPhysicalDeviceFeatures deviceFeatures{};
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

    auto&& indices{ VkUtils::FindQueueFamilies(device) };
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
