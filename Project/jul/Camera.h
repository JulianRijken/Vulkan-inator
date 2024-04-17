#pragma once

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera
{

public:
    Camera(const glm::vec3& origin, float fovAngle);

    void Update();

    [[nodiscard]] const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

    [[nodiscard]] glm::mat4 GetOrthoProjectionMatrix() const;

    [[nodiscard]] const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

    [[nodiscard]] const glm::vec3& GetOrigin() const { return m_Origin; }

    void SetFovAngle(float fovAngle);

    void SetPosition(glm::vec3 position, bool teleport = true);

    void SetNearClipping(float value);

    void SetFarClipping(float value);

    void SetPitch(float pitch);

    void SetYaw(float yaw);

    void ChangeFovAngle(float fovAngleChange);

    void SetAspect(float aspectRatio);

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    glm::vec3 m_Origin;
    glm::vec3 m_TargetOrigin;

    float m_FovAngle{};
    float m_TargetFovAngle{};

    float m_NearClippingPlane = 0.1f;
    float m_FarClippingPlane = 100.0f;

    glm::vec3 m_Forward{ 0, 0, 1 };
    glm::vec3 m_Up{ 0, 1, 0 };
    glm::vec3 m_Right{ 1, 0, 0 };

    float m_Pitch{ 0 };
    float m_TargetPitch{ 0 };
    float m_Yaw{ 0 };
    float m_TargetYaw{ 0 };

    float m_OrthoSize{ 1 };

    bool m_IsBoosting{};

    float m_AspectRatio{};

    glm::mat4 m_ViewMatrix{};
    glm::mat4 m_ProjectionMatrix{};

    inline static constexpr float MAX_FOV{ 120.0f };
    inline static constexpr float MIN_FOV{ 20.0f };

    inline static constexpr float KEY_MOVE_SPEED{ 10.0f };
    inline static constexpr float ROTATE_SPEED{ 0.05f };
    inline static constexpr float ZOOM_SPEED{ 5.0f };

    inline static constexpr float ROTATE_LERP_DURATION{ 0.3f };
    inline static constexpr float MOVE_LERP_DURATION{ 0.7f };
    inline static constexpr float ZOOM_LERP_DURATION{ 0.5f };

    inline static constexpr float BOOST_SPEED_MULTIPLIER{ 3.0f };
};
