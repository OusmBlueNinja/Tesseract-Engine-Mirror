// Mesh.h
#pragma once

#include "Component.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>
#include "Engine/AssetManager.h"

// In MeshComponent.h

class MeshComponent : public Component
{
public:
    std::vector<Submesh> submeshes;    // List of submeshes
    std::string MeshPath;

    static const std::string name;



    MeshComponent();
    ~MeshComponent();

    virtual const std::string& GetName() const override;
    static const std::string& GetStaticName();

    virtual void Update(float deltaTime) override;

    // Serialization methods
    virtual YAML::Node Serialize() override;
    virtual void Deserialize(const YAML::Node& node) override;

    // Render the mesh
    void Draw(Shader* shader);


};

