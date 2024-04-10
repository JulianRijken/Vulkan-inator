#include "vulkanbase/VulkanBase.h"

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
    m_SwapChainUPtr = std::make_unique<SwapChain>(m_Device, m_PhysicalDevice, m_Surface, windowSize);
    VulkanGlobals::s_SwapChainPtr = m_SwapChainUPtr.get();

    m_RenderPassUPtr = std::make_unique<RenderPass>(m_Device, m_SwapChainUPtr->GetImageFormat());
    VulkanGlobals::s_RenderPassPtr = m_RenderPassUPtr.get();


    VkUtils::QueueFamilyIndices indices = VkUtils::FindQueueFamilies(m_PhysicalDevice, m_Surface);
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

    // TODO try moving this in to the destructor of swap chain
    m_SwapChainUPtr->DestroyFrameBuffer();

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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
}

void VulkanBase::CreateSurface()
{
    if (glfwCreateWindowSurface(m_Instance, m_window, nullptr, &m_Surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");
}




