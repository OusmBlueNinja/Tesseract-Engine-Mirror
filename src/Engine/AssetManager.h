#pragma once

#include <string>
#include <unordered_map>



// --------------------------------------------------------------------------
// 1. Define an enum for various asset types
// --------------------------------------------------------------------------
enum class AssetType
{
    TEXTURE,
    SHADER,
    SOUND,
    // Add more as needed
};

// --------------------------------------------------------------------------
// 2. A placeholder "GenericAsset" struct
//
//    In a real system, you might have a base class (e.g., class Asset)
//    with derived classes, or store typed data in union/variant.
// --------------------------------------------------------------------------
struct GenericAsset
{
    void* data = nullptr;  // Points to the actual loaded asset data
    // In a real engine, you'd store more metadata here.
};

// --------------------------------------------------------------------------
// 3. The AssetManager class
// --------------------------------------------------------------------------
class AssetManager
{
public:
    // Constructor / Destructor
    AssetManager()  = default;
    ~AssetManager() = default;

    // loadAsset() returns a pointer to the loaded asset data.
    // In a real engine, you might return a typed pointer or a handle.
    void* loadAsset(AssetType type, const std::string& path);

private:
    // Map from "type+path" -> GenericAsset
    std::unordered_map<std::string, GenericAsset> m_AssetMap;

    // Generate a unique key for each asset based on type + path
    std::string generateKey(AssetType type, const std::string& path);

    // Actually load asset data from disk, specialized by AssetType
    void* loadAssetFromDisk(AssetType type, const std::string& path);
};
