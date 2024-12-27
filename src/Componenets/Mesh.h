
// Mesh.h
#pragma once

#include "Component.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

class MeshComponent : public Component
{
public:
    GLuint vao = 0;           // Vertex Array Object
    GLuint indexCount = 0;    // Number of indices to draw
    GLuint textureID = 0;     // The texture handle


    std::string MeshPath;

    MeshComponent();
    virtual const std::string& GetName() const override;
    static const std::string& GetStaticName();

    // Serialization methods
    virtual YAML::Node Serialize() override;
    virtual void Deserialize(const YAML::Node& node) override;

private:
    static const std::string name;
};
