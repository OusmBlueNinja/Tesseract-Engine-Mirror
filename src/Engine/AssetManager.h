#pragma once

#include <string>
#include <unordered_map>

// Forward-declare your Shader class
class Shader;

// Define types of assets
enum class AssetType
{
    TEXTURE,
    SHADER,
    SOUND,
    // Add more as you need
};

// A simple struct to hold the generic pointer
struct GenericAsset
{
    void* data = nullptr;
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
