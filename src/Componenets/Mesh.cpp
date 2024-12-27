
#include "Mesh.h"
#include "Engine/AssetManager.h"
#include "gcml.h"

#include "../Engine/AssetManager.h"


extern AssetManager *g_AssetManager;

//TODO: Make this have a OBJ path, make indexCount derive from AssetManager
//TODO: and make texture id also get from AssetManager
//?     Procastinate

const std::string MeshComponent::name = "Mesh";

MeshComponent::MeshComponent()
    : vao(0), indexCount(0), textureID(0), MeshPath("assets/models/DefaultMesh.obj")
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

    node["MeshPath"] = static_cast<std::string>(MeshPath);







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


    if (node["MeshPath"])
    {
        MeshPath = static_cast<std::string>(node["MeshPath"].as<std::string>());
        g_AssetManager->DebugAssetMap();

        #if 1

        DEBUG_PRINT("Loading Mesh: >%s<", MeshPath.c_str());

        Model* model = g_AssetManager->loadAsset<Model*>(AssetType::MODEL, MeshPath.c_str());
        DEBUG_PRINT("Model loaded successfully with %lld vertices and %lld indices.", model->vertices.size(),  model->indices.size());
        
        #else

        DEBUG_PRINT("Automatic Mesh Loading Disabled.");

        #endif
    }
}
