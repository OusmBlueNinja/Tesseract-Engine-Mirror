#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <vector>

// Forward-declare your Shader class
class Shader;

// Define types of assets
enum class AssetType
{
    TEXTURE,
    SHADER,
    SOUND,
    MODEL,
};

// A simple struct to hold the generic pointer
struct GenericAsset
{
    void* data = nullptr;
};

struct Vertex {
    float position[3];
    float texCoord[2];
    float normal[3];
};


struct Model {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint textureID;

    Model()
        : vao(0), vbo(0), ebo(0), textureID(0) {}
};

// The main AssetManager
class AssetManager
{
public:
    AssetManager()  = default;
    ~AssetManager() = default;

    // Load an asset from disk (texture, shader, etc.)
    // Returns a void* pointer to the loaded resource.
    //   - For TEXTURE, cast to (GLuint) 
    //   - For SHADER, cast to (Shader*) 
    //   - For SOUND, cast to whatever you store
    void* loadAsset(AssetType type, const std::string& path);

private:
    // Cache of already loaded assets: key = "type + path"
    std::unordered_map<std::string, GenericAsset> m_AssetMap;

    // Generate the unique key
    std::string generateKey(AssetType type, const std::string& path);

    // Actual loading from disk
    void* loadAssetFromDisk(AssetType type, const std::string& path);
};
