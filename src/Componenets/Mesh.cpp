
#include "Mesh.h"
#include "Engine/AssetManager.h"
#include "gcml.h"

#include "../Engine/AssetManager.h"

extern AssetManager g_AssetManager;

const std::string MeshComponent::name = "Mesh";

MeshComponent::MeshComponent()
    : MeshPath("assets/models/DefaultMesh.obj")
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
    (void)deltaTime;
    return;
}



void MeshComponent::Draw(Shader* shader)
{
    for (auto &submesh : submeshes)
    {
        submesh.Draw(shader);
    }
}


YAML::Node MeshComponent::Serialize()
{
    YAML::Node node;

    // Serialize each submesh
    node["MeshPath"] = MeshPath;

    YAML::Node submeshesNode;
    for (const auto &submesh : submeshes)
    {
        YAML::Node submeshNode;
        submeshNode["vao"] = static_cast<int>(submesh.vao);
        submeshNode["indexCount"] = static_cast<int>(submesh.indices.size());

        // Serialize Textures
        YAML::Node texturesNode;
        for (const auto &texture : submesh.textures)
        {
            YAML::Node texNode;
            texNode["id"] = static_cast<int>(texture.id);
            texNode["type"] = texture.type;
            texNode["path"] = texture.path;
            texturesNode.push_back(texNode);
        }
        submeshNode["textures"] = texturesNode;

        submeshesNode.push_back(submeshNode);
    }
    node["submeshes"] = submeshesNode;


    return node;
}

void MeshComponent::Deserialize(const YAML::Node &node)
{
    if (node["MeshPath"])
    {
        MeshPath = node["MeshPath"].as<std::string>();

        DEBUG_PRINT("Loading Mesh: %s", MeshPath.c_str());

        std::shared_ptr<Model> model = g_AssetManager.loadAsset<Model>(AssetType::MODEL, MeshPath.c_str());

        if (!model)
        {
            DEBUG_PRINT("Failed to load model: %s", MeshPath.c_str());
            return;
        }

        DEBUG_PRINT("Model loaded successfully with %zu submeshes.", model->submeshes.size());

        // Assign submeshes
        submeshes = std::move(model->submeshes);
    }
    else
    {
        // Handle cases where submeshes are stored directly
        if (node["submeshes"])
        {
            const YAML::Node &submeshesNode = node["submeshes"];
            for (const auto &submeshNode : submeshesNode)
            {
                Submesh submesh;

                if (submeshNode["vao"])
                {
                    submesh.vao = submeshNode["vao"].as<int>();
                }
                if (submeshNode["indexCount"])
                {
                    submesh.indices.reserve(submeshNode["indexCount"].as<int>());
                    // Assuming indices are stored elsewhere or need to be loaded
                }
                if (submeshNode["textures"])
                {
                    const YAML::Node &texturesNode = submeshNode["textures"];
                    for (const auto &texNode : texturesNode)
                    {
                        Texture texture;
                        texture.id = texNode["id"].as<int>();
                        texture.type = texNode["type"].as<std::string>();
                        texture.path = texNode["path"].as<std::string>();
                        submesh.textures.push_back(texture);
                    }
                }


                submeshes.push_back(std::move(submesh));
            }
        }
    }
}
