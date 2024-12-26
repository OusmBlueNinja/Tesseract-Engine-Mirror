#include "Engine/AssetManager.h"
#include <iostream>

// Include your Shader class
#include "Rendering/Shader.h"

#include "Windows/LoggerWindow.h"

// Include OpenGL loader (GLEW) for texture creation
#include <GL/glew.h>

// For texture loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

int LoaddedAssets = 0;

extern LoggerWindow *g_LoggerWindow;

void* AssetManager::loadAsset(AssetType type, const std::string& path)
{
    // 1) Create a unique key for cache lookup
    std::string key = generateKey(type, path);

    // 2) Check if it’s already loaded
    auto it = m_AssetMap.find(key);
    if (it != m_AssetMap.end())
    {
        // Return existing pointer
        return it->second.data;
    }

    // 3) Not loaded yet, load from disk
    void* assetData = loadAssetFromDisk(type, path);
    if (!assetData)
    {
        std::cerr << "[AssetManager] Failed to load asset: " << path << std::endl;
        return nullptr;
    }

    // 4) Store in cache
    GenericAsset newAsset;
    newAsset.data = assetData;
    m_AssetMap[key] = newAsset;

    LoaddedAssets += 1;

    g_LoggerWindow->AddLog("Loadded Asset: %s", path.c_str());

    // 5) Return pointer
    return assetData;
}

std::string AssetManager::generateKey(AssetType type, const std::string& path)
{
    return std::to_string(static_cast<int>(type)) + ":" + path;
}

void* AssetManager::loadAssetFromDisk(AssetType type, const std::string& path)
{
    switch (type)
    {
    case AssetType::TEXTURE:
    {
        // --------------------------------------------
        // Load a texture with stb_image
        // --------------------------------------------
        std::cout << "[AssetManager] Loading TEXTURE from: " << path << std::endl;

        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!data)
        {
            std::cerr << "[AssetManager] stb_image failed for: " << path << std::endl;
            return nullptr;
        }

        GLenum format = GL_RGBA;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        // if channels == 4, already GL_RGBA

        GLuint texID = 0;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                     format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);

        // Return as void*
        return reinterpret_cast<void*>(static_cast<uintptr_t>(texID));
    }

    case AssetType::SHADER:
    {
        // --------------------------------------------
        // Load a shader using your existing "Shader" class
        // --------------------------------------------
        // Example usage: path = "shaders/UnlitMaterial" => 
        //   loads "shaders/UnlitMaterial.vert" and "shaders/UnlitMaterial.frag"
        std::cout << "[AssetManager] Loading SHADER from: " << path << std::endl;

        // Create a new Shader object on the heap
        Shader* newShader = new Shader();

        // Build actual paths from the base path
        std::string vertPath = path + ".vert";
        std::string fragPath = path + ".frag";

        // Attempt to load
        if (!newShader->Load(vertPath, fragPath))
        {
            std::cerr << "[AssetManager] Could not load shader: "
                      << vertPath << " / " << fragPath << std::endl;
            delete newShader; // Cleanup
            return nullptr;
        }

        // Return as void*
        return reinterpret_cast<void*>(newShader);
    }

    case AssetType::SOUND:
    {
        std::cout << "[AssetManager] Loading SOUND from: " << path << std::endl;
        // Stub or real code to load .wav / .ogg
        return (void*)0xAAAA8888; // placeholder
    }

    default:
    {
        std::cerr << "[AssetManager] Unknown asset type for: " << path << std::endl;
        return nullptr;
    }
    }
}
