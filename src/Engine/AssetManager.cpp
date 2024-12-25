#include "AssetManager.h"
#include <iostream>
#include <GL/glew.h> // or <GL/glew.h>, whichever you use

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void* AssetManager::loadAsset(AssetType type, const std::string& path)
{
    // 1) Generate a unique key for this type+path
    std::string key = generateKey(type, path);

    // 2) Check if this asset is already loaded/cached
    auto it = m_AssetMap.find(key);
    if (it != m_AssetMap.end())
    {
        // Already loaded, return existing data
        return it->second.data;
    }

    // 3) Not loaded yet, so load from disk
    void* assetData = loadAssetFromDisk(type, path);
    if (!assetData)
    {
        std::cerr << "[AssetManager] Failed to load asset: " << path << std::endl;
        return nullptr;
    }

    // 4) Cache the result in m_AssetMap
    GenericAsset newAsset;
    newAsset.data = assetData;
    m_AssetMap[key] = newAsset;

    // 5) Return the new asset pointer
    return assetData;
}

std::string AssetManager::generateKey(AssetType type, const std::string& path)
{
    // Combine the numeric type and path into a single string key
    return std::to_string(static_cast<int>(type)) + ":" + path;
}

void* AssetManager::loadAssetFromDisk(AssetType type, const std::string& path)
{
    switch (type)
    {
        case AssetType::TEXTURE:
        {
            // ----------------------------------------------------------
            // REAL CODE: load an image file (PNG, JPG, etc.) with stb_image
            // and upload it as an OpenGL texture.
            // ----------------------------------------------------------
            std::cout << "[AssetManager] Loading TEXTURE from " << path << std::endl;

            // 1) Load pixels using stb_image
            int width, height, channels;
            unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
            if (!data)
            {
                std::cerr << "[AssetManager] stb_image failed to load: " << path << std::endl;
                return nullptr;
            }

            // 2) Determine the pixel format
            GLenum format = GL_RGBA; // default
            if (channels == 1)
                format = GL_RED;
            else if (channels == 3)
                format = GL_RGB;
            // if channels == 4, already RGBA

            // 3) Generate and bind a texture
            GLuint textureID = 0;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            // 4) Upload the data to OpenGL
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                         GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // 5) Set texture parameters (filtering, wrapping, etc.)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // 6) Unbind and free the stb_image data
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(data);

            // Return the texture ID, but cast it to `void*`
            // so it fits our `GenericAsset.data` usage.
            return reinterpret_cast<void*>(static_cast<uintptr_t>(textureID));
        }

        case AssetType::SHADER:
        {
            std::cout << "[AssetManager] Loading SHADER from " << path << std::endl;
            // Stub: in real code, read/compile .vert/.frag, return program ID
            return (void*)0x87654321;
        }

        case AssetType::SOUND:
        {
            std::cout << "[AssetManager] Loading SOUND from " << path << std::endl;
            // Stub: load .wav or .ogg file, return pointer to sound data
            return (void*)0xAAAA8888;
        }

        default:
        {
            std::cerr << "[AssetManager] Unknown asset type!\n";
            return nullptr;
        }
    }
}
