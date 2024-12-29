// CameraComponent.h

#pragma once

#include "Component.h"
#include "GameObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <yaml-cpp/yaml.h>

class CameraComponent : public Component
{
public:
    // Updated constructor to accept GameObject*
    CameraComponent();
    virtual ~CameraComponent();

    // Overridden methods from Component
    virtual const std::string& GetName() const override;

    static const std::string& GetStaticName();

    virtual YAML::Node Serialize() override;
    virtual void Deserialize(const YAML::Node& node) override;

    // Camera-specific methods
    void SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
    void SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetProjectionMatrix() const;

private:
    // Projection parameters
    bool m_IsPerspective;
    float m_FOV;
    float m_AspectRatio;
    float m_NearPlane;
    float m_FarPlane;

    float m_Left;
    float m_Right;
    float m_Bottom;
    float m_Top;

    // Matrices
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;

    GameObject* owner;



    // Update matrices
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
};
