#include "Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include "jul/GameTime.h"
#include "jul/Input.h"
#include "jul/MathExtensions.h"
#include "SwapChain.h"
#include "vulkanbase/VulkanGlobals.h"

Camera::Camera(const glm::vec3& origin, float fovAngle) :
    m_Origin{ origin },
    m_TargetOrigin{ m_Origin },
    m_AspectRatio{ VulkanGlobals::GetSwapChain().GetAspect() },
    m_FovAngle{ fovAngle }
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

    m_Pitch = jul::math::LerpSmooth(m_Pitch, m_TargetPitch, jul::GameTime::GetDeltaTimeF(), ROTATE_LERP_DURATION);
    m_Yaw = jul::math::LerpSmooth(m_Yaw, m_TargetYaw, jul::GameTime::GetDeltaTimeF(), ROTATE_LERP_DURATION);

    const glm::mat4x4 pitchYawRotation = glm::yawPitchRoll(glm::radians(m_Yaw), glm::radians(m_Pitch), 0.0f);
    m_Right = pitchYawRotation[0];
    m_Up = pitchYawRotation[1];
    m_Forward = pitchYawRotation[2];

    movementInputVector = pitchYawRotation * glm::vec4(movementInputVector, 0.0f);

    m_TargetOrigin += (movementInputVector * jul::GameTime::GetDeltaTimeF() * KEY_MOVE_SPEED *
                       (m_IsBoosting ? BOOST_SPEED_MULTIPLIER : 1.0f));

    m_Origin = jul::math::LerpSmooth(m_Origin, m_TargetOrigin, jul::GameTime::GetDeltaTimeF(), MOVE_LERP_DURATION);

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

void Camera::SetAspect(float aspectRatio) { m_AspectRatio = aspectRatio; }

void Camera::UpdateViewMatrix() { m_ViewMatrix = glm::lookAt(m_Origin, m_Origin + m_Forward, m_Up); }


void Camera::UpdateProjectionMatrix()
{
    m_ProjectionMatrix =
        glm::perspective(glm::radians(m_FovAngle), m_AspectRatio, m_NearClippingPlane, m_FarClippingPlane);
}
