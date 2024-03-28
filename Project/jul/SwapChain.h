#include "vulkan/vulkan_core.h"
#include <vector>


struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


class SwapChain
{
public:
    SwapChain();
    ~SwapChain();

    VkFormat GetImageFormat() {return m_SwapChainImageFormat; }

    void DestroyFrameBuffer();

private:

    void CreateSwapChain();
    void CreateImageViews();
    void CreateFrameBuffers();


    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);


    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;
    VkSwapchainKHR m_SwapChain;
    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    std::vector<VkFramebuffer> m_SwapChainFramebuffers;


};
