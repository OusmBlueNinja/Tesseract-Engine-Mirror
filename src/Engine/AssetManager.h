#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <GL/glew.h>
#include <vector>
#include <variant>
#include "gcml.h"
#include "stdexcept"
#include <iostream>
#include "Rendering/Shader.h"
#include <algorithm>
#include <cmath> // For std::abs
#include <memory>

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

    // Equality operator to compare two Vertex instances
    bool operator==(const Vertex &other) const
    {
        // Compare positions
        for (int i = 0; i < 3; ++i)
        {
            if (position[i] != other.position[i])
                return false;
        }

        // Compare texture coordinates
        for (int i = 0; i < 2; ++i)
        {
            if (texCoord[i] != other.texCoord[i])
                return false;
        }

        // Compare normals
        for (int i = 0; i < 3; ++i)
        {
            if (normal[i] != other.normal[i])
                return false;
        }

        return true;
    }
};

// Define a Texture structure
struct Texture
{
    GLuint id;
    std::string type;
    std::string path;
};

// In AssetManager.h or a separate header file

struct Submesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    GLuint vao = 0, vbo = 0, ebo = 0;

    // Initialize OpenGL buffers for the submesh
    void Initialize()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        // Texture coordinates
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(3 * sizeof(float)));
        // Normals
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(5 * sizeof(float)));

        glBindVertexArray(0);
    }

    // Render the submesh
    void Draw(Shader *shader)
    {
        // Bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;

        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
            // Retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);

            // Now set the sampler to the correct texture unit
            shader->SetInt((name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // Draw mesh
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }
};

// In AssetManager.h or a separate header file

struct Model
{
    std::vector<Submesh> submeshes;

    // Render all submeshes
    void Draw(Shader *shader)
    {
        for (auto &submesh : submeshes)
        {
            submesh.Draw(shader);
        }
    }

    // Cleanup OpenGL resources
    void Cleanup()
    {
        for (auto &submesh : submeshes)
        {
            if (submesh.vao != 0)
                glDeleteVertexArrays(1, &submesh.vao);
            if (submesh.vbo != 0)
                glDeleteBuffers(1, &submesh.vbo);
            if (submesh.ebo != 0)
                glDeleteBuffers(1, &submesh.ebo);
        }
    }
};

// The main AssetManager
class AssetManager
{
public:
    AssetManager() = default;
    ~AssetManager() = default;

    using AssetVariant = std::variant<std::shared_ptr<Model>, std::shared_ptr<Shader>, std::shared_ptr<GLuint>>;

    // Template function to load an asset
    template <typename T>
    std::shared_ptr<T> loadAsset(AssetType type, const std::string &path)
    {
        // 1) Create a unique key for cache lookup
        std::string key = generateKey(type, path);

        // 2) Check if it’s already loaded
        auto it = m_AssetMap.find(key);
        if (it != m_AssetMap.end())
        {
            // Debug: Log the variant type
            if (std::holds_alternative<std::shared_ptr<T>>(it->second))
            {
#ifdef DEBUG
                DebugAssetMap();
#endif
                std::cout << "[AssetManager] Retrieved asset from cache: " << key << std::endl;
                return std::get<std::shared_ptr<T>>(it->second);
            }
            else
            {
                throw std::runtime_error("Asset type mismatch for key: " + key);
            }
        }

        // 3) Not loaded yet
        AssetVariant assetData = loadAssetFromDisk(type, path);

        if (assetData.valueless_by_exception())
        {
            DEBUG_PRINT("[AssetManager] Failed to load asset: %s", path.c_str());
            return nullptr; // For smart pointers, return nullptr on failure
        }

        // 4) Store in cache
        m_AssetMap[key] = assetData;

        // 5) Return the loaded asset
        return std::get<std::shared_ptr<T>>(assetData);
    }

    void DebugAssetMap();

private:
    // Cache of already loaded assets: key = "type + path"
    std::unordered_map<std::string, AssetVariant> m_AssetMap;

    AssetVariant loadAssetFromDisk(AssetType type, const std::string &path);
    // Generate the unique key
    std::string generateKey(AssetType type, const std::string &path);
};
