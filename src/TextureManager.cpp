// src/TextureManager.cpp
#include "TextureManager.h"
#include "Logger.h"
#include "stb_image.h"

GLuint TextureManager::LoadTexture(const std::string& path) {
    // Check if texture already loaded
    auto it = textures.find(path);
    if (it != textures.end()) {
        return it->second;
    }

    // Load image
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!data) {
        Logger::GetInstance().Log(LogLevel::ERROR, "Failed to load texture: " + path);
        return 0;
    }

    // Generate OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //glGenerateMipmap(GL_TEXTURE_2D); // Uncomment if using mipmaps

    glBindTexture(GL_TEXTURE_2D, 0);

    // Free image data
    stbi_image_free(data);

    // Store texture
    textures[path] = textureID;

    Logger::GetInstance().Log(LogLevel::INFO, "Loaded texture: " + path);
    return textureID;
}

GLuint TextureManager::GetTexture(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) {
        return it->second;
    }
    return LoadTexture(path);
}

void TextureManager::Cleanup() {
    for (auto& pair : textures) {
        glDeleteTextures(1, &pair.second);
    }
    textures.clear();
}
