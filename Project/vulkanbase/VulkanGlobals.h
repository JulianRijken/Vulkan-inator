#pragma once

#include "vulkan/vulkan_core.h"

class SwapChain;
class RenderPass;

class VulkanGlobals
{
    friend class VulkanBase;


public:
    [[nodiscard]] static inline VkDevice GetDevice() { return s_Device; }

    [[nodiscard]] static inline VkPhysicalDevice GetPhysicalDevice() { return s_PhysicalDevice; }

    [[nodiscard]] static inline SwapChain& GetSwapChain() { return *s_SwapChainPtr; }

    [[nodiscard]] static inline RenderPass& GetRederPass() { return *s_RenderPassPtr; }

    [[nodiscard]] static inline VkQueue GetGraphicsQueue() { return s_GraphicsQueue; }

    [[nodiscard]] static inline VkSurfaceKHR GetSurface() { return s_Surface; }


private:
    static inline VkDevice s_Device{};
    static inline VkPhysicalDevice s_PhysicalDevice{};
    static inline VkQueue s_GraphicsQueue{};
    static inline SwapChain* s_SwapChainPtr{};
    static inline RenderPass* s_RenderPassPtr{};
    static inline VkSurfaceKHR s_Surface{};
};
