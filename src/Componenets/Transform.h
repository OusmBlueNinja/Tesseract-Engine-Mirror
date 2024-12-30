// Transform.h
#pragma once

#include "Component.h"
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

class TransformComponent : public Component
{
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::vec3 GetPosition() const
    {
        return position;
    }

    void SetPosition(float x, float y, float z)
    {
        position = {x, y, z};
    }

    glm::vec3 GetRotation() const
    {
        return rotation;
    }

    void SetRotation(float x, float y, float z)
    {
        rotation = {x, y, z};
    }

    TransformComponent();
    virtual const std::string &GetName() const override;
    static const std::string &GetStaticName();

    virtual void Update(float deltaTime) override;

    // Serialization methods
    virtual YAML::Node Serialize() override;
    virtual void Deserialize(const YAML::Node &node) override;

private:
    static const std::string name;
};
