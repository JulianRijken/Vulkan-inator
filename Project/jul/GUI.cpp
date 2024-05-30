#include "GUI.h"

#include <vulkan/vulkan_core.h>

#include <stdexcept>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "SwapChain.h"
#include "vulkanbase/VulkanGlobals.h"
#include "vulkanbase/VulkanUtil.h"

void GUI::Init(GLFWwindow* windowPtr)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls


    ImGui_ImplGlfw_InitForVulkan(windowPtr, true);
    ImGui::StyleColorsDark();  // or ImGui::StyleColorsClassic();

    const VkDescriptorPoolSize poolSizes[] = {
        {               VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {         VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {         VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {  VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {  VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {      VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
    };
    VkDescriptorPoolCreateInfo pool_info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                                             .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                                             .maxSets = 1000 * IM_ARRAYSIZE(poolSizes),
                                             .poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes),
                                             .pPoolSizes = poolSizes };

    if(vkCreateDescriptorPool(VulkanGlobals::GetDevice(), &pool_info, nullptr, &g_GUIPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor pool");


    const auto imageFormat = VulkanGlobals::GetSwapChain().GetImageFormat();

    // 5. Initialize ImGui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {
        .Instance = VulkanGlobals::GetInstance(),
        .PhysicalDevice = VulkanGlobals::GetPhysicalDevice(),
        .Device = VulkanGlobals::GetDevice(),
        .QueueFamily = vulkanUtil::FindQueueFamilies(VulkanGlobals::GetPhysicalDevice()).graphicsFamily.value(),
        .Queue = VulkanGlobals::GetGraphicsQueue(),
        .DescriptorPool = g_GUIPool,
        .RenderPass = VulkanGlobals::GetRederPass(),
        .MinImageCount = 2,
        .ImageCount = static_cast<uint32_t>(VulkanGlobals::GetSwapChain().GetImageCount()),
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        .UseDynamicRendering = true,
        .PipelineRenderingCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
                                        .colorAttachmentCount = 1,
                                        .pColorAttachmentFormats = &imageFormat},
    };

    ImGui_ImplVulkan_Init(&init_info);
}

void GUI::Cleanup()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    vkDestroyDescriptorPool(VulkanGlobals::GetDevice(), g_GUIPool, nullptr);
}

void GUI::NewFrame()
{
    // IMGUI SETUP
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    // RENDER
    ImGui::ShowDemoWindow();
    ImGui::Render();
}

void GUI::EndFrame() { ImGui::EndFrame(); }
