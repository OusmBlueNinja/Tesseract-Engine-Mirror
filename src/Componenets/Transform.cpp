// TransformComponent.cpp
#include "Transform.h"

const std::string TransformComponent::name = "Transform";

TransformComponent::TransformComponent()
    : position(0.0f), rotation(0.0f), scale(1.0f)
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    scale =    glm::vec3(1.0f, 1.0f, 1.0f);

}

const std::string& TransformComponent::GetName() const
{
    return name;
}

const std::string& TransformComponent::GetStaticName()
{
    return name;
}

YAML::Node TransformComponent::Serialize()
{
    YAML::Node node;

    // Position
    {
        YAML::Node posNode;
        posNode.SetStyle(YAML::EmitterStyle::Flow);
        posNode.push_back(position.x);
        posNode.push_back(position.y);
        posNode.push_back(position.z);
        node["Position"] = posNode;
    }

    // Rotation
    {
        YAML::Node rotNode;
        rotNode.SetStyle(YAML::EmitterStyle::Flow);
        rotNode.push_back(rotation.x);
        rotNode.push_back(rotation.y);
        rotNode.push_back(rotation.z);
        node["Rotation"] = rotNode;
    }

    // Scale
    {
        YAML::Node scaleNode;
        scaleNode.SetStyle(YAML::EmitterStyle::Flow);
        scaleNode.push_back(scale.x);
        scaleNode.push_back(scale.y);
        scaleNode.push_back(scale.z);
        node["Scale"] = scaleNode;
    }

    return node;
}

void TransformComponent::Deserialize(const YAML::Node& node)
{
    if (node["Position"])
    {
        auto pos = node["Position"].as<std::vector<float>>();
        if (pos.size() == 3)
            position = glm::vec3(pos[0], pos[1], pos[2]);
    }
    if (node["Rotation"])
    {
        auto rot = node["Rotation"].as<std::vector<float>>();
        if (rot.size() == 3)
            rotation = glm::vec3(rot[0], rot[1], rot[2]);
    }
    if (node["Scale"])
    {
        auto scl = node["Scale"].as<std::vector<float>>();
        if (scl.size() == 3)
            scale = glm::vec3(scl[0], scl[1], scl[2]);
    }
}
