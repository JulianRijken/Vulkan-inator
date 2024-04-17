#include "Input.h"

void Input::OnKeyDown(int key) { s_Keys[key] = 1; }

void Input::OnKeyUp(int key) { s_Keys[key] = 0; }

void Input::OnMouseMove(glm::vec2 mousePosition)
{
    static glm::vec2 s_LastMousePosition{};
    static bool s_LastPositionUpdated{ false };

    s_CurrentMousePosition = mousePosition;

    if(s_LastPositionUpdated == false)
    {
        s_LastMousePosition = s_CurrentMousePosition;
        s_LastPositionUpdated = true;
    }

    s_AccumulatedMouseDelta += s_CurrentMousePosition - s_LastMousePosition;
    s_LastMousePosition = s_CurrentMousePosition;
}

void Input::OnMouseScroll(glm::vec2 scrollDelta) { s_AccumulatedScrollDelta += scrollDelta; }

void Input::Update()
{
    s_ScrollDelta = s_AccumulatedScrollDelta;
    s_AccumulatedScrollDelta = {};

    s_MouseDelta = s_AccumulatedMouseDelta;
    s_AccumulatedMouseDelta = {};

    for(auto&& key : s_Keys)
    {
        if(key == 0)
            continue;
        key++;
    }
}

bool Input::GetKeyDown(int key) { return s_Keys[key] == 1; }

bool Input::GetKeyUp(int key) { return s_Keys[key] == 0; }

bool Input::GetKeyHeld(int key) { return s_Keys[key] >= 1; }

glm::vec2 Input::GetMousePosition() { return s_CurrentMousePosition; }

glm::vec2 Input::GetMouseDelta() { return s_MouseDelta; }

glm::vec2 Input::GetScrollDelta() { return s_ScrollDelta; }
