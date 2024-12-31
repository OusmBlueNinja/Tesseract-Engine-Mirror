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

AssetManager::AssetVariant AssetManager::loadAssetFromDisk(AssetType type, const std::string &path)
{
    // DebugAssetMap();
    g_LoggerWindow->AddLog("[AssetManager] Loading asset: %s", path.c_str());
    LoadedAssets = m_AssetMap.size();
    switch (type)
    {
    case AssetType::TEXTURE:
    {
        GLuint textureID = LoadTextureFromList(path); // Returns GLuint
        return std::make_shared<GLuint>(textureID);   // Wrap in shared_ptr
    }
    case AssetType::SHADER:
    {
        Shader* shaderPtr = LoadShaderFromList(path); // Returns Shader*
        if (shaderPtr != nullptr)
        {
            // It's essential to ensure that shaderPtr is dynamically allocated and not managed elsewhere
            return std::shared_ptr<Shader>(shaderPtr);
        }
        else
        {
            throw std::runtime_error("Failed to load shader: " + path);
        }
    }
    case AssetType::MODEL:
    {
        Model* modelPtr = LoadModelFromList(path); // Returns Model*
        if (modelPtr != nullptr)
        {
            // It's essential to ensure that modelPtr is dynamically allocated and not managed elsewhere
            return std::shared_ptr<Model>(modelPtr);
        }
        else
        {
            throw std::runtime_error("Failed to load model: " + path);
        }
    }
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

    int width, height, channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
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

Shader* LoadShaderFromList(const std::string &path)
{

    // Build actual paths from the base path
    std::string vertPath = path + ".vert";
    std::string fragPath = path + ".frag";

    // Create a new Shader object using the constructor that takes vertex and fragment paths
    Shader *newShader = new Shader(vertPath.c_str(), fragPath.c_str());

    // Check if shader compiled and linked successfully
    if (newShader->ID == 0)
    {
        delete newShader; // Cleanup
        return nullptr;
    }

    // Return the Shader pointer as void*
    return newShader;
}







GLuint LoadTexture(const std::string &path, const std::string &directory)
{
    std::string fullPath = directory + path;
    int width, height, channels;
    unsigned char *data = stbi_load(fullPath.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        DEBUG_PRINT("[AssetManager] failed to load texture: %s: %s", fullPath.c_str(),stbi_failure_reason());
        return 0;
    }

    GLenum format;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;
    else
        format = GL_RGB; // Default fallback

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                 format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    return textureID;
}








Model* LoadModelFromList(const std::string &path)
{

    std::ifstream objFile(path);
    if (!objFile.is_open())
    {
        return nullptr;
    }

    std::vector<float> temp_positions;
    std::vector<float> temp_texCoords;
    std::vector<float> temp_normals;

    std::string directory;
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash != std::string::npos)
        directory = path.substr(0, lastSlash + 1);
    else
        directory = "";

    
    std::string currentMaterial = "default";

    // Map material name to Submesh
    std::unordered_map<std::string, Submesh> materialToSubmesh;
    materialToSubmesh[currentMaterial] = Submesh();

    std::string line;
    std::string mtlFileName;
    while (std::getline(objFile, line))
    {
        if (line.empty() || line[0] == '#')
            continue; // Skip empty lines and comments

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            // Flip the model vertically by inverting the y-axis
            temp_positions.push_back(x);
            temp_positions.push_back(-y); // Inverted
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
            // Invert the y-axis for normals as well
            temp_normals.push_back(nx);
            temp_normals.push_back(-ny); // Inverted
            temp_normals.push_back(nz);
        }
        else if (prefix == "usemtl")
        {
            iss >> currentMaterial;
            if (materialToSubmesh.find(currentMaterial) == materialToSubmesh.end())
            {
                materialToSubmesh[currentMaterial] = Submesh();
            }
        }
        else if (prefix == "mtllib")
        {
            iss >> mtlFileName;
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

            // Triangulate if the face has more than 3 vertices
            for (size_t i = 1; i + 1 < faceVertices.size(); ++i)
            {
                // Current material's submesh
                Submesh &currentSubmesh = materialToSubmesh[currentMaterial];

                auto addVertex = [&](unsigned int v, unsigned int t, unsigned int n) -> unsigned int {
                    Vertex vertex;
                    // OBJ indices are 1-based
                    vertex.position[0] = temp_positions[(v - 1) * 3];
                    vertex.position[1] = temp_positions[(v - 1) * 3 + 1];
                    vertex.position[2] = temp_positions[(v - 1) * 3 + 2];

                    if (!temp_texCoords.empty() && t > 0)
                    {
                        vertex.texCoord[0] = temp_texCoords[(t - 1) * 2];
                        vertex.texCoord[1] = temp_texCoords[(t - 1) * 2 + 1];
                    }
                    else
                    {
                        vertex.texCoord[0] = 0.0f;
                        vertex.texCoord[1] = 0.0f;
                    }

                    if (!temp_normals.empty() && n > 0)
                    {
                        vertex.normal[0] = temp_normals[(n - 1) * 3];
                        vertex.normal[1] = temp_normals[(n - 1) * 3 + 1];
                        vertex.normal[2] = temp_normals[(n - 1) * 3 + 2];
                    }
                    else
                    {
                        vertex.normal[0] = 0.0f;
                        vertex.normal[1] = 0.0f;
                        vertex.normal[2] = 0.0f;
                    }

                    // Check if the vertex already exists in the submesh
                    auto it = std::find(currentSubmesh.vertices.begin(), currentSubmesh.vertices.end(), vertex);
                    if (it != currentSubmesh.vertices.end())
                    {
                        return static_cast<unsigned int>(std::distance(currentSubmesh.vertices.begin(), it));
                    }
                    else
                    {
                        currentSubmesh.vertices.push_back(vertex);
                        return static_cast<unsigned int>(currentSubmesh.vertices.size() - 1);
                    }
                };

                unsigned int idx0 = addVertex(std::get<0>(faceVertices[0]), std::get<1>(faceVertices[0]), std::get<2>(faceVertices[0]));
                unsigned int idx1 = addVertex(std::get<0>(faceVertices[i]), std::get<1>(faceVertices[i]), std::get<2>(faceVertices[i]));
                unsigned int idx2 = addVertex(std::get<0>(faceVertices[i + 1]), std::get<1>(faceVertices[i + 1]), std::get<2>(faceVertices[i + 1]));

                currentSubmesh.indices.push_back(idx0);
                currentSubmesh.indices.push_back(idx1);
                currentSubmesh.indices.push_back(idx2);
            }
        }
    }

    objFile.close();

    // Load MTL file if specified
    std::unordered_map<std::string, std::vector<Texture>> materialTexturesMap;
    if (!mtlFileName.empty())
    {
        std::ifstream mtlFile(directory + mtlFileName);
        if (mtlFile.is_open())
        {
            std::string mtlLine;
            std::string currentMaterialName;
            while (std::getline(mtlFile, mtlLine))
            {
                if (mtlLine.empty() || mtlLine[0] == '#')
                    continue; // Skip comments and empty lines

                std::istringstream mtlIss(mtlLine);
                std::string mtlPrefix;
                mtlIss >> mtlPrefix;

                if (mtlPrefix == "newmtl")
                {
                    mtlIss >> currentMaterialName;
                }
                else if (mtlPrefix == "map_Kd")
                {
                    std::string texturePath;
                    mtlIss >> texturePath;
                    if (!texturePath.empty())
                    {
                        GLuint texID = LoadTexture(texturePath, directory);
                        if (texID != 0)
                        {
                            Texture texture;
                            texture.id = texID;
                            texture.type = "texture_diffuse";
                            texture.path = texturePath;
                            materialTexturesMap[currentMaterialName].push_back(texture);
                        }
                    }
                }
                else if (mtlPrefix == "map_Ks")
                {
                    std::string texturePath;
                    mtlIss >> texturePath;
                    if (!texturePath.empty())
                    {
                        GLuint texID = LoadTexture(texturePath, directory);
                        if (texID != 0)
                        {
                            Texture texture;
                            texture.id = texID;
                            texture.type = "texture_specular";
                            texture.path = texturePath;
                            materialTexturesMap[currentMaterialName].push_back(texture);
                        }
                    }
                }
                else if (mtlPrefix == "map_Bump" || mtlPrefix == "map_bump" || mtlPrefix == "bump")
                {
                    std::string texturePath;
                    mtlIss >> texturePath;
                    if (!texturePath.empty())
                    {
                        GLuint texID = LoadTexture(texturePath, directory);
                        if (texID != 0)
                        {
                            Texture texture;
                            texture.id = texID;
                            texture.type = "texture_normal";
                            texture.path = texturePath;
                            materialTexturesMap[currentMaterialName].push_back(texture);
                        }
                    }
                }
                // Add more texture types as needed
            }

            mtlFile.close();
        }
        else
        {
        }
    }
    else
    {
    }

    // Assign textures to submeshes based on their material
    for (auto &pair : materialToSubmesh)
    {
        const std::string &materialName = pair.first;
        Submesh &submesh = pair.second;

        if (materialTexturesMap.find(materialName) != materialTexturesMap.end())
        {
            submesh.textures = materialTexturesMap[materialName];
        }
        else
        {
            // If no material textures found, you can assign default textures or leave it empty
        }

        // Initialize OpenGL buffers for the submesh
        submesh.Initialize();
    }

    if (materialToSubmesh.empty())
    {
        return nullptr;
    }

    // Create Model object
    Model *model = new Model();

    // Move submeshes to the model
    for (auto &pair : materialToSubmesh)
    {
        model->submeshes.emplace_back(std::move(pair.second));
    }

    DEBUG_PRINT("[AssetManager] Loaded model with %lld submeshes.", model->submeshes.size());


    return model;
}
