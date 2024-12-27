#include "Engine/AssetManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <GL/glew.h>
#include <vector>

#include <filesystem>
#include <variant>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "imgui.h"
#include "gcml.h"

#include "Windows/LoggerWindow.h"
#include "Rendering/Shader.h"

GLuint LoadTextureFromList(const std::string &path);
Shader *LoadShaderFromList(const std::string &path);
Model *LoadModelFromList(const std::string &path);

int LoadedAssets = 0;

extern LoggerWindow *g_LoggerWindow;

std::string getDirectoryPath(const std::string &fullPath)
{
    std::filesystem::path pathObj(fullPath);
    std::filesystem::path dir = pathObj.parent_path();
    return dir.string();
}

void AssetManager::DebugAssetMap()
{
    std::cout << "[AssetManager] Debugging m_AssetMap:" << std::endl;
    int i = 0;
    for (const auto &[key, value] : m_AssetMap)
    {
        std::cout << "  Key: " << key << ", Type Index: " << value.index() << std::endl;
        i++;
    }
    if (i == 0)
    {
        DEBUG_PRINT("No Cashed Assets");
    }
}

// Implementation of AssetManager::loadAssetFromDisk
AssetManager::AssetVariant AssetManager::loadAssetFromDisk(AssetType type, const std::string &path)
{
    switch (type)
    {
    case AssetType::TEXTURE:
        return LoadTextureFromList(path); // Returns GLuint
    case AssetType::SHADER:
        return LoadShaderFromList(path); // Returns Shader*
    case AssetType::SOUND:
        return std::string("Loaded sound: " + path); // Example placeholder for sound
    case AssetType::MODEL:
        return LoadModelFromList(path); // Returns Model*
    default:
        throw std::invalid_argument("Unknown AssetType");
    }
}

std::string AssetManager::generateKey(AssetType type, const std::string &path)
{
    return std::to_string(static_cast<int>(type)) + ":" + path;
}

GLuint LoadTextureFromList(const std::string &path)
{
    // --------------------------------------------
    // Load a texture with stb_image
    // --------------------------------------------
    std::cout << "[AssetManager] Loading TEXTURE from: " << path << std::endl;

    int width, height, channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        std::cerr << "[AssetManager] stb_image failed for: " << path << std::endl;
        return 0;
    }

    GLenum format = GL_RGBA;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
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
    return texID;
}

Shader *LoadShaderFromList(const std::string &path)
{
    // --------------------------------------------
    // Load a shader using your existing "Shader" class
    // --------------------------------------------
    // Example usage: path = "shaders/UnlitMaterial" =>
    //   loads "shaders/UnlitMaterial.vert" and "shaders/UnlitMaterial.frag"
    std::cout << "[AssetManager] Loading SHADER from: " << path << std::endl;

    // Create a new Shader object on the heap
    Shader *newShader = new Shader();

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
    return newShader;
}

// case AssetType::SOUND:
//{
//     std::cout << "[AssetManager] Loading SOUND from: " << path << std::endl;
//     // Stub or real code to load .wav / .ogg
//     return (void *)0xAAAA8888; // placeholder
// }

Model *LoadModelFromList(const std::string &path)
{
    // --------------------------------------------
    // Load an OBJ model
    // --------------------------------------------
    std::cout << "[AssetManager] Loading MODEL from: " << path << std::endl;

    std::ifstream objFile(path);
    if (!objFile.is_open())
    {
        DEBUG_PRINT("[AssetManager] Failed to open OBJ file: %s\n", path.c_str());
        return nullptr;
    }

    std::vector<float> temp_positions;
    std::vector<float> temp_texCoords;
    std::vector<float> temp_normals;
    std::vector<unsigned int> vertexIndices, texCoordIndices, normalIndices;

    std::string directory;
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos)
        directory = path.substr(0, lastSlash + 1);
    else
        directory = "";

    DEBUG_PRINT("[AssetManager] Asset Directory: %s", directory.c_str());

    std::string line;
    std::string mtlFileName;
    while (std::getline(objFile, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            temp_positions.push_back(x);
            temp_positions.push_back(y);
            temp_positions.push_back(z);
        }
        else if (prefix == "vt")
        {
            float u, v;
            iss >> u >> v;
            temp_texCoords.push_back(u);
            temp_texCoords.push_back(v);
        }
        else if (prefix == "vn")
        {
            float nx, ny, nz;
            iss >> nx >> ny >> nz;
            temp_normals.push_back(nx);
            temp_normals.push_back(ny);
            temp_normals.push_back(nz);
        }
        else if (prefix == "f")
        {
            std::string vertexStr;
            std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> faceVertices;
            while (iss >> vertexStr)
            {
                unsigned int vIdx = 0, tIdx = 0, nIdx = 0;
                size_t firstSlash = vertexStr.find('/');
                size_t secondSlash = vertexStr.find('/', firstSlash + 1);

                if (firstSlash == std::string::npos)
                {
                    // Format: f v1 v2 v3
                    vIdx = std::stoi(vertexStr);
                }
                else if (secondSlash == std::string::npos)
                {
                    // Format: f v1/vt1 v2/vt2 v3/vt3
                    vIdx = std::stoi(vertexStr.substr(0, firstSlash));
                    tIdx = std::stoi(vertexStr.substr(firstSlash + 1));
                }
                else if (secondSlash > firstSlash + 1)
                {
                    // Format: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
                    vIdx = std::stoi(vertexStr.substr(0, firstSlash));
                    tIdx = std::stoi(vertexStr.substr(firstSlash + 1, secondSlash - firstSlash - 1));
                    nIdx = std::stoi(vertexStr.substr(secondSlash + 1));
                }
                else
                {
                    // Format: f v1//vn1 v2//vn2 v3//vn3
                    vIdx = std::stoi(vertexStr.substr(0, firstSlash));
                    nIdx = std::stoi(vertexStr.substr(secondSlash + 1));
                }

                faceVertices.emplace_back(vIdx, tIdx, nIdx);
            }

            // Triangulate if the face has more than 3 vertices (optional)
            for (size_t i = 1; i + 1 < faceVertices.size(); ++i)
            {
                vertexIndices.push_back(std::get<0>(faceVertices[0]));
                texCoordIndices.push_back(std::get<1>(faceVertices[0]));
                normalIndices.push_back(std::get<2>(faceVertices[0]));

                vertexIndices.push_back(std::get<0>(faceVertices[i]));
                texCoordIndices.push_back(std::get<1>(faceVertices[i]));
                normalIndices.push_back(std::get<2>(faceVertices[i]));

                vertexIndices.push_back(std::get<0>(faceVertices[i + 1]));
                texCoordIndices.push_back(std::get<1>(faceVertices[i + 1]));
                normalIndices.push_back(std::get<2>(faceVertices[i + 1]));
            }
        }

        else if (prefix == "mtllib")
        {
            iss >> mtlFileName;
        }
    }

    objFile.close();

    // Load MTL file if specified
    std::string texturePath;
    if (!mtlFileName.empty())
    {
        std::ifstream mtlFile(directory + mtlFileName);
        if (mtlFile.is_open())
        {
            while (std::getline(mtlFile, line))
            {
                std::istringstream mtlIss(line);
                std::string mtlPrefix;
                mtlIss >> mtlPrefix;
                if (mtlPrefix == "map_Kd")
                {
                    mtlIss >> texturePath;
                    break; // Assuming only one texture map for simplicity
                }
            }
            mtlFile.close();
        }
        else
        {
            DEBUG_PRINT("[AssetManager] Failed to open MTL file: %s", mtlFileName.c_str());
        }
    }

    if (texturePath.empty())
    {
        DEBUG_PRINT("[AssetManager] No texture found for OBJ:  %s", path.c_str());
    }
    else
    {
        DEBUG_PRINT("[AssetManager] Texture for OBJ:  %s%s", directory.c_str(), texturePath.c_str());
    }

    // Create Model object
    Model *model = new Model();

    // Populate vertices
    std::unordered_map<std::string, unsigned int> uniqueVertices;
    for (size_t i = 0; i < vertexIndices.size(); ++i)
    {
        std::ostringstream keyStream;
        keyStream << vertexIndices[i] << "/" << texCoordIndices[i] << "/" << normalIndices[i];
        std::string key = keyStream.str();

        if (uniqueVertices.find(key) == uniqueVertices.end())
        {
            Vertex vertex;
            // OBJ indices are 1-based
            vertex.position[0] = temp_positions[(vertexIndices[i] - 1) * 3];
            vertex.position[1] = temp_positions[(vertexIndices[i] - 1) * 3 + 1];
            vertex.position[2] = temp_positions[(vertexIndices[i] - 1) * 3 + 2];

            if (!temp_texCoords.empty())
            {
                vertex.texCoord[0] = temp_texCoords[(texCoordIndices[i] - 1) * 2];
                vertex.texCoord[1] = temp_texCoords[(texCoordIndices[i] - 1) * 2 + 1];
            }
            else
            {
                vertex.texCoord[0] = 0.0f;
                vertex.texCoord[1] = 0.0f;
            }

            if (!temp_normals.empty())
            {
                vertex.normal[0] = temp_normals[(normalIndices[i] - 1) * 3];
                vertex.normal[1] = temp_normals[(normalIndices[i] - 1) * 3 + 1];
                vertex.normal[2] = temp_normals[(normalIndices[i] - 1) * 3 + 2];
            }
            else
            {
                vertex.normal[0] = 0.0f;
                vertex.normal[1] = 0.0f;
                vertex.normal[2] = 0.0f;
            }

            model->vertices.push_back(vertex);
            unsigned int newIndex = static_cast<unsigned int>(model->vertices.size() - 1);
            uniqueVertices[key] = newIndex;
            model->indices.push_back(newIndex);
        }
        else
        {
            model->indices.push_back(uniqueVertices[key]);
        }
    }

    // Generate OpenGL buffers
    glGenVertexArrays(1, &model->vao);
    glGenBuffers(1, &model->vbo);
    glGenBuffers(1, &model->ebo);

    glBindVertexArray(model->vao);

    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, model->vertices.size() * sizeof(Vertex), model->vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->indices.size() * sizeof(unsigned int), model->indices.data(), GL_STATIC_DRAW);

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

    // Load texture if available
    if (!texturePath.empty())
    {
        int width, height, channels;
        unsigned char *data = stbi_load((directory + texturePath).c_str(), &width, &height, &channels, 0);
        if (!data)
        {
            DEBUG_PRINT("[AssetManager] stb_image failed to load texture: %s", (directory + texturePath).c_str());
        }
        else
        {
            GLenum format = GL_RGBA;
            if (channels == 1)
                format = GL_RED;
            else if (channels == 3)
                format = GL_RGB;
            // if channels == 4, already GL_RGBA

            glGenTextures(1, &model->textureID);
            glBindTexture(GL_TEXTURE_2D, model->textureID);

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                         format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // Set texture params
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(data);
        }
    }
    else
    {
        model->textureID = 0; // No texture
    }

    // Return the Model pointer as void*
    return model;
}
