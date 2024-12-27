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

    TransformComponent();
    virtual const std::string& GetName() const override;
    static const std::string& GetStaticName();

    // Serialization methods
    virtual YAML::Node Serialize() override;
    virtual void Deserialize(const YAML::Node& node) override;

private:
    static const std::string name;
};
