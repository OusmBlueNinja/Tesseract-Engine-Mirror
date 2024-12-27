#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <vector>
#include <variant>
#include "gcml.h"
#include "stdexcept"
#include <iostream>

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
    void *data = nullptr;
};

struct Vertex
{
    float position[3];
    float texCoord[2];
    float normal[3];
};

struct Model
{
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
    AssetManager() = default;
    ~AssetManager() = default;

    using AssetVariant = std::variant<GLuint, Shader *, std::string, Model *>;

    // Load an asset from disk (texture, shader, etc.)
    // Returns a void* pointer to the loaded resource.
    //   - For TEXTURE, cast to (GLuint)
    //   - For SHADER, cast to (Shader*)
    //   - For SOUND, cast to whatever you store

    // Template function to load an asset
    // Template function to load an asset
    template <typename T>
    T loadAsset(AssetType type, const std::string &path)
    {
        // 1) Create a unique key for cache lookup
        std::string key = generateKey(type, path);

        // 2) Check if it’s already loaded
        auto it = m_AssetMap.find(key);
        if (it != m_AssetMap.end())
        {
            // Debug: Log the variant type
            std::cout << "[AssetManager] Found asset in map." << std::endl;
            if (std::holds_alternative<T>(it->second))
            {
                return std::get<T>(it->second);
            }
            else
            {
                throw std::runtime_error("Asset type mismatch for key: " + key);
            }
        }

        // 3) Not loaded yet, load from disk
        AssetVariant assetData = loadAssetFromDisk(type, path);

        if (assetData.index() == std::variant_npos)
        {
            DEBUG_PRINT("[AssetManager] Failed to load asset: %s", path.c_str());
            // Replace NULL with 0 for non-pointer types
            if constexpr (std::is_pointer_v<T>)
            {
                return nullptr; // For pointers
            }
            else
            {
                return 0; // For non-pointer types
            }
        }

        // 4) Store in cache
        m_AssetMap[key] = assetData;

        DEBUG_PRINT("[AssetManager] Loaded Asset: %s", path.c_str());

        // 5) Return the loaded asset
        return std::get<T>(assetData);
    }

    void DebugAssetMap();

private:
    // Cache of already loaded assets: key = "type + path"
    std::unordered_map<std::string, AssetVariant> m_AssetMap;

    AssetVariant loadAssetFromDisk(AssetType type, const std::string &path);
    // Generate the unique key
    std::string generateKey(AssetType type, const std::string &path);
};
