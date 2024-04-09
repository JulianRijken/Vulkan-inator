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


    glm::ivec2 windowSize{};
    glfwGetFramebufferSize(m_window, &windowSize.x, &windowSize.y);

    m_SwapChain = std::make_unique<SwapChain>(m_Device, m_PhysicalDevice, m_Surface, windowSize);
    m_RenderPass = std::make_unique<RenderPass>(m_Device, m_SwapChain->GetImageFormat());


    m_Pipline2D = std::make_unique<Pipeline<Mesh::Vertex2D>>(
        m_Device,
        m_PhysicalDevice,
        *m_RenderPass,
        Shader{ "shaders/shader2D.vert.spv", "shaders/shader2D.frag.spv", m_Device },
        m_SwapChain->GetImageCount());

    m_Pipline3D = std::make_unique<Pipeline<Mesh::Vertex3D>>(
        m_Device,
        m_PhysicalDevice,
        *m_RenderPass,
        Shader{ "shaders/shader3D.vert.spv", "shaders/shader3D.frag.spv", m_Device },
        m_SwapChain->GetImageCount());


    m_SwapChain->CreateFrameBuffers(m_RenderPass.get());

    VkUtils::QueueFamilyIndices indices = VkUtils::FindQueueFamilies(m_PhysicalDevice, m_Surface);
    m_CommandBufferUPtr = std::make_unique<CommandBuffer>(m_Device, indices.graphicsFamily.value());

    CreateSyncObjects();
    {
        const std::vector<Mesh::Vertex2D> vertices = {
            {{ 0.0f, -0.5f }, { 1.0f, 1.0f, 1.0f }},
            { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{ -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }}
        };

        const std::vector<uint32_t> indeces = { 0, 1, 2 };


        // m_Pipline2D->AddMesh(Mesh{
        //     m_Device,
        //     m_PhysicalDevice,
        //     m_GraphicsQueue,
        //     indeces,
        //     Mesh::VertexData{.data = (void*)vertices.data(),
        //                      .vertexCount = static_cast<uint32_t>(vertices.size()),
        //                      .typeSize = sizeof(Mesh::Vertex2D)}
        // });
    }

    {

        const std::vector<Mesh::Vertex3D> vertices{
  // front
            { { -0.5f, -0.5f, 0.5f }, {}, { 1, 0, 1 }}, // 0
            {  { 0.5f, -0.5f, 0.5f }, {}, { 1, 0, 1 }}, // 1
            {   { 0.5f, 0.5f, 0.5f }, {}, { 1, 0, 1 }}, // 2
            {  { -0.5f, 0.5f, 0.5f }, {}, { 1, 0, 1 }}, // 3

  // back
            {{ -0.5f, -0.5f, -0.5f }, {}, { 1, 0, 1 }}, // 4
            { { 0.5f, -0.5f, -0.5f }, {}, { 1, 0, 1 }}, // 5
            {  { 0.5f, 0.5f, -0.5f }, {}, { 1, 0, 1 }}, // 6
            { { -0.5f, 0.5f, -0.5f }, {}, { 1, 0, 1 }}  // 7
        };

        const std::vector<uint32_t> indeces{
            0, 1, 2, 2, 3, 0,  // front
            1, 5, 6, 6, 2, 1,  // right
            5, 4, 7, 7, 6, 5,  // back
            4, 0, 3, 3, 7, 4,  // left
            3, 2, 6, 6, 7, 3,  // top
            4, 5, 1, 1, 0, 4   // bottom
        };

        m_Pipline3D->AddMesh(Mesh{
            m_Device,
            m_PhysicalDevice,
            m_GraphicsQueue,
            indeces,
            Mesh::VertexData{.data = (void*)vertices.data(),
                             .vertexCount = static_cast<uint32_t>(vertices.size()),
                             .typeSize = sizeof(Mesh::Vertex2D)}
        });
    }
}

void VulkanBase::MainLoop()
{
    while(not glfwWindowShouldClose(m_window))
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




