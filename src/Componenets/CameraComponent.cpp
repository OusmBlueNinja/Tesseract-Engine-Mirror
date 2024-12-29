// CameraComponent.cpp

#include "CameraComponent.h"
#include "GameObject.h" // Ensure this is included to access GameObject
#include "Transform.h"  // Ensure Transform component is available
#include <iostream>

// Constructor implementation
CameraComponent::CameraComponent() :
    m_IsPerspective(true),
    m_FOV(45.0f), m_AspectRatio(16.0f / 9.0f),
    m_NearPlane(0.1f), m_FarPlane(100.0f),
    m_Left(-1.0f), m_Right(1.0f), m_Bottom(-1.0f), m_Top(1.0f),
    m_ViewMatrix(1.0f), m_ProjectionMatrix(1.0f)
{
    UpdateProjectionMatrix();
}

CameraComponent::~CameraComponent()
{
    // Cleanup if necessary
}

const std::string &CameraComponent::GetName() const
{
    static const std::string name = "CameraComponent";
    return name;
}

const std::string &CameraComponent::GetStaticName()
{
    static const std::string name = "CameraComponent";
    return name;
}



YAML::Node CameraComponent::Serialize()
{
    YAML::Node node;
    node["IsPerspective"] = m_IsPerspective;
    if (m_IsPerspective)
    {
        node["FOV"] = m_FOV;
        node["AspectRatio"] = m_AspectRatio;
        node["NearPlane"] = m_NearPlane;
        node["FarPlane"] = m_FarPlane;
    }
    else
    {
        node["Left"] = m_Left;
        node["Right"] = m_Right;
        node["Bottom"] = m_Bottom;
        node["Top"] = m_Top;
        node["NearPlane"] = m_NearPlane;
        node["FarPlane"] = m_FarPlane;
    }
    return node;
}

void CameraComponent::Deserialize(const YAML::Node &node)
{
    if (node["IsPerspective"])
    {
        m_IsPerspective = node["IsPerspective"].as<bool>();
    }

    if (m_IsPerspective)
    {
        if (node["FOV"])
            m_FOV = node["FOV"].as<float>();
        if (node["AspectRatio"])
            m_AspectRatio = node["AspectRatio"].as<float>();
        if (node["NearPlane"])
            m_NearPlane = node["NearPlane"].as<float>();
        if (node["FarPlane"])
            m_FarPlane = node["FarPlane"].as<float>();
    }
    else
    {
        if (node["Left"])
            m_Left = node["Left"].as<float>();
        if (node["Right"])
            m_Right = node["Right"].as<float>();
        if (node["Bottom"])
            m_Bottom = node["Bottom"].as<float>();
        if (node["Top"])
            m_Top = node["Top"].as<float>();
        if (node["NearPlane"])
            m_NearPlane = node["NearPlane"].as<float>();
        if (node["FarPlane"])
            m_FarPlane = node["FarPlane"].as<float>();
    }

    UpdateProjectionMatrix();
    UpdateViewMatrix();
}

void CameraComponent::SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    m_IsPerspective = true;
    m_FOV = fov;
    m_AspectRatio = aspectRatio;
    m_NearPlane = nearPlane;
    m_FarPlane = farPlane;
    UpdateProjectionMatrix();
}

void CameraComponent::SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
    m_IsPerspective = false;
    m_Left = left;
    m_Right = right;
    m_Bottom = bottom;
    m_Top = top;
    m_NearPlane = nearPlane;
    m_FarPlane = farPlane;
    UpdateProjectionMatrix();
}

const glm::mat4 &CameraComponent::GetViewMatrix() const
{
    return m_ViewMatrix;
}

const glm::mat4 &CameraComponent::GetProjectionMatrix() const
{
    return m_ProjectionMatrix;
}

void CameraComponent::UpdateViewMatrix()
{
    // Retrieve the Transform component from the owning GameObject
    std::shared_ptr<TransformComponent> transform = owner->GetComponent<TransformComponent>();

    if (transform)
    {
        glm::vec3 position = transform->GetPosition();
        glm::vec3 rotation = transform->GetRotation();

        // Convert Euler angles to radians
        glm::vec3 rotRad = glm::radians(rotation);

        // Calculate forward vector
        glm::vec3 forward;
        forward.x = cos(rotRad.y) * cos(rotRad.x);
        forward.y = sin(rotRad.x);
        forward.z = sin(rotRad.y) * cos(rotRad.x);
        forward = glm::normalize(forward);

        // Define up vector (assuming Y-up)
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        m_ViewMatrix = glm::lookAt(position, position + forward, up);
    }
    else
    {
        std::cerr << "Transform component missing on GameObject: " << m_Owner->name << std::endl;
    }
}

void CameraComponent::UpdateProjectionMatrix()
{
    if (m_IsPerspective)
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
    }
    else
    {
        m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_NearPlane, m_FarPlane);
    }
}
