#pragma once

#include <GLFW/glfw3.h>

#include "vulkan/vulkan_core.h"

class GUI
{
public:
    GUI(GUI&&) = delete;
    GUI(const GUI&) = delete;
    GUI& operator=(GUI&&) = delete;
    GUI& operator=(const GUI&) = delete;

    static void Init(GLFWwindow* windowPtr);
    static void Cleanup();
    static void NewFrame();
    static void EndFrame();

private:
    inline static VkDescriptorPool g_GUIPool;
};
