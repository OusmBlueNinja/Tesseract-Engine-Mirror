
#include "Mesh.h"

const std::string MeshComponent::name = "Mesh";

MeshComponent::MeshComponent()
    : vao(0), indexCount(0), textureID(0)
{
}

const std::string& MeshComponent::GetName() const
{
    return name;
}

const std::string& MeshComponent::GetStaticName()
{
    return name;
}

YAML::Node MeshComponent::Serialize()
{
    YAML::Node node;
    
    node["vao"] = static_cast<int>(vao);
    node["indexCount"] = static_cast<int>(indexCount);
    node["textureID"] = static_cast<int>(textureID);



    return node;
}

void MeshComponent::Deserialize(const YAML::Node& node)
{
    if (node["vao"])
    {
        vao = static_cast<int>(node["vao"].as<int>());
    }
    if (node["indexCount"])
    {
        indexCount = static_cast<int>(node["indexCount"].as<int>());
    }
    if (node["textureID"])
    {
        textureID = static_cast<int>(node["textureID"].as<int>());
    }
}
