#include "Camera.h"

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include "jul/GameTime.h"
#include "jul/Input.h"
#include "jul/MathExtensions.h"
#include "SwapChain.h"
#include "vulkanbase/VulkanGlobals.h"

Camera::Camera(const glm::vec3& position, float fovAngle) :
    m_Position{ position },
    m_TargetPosition{ m_Position },
    m_AspectRatio{ VulkanGlobals::GetSwapChain().GetAspect() },
    m_FovAngle{ fovAngle },
    m_TargetFovAngle{ m_FovAngle }
{
}

void Camera::Update()
{
    const glm::vec2 mouseDelta = Input::GetMouseDelta();
    m_TargetPitch += mouseDelta.y * ROTATE_SPEED;
    m_TargetYaw -= mouseDelta.x * ROTATE_SPEED;

    m_IsBoosting = Input::GetKeyHeld(GLFW_KEY_LEFT_SHIFT);

    glm::vec3 movementInputVector{};
    if(Input::GetKeyHeld(GLFW_KEY_W))
        movementInputVector.z += 1;
    if(Input::GetKeyHeld(GLFW_KEY_S))
        movementInputVector.z -= 1;
    if(Input::GetKeyHeld(GLFW_KEY_A))
        movementInputVector.x += 1;
    if(Input::GetKeyHeld(GLFW_KEY_D))
        movementInputVector.x -= 1;
    if(Input::GetKeyHeld(GLFW_KEY_E))
        movementInputVector.y += 1;
    if(Input::GetKeyHeld(GLFW_KEY_Q))
        movementInputVector.y -= 1;

    if(Input::GetKeyDown(GLFW_KEY_P))
        std::cout << "Camera Position: (" << m_Position.x << ", " << m_Position.y << ", " << m_Position.z << ")"
                  << std::endl;

    ChangeFovAngle(Input::GetScrollDelta().y * -ZOOM_SPEED);
    m_FovAngle =
        jul::math::LerpSmooth(m_FovAngle, m_TargetFovAngle, jul::GameTime::GetDeltaTimeF(), ZOOM_LERP_DURATION);

    m_Pitch = jul::math::LerpSmooth(m_Pitch, m_TargetPitch, jul::GameTime::GetDeltaTimeF(), ROTATE_LERP_DURATION);
    m_Yaw = jul::math::LerpSmooth(m_Yaw, m_TargetYaw, jul::GameTime::GetDeltaTimeF(), ROTATE_LERP_DURATION);

    const glm::mat4x4 pitchYawRotation = glm::yawPitchRoll(glm::radians(m_Yaw), glm::radians(m_Pitch), 0.0f);
    m_Right = pitchYawRotation[0];
    m_Up = pitchYawRotation[1];
    m_Forward = pitchYawRotation[2];

    movementInputVector = pitchYawRotation * glm::vec4(movementInputVector, 0.0f);

    m_TargetPosition += (movementInputVector * jul::GameTime::GetDeltaTimeF() * KEY_MOVE_SPEED *
                         (m_IsBoosting ? BOOST_SPEED_MULTIPLIER : 1.0f));

    m_Position =
        jul::math::LerpSmooth(m_Position, m_TargetPosition, jul::GameTime::GetDeltaTimeF(), MOVE_LERP_DURATION);

    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

glm::mat4 Camera::GetOrthoProjectionMatrix() const
{
    return glm::ortho(
        -m_OrthoSize * m_AspectRatio, m_OrthoSize * m_AspectRatio, -m_OrthoSize, m_OrthoSize, -1.0f, 1.0f);
}

void Camera::SetFovAngle(float fovAngle) { m_FovAngle = fovAngle; }

void Camera::SetPosition(glm::vec3 position, bool teleport)
{
    m_TargetPosition = position;

    if(teleport)
        m_Position = position;
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
    m_TargetFovAngle = std::clamp(m_TargetFovAngle + fovAngleChange, MIN_FOV, MAX_FOV);
}

void Camera::SetAspect(float aspectRatio) { m_AspectRatio = aspectRatio; }

void Camera::UpdateViewMatrix() { m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Forward, m_Up); }


void Camera::UpdateProjectionMatrix()
{
    m_ProjectionMatrix =
        glm::perspective(glm::radians(m_FovAngle), m_AspectRatio, m_NearClippingPlane, m_FarClippingPlane);
}
