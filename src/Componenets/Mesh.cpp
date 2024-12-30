
#include "Mesh.h"
#include "Engine/AssetManager.h"
#include "gcml.h"

#include "../Engine/AssetManager.h"

extern AssetManager g_AssetManager;

const std::string MeshComponent::name = "Mesh";

MeshComponent::MeshComponent()
    : vao(0), indexCount(0), MeshPath("assets/models/DefaultMesh.obj")
{
}

const std::string &MeshComponent::GetName() const
{
    return name;
}

const std::string &MeshComponent::GetStaticName()
{
    return name;
}

void MeshComponent::Update(float deltaTime)
{
    return;
}
YAML::Node MeshComponent::Serialize()
{
    YAML::Node node;

    node["vao"] = static_cast<int>(vao);
    node["indexCount"] = static_cast<int>(indexCount);

    // Serialize Textures
    YAML::Node texturesNode;
    for (const auto &texture : textures)
    {
        YAML::Node texNode;
        texNode["id"] = static_cast<int>(texture.id);
        texNode["type"] = texture.type;
        texNode["path"] = texture.path;
        texturesNode.push_back(texNode);
    }
    node["textures"] = texturesNode;

    node["MeshPath"] = MeshPath;

    return node;
}

void MeshComponent::Deserialize(const YAML::Node &node)
{
    if (node["MeshPath"])
    {
        MeshPath = node["MeshPath"].as<std::string>();

        DEBUG_PRINT("Loading Mesh: %s", MeshPath.c_str());

        Model *model = g_AssetManager.loadAsset<Model *>(AssetType::MODEL, MeshPath.c_str());

        if (!model)
        {
            DEBUG_PRINT("Failed to load model: %s", MeshPath.c_str());
            return;
        }

        DEBUG_PRINT("Model loaded successfully with %zu vertices and %zu indices.", 
                    model->vertices.size(), model->indices.size());

        // Assign VAO and index count
        if (model->vao != 0)
        {
            vao = model->vao;
        }
        else if (node["vao"])
        {
            vao = node["vao"].as<int>();
        }

        if (model->indices.size() != 0)
        {
            indexCount = static_cast<GLuint>(model->indices.size());
        }
        else if (node["indexCount"])
        {
            indexCount = node["indexCount"].as<int>();
        }

        // Assign Textures
        if (!model->textures.empty())
        {
            textures = model->textures;
        }
        else if (node["textures"])
        {
            const YAML::Node &texturesNode = node["textures"];
            for (const auto &texNode : texturesNode)
            {
                Texture texture;
                texture.id = texNode["id"].as<int>();
                texture.type = texNode["type"].as<std::string>();
                texture.path = texNode["path"].as<std::string>();
                textures.push_back(texture);
            }
        }
    }
    else
    {
        if (node["vao"])
        {
            vao = node["vao"].as<int>();
        }
        if (node["indexCount"])
        {
            indexCount = node["indexCount"].as<int>();
        }
        if (node["textures"])
        {
            const YAML::Node &texturesNode = node["textures"];
            for (const auto &texNode : texturesNode)
            {
                Texture texture;
                texture.id = texNode["id"].as<int>();
                texture.type = texNode["type"].as<std::string>();
                texture.path = texNode["path"].as<std::string>();
                textures.push_back(texture);
            }
        }
    }
}