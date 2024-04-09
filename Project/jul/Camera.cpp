#include "Camera.h"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

Camera::Camera(const glm::vec3& origin, float fovAngle, float aspectRatio) :
    m_Origin{ origin },
    m_TargetOrigin{ m_Origin },
    m_AspectRatio{ aspectRatio },
    m_FovAngle{ fovAngle }
{
}

void Camera::Update()
{
    // constexpr float minFps{ 30.0f };
    // constexpr float maxElapsed{ 1.0f / minFps };
    // // using min to create a minimum delay
    // const float deltaTime = std::min(timer.GetElapsed(), maxElapsed);

    // // Keyboard Input
    // const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

    // // Mouse Input
    // int mouseX{}, mouseY{};
    // const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

    // const bool isRightMouseDown{ (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0 };
    // const bool isLeftMouseDown{ (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0 };

    // Vector3 inputVector{};
    // Vector3 inputVectorMouse{};


    // if(isLeftMouseDown && isRightMouseDown)
    // {
    //     inputVectorMouse.y -= static_cast<float>(mouseY) * MOUSE_MOVE_SPEED;
    //     SDL_SetRelativeMouseMode(SDL_TRUE);
    // }
    // else if(isLeftMouseDown)
    // {
    //     inputVectorMouse.z -= static_cast<float>(mouseY) * MOUSE_MOVE_SPEED;
    //     m_TargetYaw -= static_cast<float>(mouseX) * ROTATE_SPEED;
    //     SDL_SetRelativeMouseMode(SDL_TRUE);
    // }
    // else if(isRightMouseDown)
    // {
    //     m_TargetPitch -= static_cast<float>(mouseY) * ROTATE_SPEED;
    //     m_TargetYaw -= static_cast<float>(mouseX) * ROTATE_SPEED;
    //     SDL_SetRelativeMouseMode(SDL_TRUE);
    // }
    // else
    // {
    //     SDL_SetRelativeMouseMode(SDL_FALSE);
    // }

    // m_BoostingSpeed = pKeyboardState[SDL_SCANCODE_LSHIFT];

    // if(pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT])
    //     inputVector.x -= 1;

    // if(pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT])
    //     inputVector.x += 1;

    // if(pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP])
    //     inputVector.z += 1;

    // if(pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN])
    //     inputVector.z -= 1;

    // if(pKeyboardState[SDL_SCANCODE_Q])
    //     inputVector.y -= 1;

    // if(pKeyboardState[SDL_SCANCODE_E])
    //     inputVector.y += 1;


    // m_Pitch = std::lerp(m_Pitch, m_TargetPitch, deltaTime * ROTATE_LERP_SPEED);
    // m_Yaw = std::lerp(m_Yaw, m_TargetYaw, deltaTime * ROTATE_LERP_SPEED);
    m_Pitch = std::lerp(m_Pitch, m_TargetPitch, 0.0166666666f * ROTATE_LERP_SPEED);
    m_Yaw = std::lerp(m_Yaw, m_TargetYaw, 0.0166666666f * ROTATE_LERP_SPEED);


    m_Pitch = 45.0f;
    const glm::mat4x4 pitchYawRotation = glm::yawPitchRoll(glm::radians(m_Yaw), glm::radians(m_Pitch), 0.0f);
    m_Right = pitchYawRotation[0];
    m_Up = pitchYawRotation[1];
    m_Forward = pitchYawRotation[2];


    // m_Forward = { UNIT_FOARWARD };  // Because we transform the point we need to reset it first
    // m_Forward = pitchYawRotation * glm::vec4(m_Forward, 0.0f);

    // m_Right = glm::normalize(glm::cross({ 0, 1, 0 }, m_Forward));
    // m_Up = glm::normalize(glm::cross(m_Forward, m_Right));


    // inputVector = pitchYawRotation.TransformVector(inputVector);
    // m_TargetOrigin += (inputVector * deltaTime * KEY_MOVE_SPEED * (m_BoostingSpeed ? BOOST_SPEED_MULTIPLIER : 1.0f))
    // +
    //                   inputVectorMouse;
    // m_Origin = Lerp(m_Origin, m_TargetOrigin, deltaTime * MOVE_LERP_SPEED);
    m_Origin = m_TargetOrigin;

    // Update Matrices
    // Try to optimize this - should only be called once or when fov/aspectRatio changes
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::SetFovAngle(float fovAngle) { m_FovAngle = fovAngle; }

void Camera::SetPosition(glm::vec3 position, bool teleport)
{
    m_TargetOrigin = position;

    if(teleport)
        m_Origin = position;
}

void Camera::SetNearClipping(float value) { m_NearClippingPlane = value; }

void Camera::SetFarClipping(float value) { m_FarClippingPlane = value; }

void Camera::SetPitch(float pitch)
{
    m_Pitch = pitch;
    m_TargetPitch = pitch;
}

void Camera::SetYaw(float yaw)
{
    m_Yaw = yaw;
    m_TargetYaw = yaw;
}

void Camera::ChangeFovAngle(float fovAngleChange)
{
    m_FovAngle += fovAngleChange;

    if(m_FovAngle < MAX_FOV)
    {
        m_FovAngle = MAX_FOV;
        return;
    }

    if(m_FovAngle > MIN_FOV)
    {
        m_FovAngle = MIN_FOV;
        return;
    }
}

void Camera::UpdateViewMatrix() { m_ViewMatrix = glm::lookAt(m_Origin, m_Origin + m_Forward, m_Up); }

void Camera::UpdateProjectionMatrix()
{
    m_ProjectionMatrix =
        glm::perspective(glm::radians(m_FovAngle), m_AspectRatio, m_NearClippingPlane, m_FarClippingPlane);
}
