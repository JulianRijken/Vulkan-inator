#pragma once
#include <GLFW/glfw3.h>

#include <array>
#include <glm/vec2.hpp>

class Input
{
public:
    static void OnKeyDown(int key);

    static void OnKeyUp(int key);

    static void OnMouseMove(glm::vec2 mousePosition);

    static void Update();

    static bool GetKeyDown(int key);
    static bool GetKeyUp(int key);
    static bool GetKeyHeld(int key);


    static glm::vec2 GetMousePosition();
    static glm::vec2 GetMouseDelta();

private:
    static inline std::array<int, GLFW_KEY_LAST> s_Keys{};

    static inline glm::vec2 s_CurrentMousePosition{};

    static inline glm::vec2 s_AccumulatedMouseDelta{};
    static inline glm::vec2 s_MouseDelta{};
};

