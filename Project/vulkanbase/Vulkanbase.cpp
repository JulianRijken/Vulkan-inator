#include "vulkanbase/VulkanBase.h"

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

    m_SwapChain  = std::make_unique<SwapChain>();
    m_RenderPass = std::make_unique<RenderPass>(m_Device, m_SwapChain->GetImageFormat());
    m_Pipline2D  = std::make_unique<Pipeline<Mesh::Vertex2D>>(m_Device,*m_RenderPass);
    m_Pipline3D  = std::make_unique<Pipeline<Mesh::Vertex3D>>(m_Device,*m_RenderPass);

    CreateFrameBuffers();

    VkUtils::QueueFamilyIndices indices = VkUtils::FindQueueFamilies(m_PhysicalDevice, m_Surface);
    m_CommandBufferUPtr = std::make_unique<CommandBuffer>(m_Device, indices.graphicsFamily.value());

    CreateSyncObjects();



    const std::vector<Mesh::Vertex2D> VERTICES = {{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
                                                  {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                                  {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};



    m_Pipline2D->AddMesh(Mesh{m_Device,m_PhysicalDevice, m_GraphicsQueue,
        Mesh::VertexData
        {
            .data = (void*) VERTICES.data(),
            .vertexCount = static_cast<uint32_t>(VERTICES.size()),
            .typeSize = sizeof(Mesh::Vertex2D)},
        });


    // m_Pipline2D->InitScene(physicalDevice,graphicsQueue);
    // m_Pipline3D->InitScene(physicalDevice,graphicsQueue);
}

void VulkanBase::MainLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        DrawFrame();
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
    m_SwapChain->DestroyFrameBuffer();

    m_Pipline2D.reset();
    m_Pipline3D.reset();
    m_RenderPass.reset();
    m_SwapChain.reset();


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
}

void VulkanBase::CreateSurface()
{
    if (glfwCreateWindowSurface(m_Instance, m_window, nullptr, &m_Surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");
}




