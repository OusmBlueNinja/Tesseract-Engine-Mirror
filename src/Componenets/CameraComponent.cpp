// CameraComponent.cpp

#include "CameraComponent.h"
#include "GameObject.h" // Ensure this is included to access GameObject
#include "Transform.h"  // Ensure Transform component is available
#include <iostream>
#include <cassert>

#include "gcml.h"

// Constructor implementation
CameraComponent::CameraComponent() : IsPerspective(true),
                                     FOV(45.0f), AspectRatio(16.0f / 9.0f),
                                     NearPlane(0.1f), FarPlane(100.0f),
                                     Left(-1.0f), Right(1.0f), Bottom(-1.0f), Top(1.0f), DefaultRuntimeCamera(false),
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
    node["IsPerspective"] = IsPerspective;
    node["DefaultRuntimeCamera"] = DefaultRuntimeCamera;

    if (IsPerspective)
    {
        node["FOV"] = FOV;
        node["AspectRatio"] = AspectRatio;
        node["NearPlane"] = NearPlane;
        node["FarPlane"] = FarPlane;
    }
    else
    {
        node["Left"] = Left;
        node["Right"] = Right;
        node["Bottom"] = Bottom;
        node["Top"] = Top;
        node["NearPlane"] = NearPlane;
        node["FarPlane"] = FarPlane;
    }
    return node;
}

void CameraComponent::Deserialize(const YAML::Node &node)
{
    if (node["IsPerspective"])
    {
        IsPerspective = node["IsPerspective"].as<bool>();
    }

    if (node["DefaultRuntimeCamera"])
    {
        DefaultRuntimeCamera = node["DefaultRuntimeCamera"].as<bool>();
    }

    if (IsPerspective)
    {
        if (node["FOV"])
            FOV = node["FOV"].as<float>();
        if (node["AspectRatio"])
            AspectRatio = node["AspectRatio"].as<float>();
        if (node["NearPlane"])
            NearPlane = node["NearPlane"].as<float>();
        if (node["FarPlane"])
            FarPlane = node["FarPlane"].as<float>();
    }
    else
    {
        if (node["Left"])
            Left = node["Left"].as<float>();
        if (node["Right"])
            Right = node["Right"].as<float>();
        if (node["Bottom"])
            Bottom = node["Bottom"].as<float>();
        if (node["Top"])
            Top = node["Top"].as<float>();
        if (node["NearPlane"])
            NearPlane = node["NearPlane"].as<float>();
        if (node["FarPlane"])
            FarPlane = node["FarPlane"].as<float>();
    }

    UpdateProjectionMatrix();
}

void CameraComponent::SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    IsPerspective = true;
    FOV = fov;
    AspectRatio = aspectRatio;
    NearPlane = nearPlane;
    FarPlane = farPlane;
    UpdateProjectionMatrix();
}

void CameraComponent::SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
    IsPerspective = false;
    Left = left;
    Right = right;
    Bottom = bottom;
    Top = top;
    NearPlane = nearPlane;
    FarPlane = farPlane;
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

void CameraComponent::Update(float _deltaTime)
{
    (void)_deltaTime; // Suppress unused parameter warning
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void CameraComponent::UpdateViewMatrix()
{
    
    if (m_Owner)
    {

        std::shared_ptr<TransformComponent> transform = m_Owner->GetComponent<TransformComponent>();

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
            DEBUG_PRINT("Transform component missing on GameObject: %s", m_Owner->name.c_str());
        }
    }
    else
    {
        DEBUG_PRINT("Game Object has no parent");
    }
}

void CameraComponent::UpdateProjectionMatrix()
{
    if (IsPerspective)
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, NearPlane, FarPlane);
    }
    else
    {
        m_ProjectionMatrix = glm::ortho(Left, Right, Bottom, Top, NearPlane, FarPlane);
    }
}
