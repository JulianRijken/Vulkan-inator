#pragma once

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera
{

public:
    Camera(const glm::vec3& origin, float fovAngle, float aspectRatio);

    void Update();

    [[nodiscard]] const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

    [[nodiscard]] const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

    [[nodiscard]] const glm::vec3& GetOrigin() const { return m_Origin; }

    void SetFovAngle(float fovAngle);

    void SetPosition(glm::vec3 position, bool teleport = true);

    void SetNearClipping(float value);

    void SetFarClipping(float value);

    void SetPitch(float pitch);

    void SetYaw(float yaw);

    void ChangeFovAngle(float fovAngleChange);


private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    glm::vec3 m_Origin;
    glm::vec3 m_TargetOrigin;

    float m_FovAngle{};

    float m_NearClippingPlane = 0.1f;
    float m_FarClippingPlane = 100.0f;

    glm::vec3 m_Forward{ 0, 0, 1 };
    glm::vec3 m_Up{ 0, 1, 0 };
    glm::vec3 m_Right{ 1, 0, 0 };

    float m_Pitch{};
    float m_TargetPitch{ -45 };
    float m_Yaw{};
    float m_TargetYaw{};

    bool m_BoostingSpeed{};

    float m_AspectRatio{};

    glm::mat4 m_ViewMatrix{};
    glm::mat4 m_ProjectionMatrix{};


    // inline static constexpr glm::vec3 UNIT_FOARWARD{ 0, 0, 1 };
    // inline static constexpr glm::vec3 UNIT_UP{ 0, 1, 0 };
    // inline static constexpr glm::vec3 UNIT_RIGHT{ 1, 0, 0 };

    inline static constexpr float MAX_FOV{ 175.0f };
    inline static constexpr float MIN_FOV{ 5.0f };

    inline static constexpr float KEY_MOVE_SPEED{ 70.0f };
    inline static constexpr float MOUSE_MOVE_SPEED{ 0.07f };
    inline static constexpr float ROTATE_SPEED{ 0.001f };

    inline static constexpr float ROTATE_LERP_SPEED{ 20.0f };
    inline static constexpr float MOVE_LERP_SPEED{ 10.0f };

    inline static constexpr float BOOST_SPEED_MULTIPLIER{ 3.0f };
};