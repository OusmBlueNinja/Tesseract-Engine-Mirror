
#include "Mesh.h"
#include "Engine/AssetManager.h"
#include "gcml.h"

#include "../Engine/AssetManager.h"
#include "Windows/LoggerWindow.h"

extern AssetManager g_AssetManager;
extern LoggerWindow *g_LoggerWindow;

const std::string MeshComponent::name = "Mesh";

MeshComponent::MeshComponent()
    : MeshPath("assets/models/DefaultMesh.obj")
{
}

MeshComponent::~MeshComponent()
{
    for (auto &submesh : submeshes)
    {
        // Delete OpenGL buffers associated with the submesh
        if (submesh.vbo != 0)
        {
            glDeleteBuffers(1, &submesh.vbo);
            submesh.vbo = 0;
        }
        if (submesh.ebo != 0)
        {
            glDeleteBuffers(1, &submesh.ebo);
            submesh.ebo = 0;
        }
        if (submesh.vao != 0)
        {
            glDeleteVertexArrays(1, &submesh.vao);
            submesh.vao = 0;
        }

        // Clear textures associated with the submesh
        for (const auto &texture : submesh.textures)
        {
            if (texture.id != 0)
            {
                glDeleteTextures(1, &texture.id);
            }
        }

        // Clear submesh data
        submesh.textures.clear();
        submesh.vertices.clear();
        submesh.indices.clear();
    }

    // Clear the submesh container
    submeshes.clear();
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

void MeshComponent::Draw(Shader *shader)
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
    node["MeshPath"] = MeshPath;
    node["submeshes_len"] = submeshes.size();
    node["submeshes"] = submeshesNode;

    return node;
}

void MeshComponent::Deserialize(const YAML::Node &node)
{
    int submeshes_len = 0;
    if (node["submeshes_len"])
    {
        submeshes_len = node["submeshes_len"].as<int>();
    }
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

        if (submeshes_len != static_cast<int>(model->submeshes.size()))
        {
            g_LoggerWindow->AddLog("[Mesh] Size Mismatch [%d:%d]: Check for Curupted Scene Files", submeshes_len, static_cast<int>(submeshes.size()));
        }

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
            if (submeshes_len != static_cast<int>(submeshes.size()))
            {
                g_LoggerWindow->AddLog("[Mesh] Size Mismatch [%d:%d]: Check for Curupted Scene Files", submeshes_len, static_cast<int>(submeshes.size()));
            }
        }
    }
}
