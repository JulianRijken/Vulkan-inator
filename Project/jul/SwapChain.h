#include <vector>

#include "glm/vec2.hpp"
#include "RenderPass.h"
#include "vulkan/vulkan_core.h"


struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


class SwapChain
{
public:
    SwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const glm::ivec2& extents);
    ~SwapChain();

    VkFormat GetImageFormat() {return m_SwapChainImageFormat; }

    void DestroyFrameBuffer();
    void CreateFrameBuffers(RenderPass* renderPass);

    operator VkSwapchainKHR();

    VkExtent2D GetExtent() { return m_SwapChainExtent; }

    VkFramebuffer GetFrameBuffer(int index) { return m_SwapChainFramebuffers[index]; }

private:
    void CreateSwapChain(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const glm::ivec2& extents);
    void CreateImageViews();


    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const glm::ivec2& extents);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);


    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;
    VkSwapchainKHR m_SwapChain;
    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;

    std::vector<VkFramebuffer> m_SwapChainFramebuffers;
    VkDevice m_Device;
};