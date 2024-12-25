// src/TextureManager.h
#pragma once

#include <string>
#include <unordered_map>
#include <GLFW/glfw3.h> // Ensure GLAD is included before GLFW in the main.cpp

class TextureManager {
public:
    GLuint LoadTexture(const std::string& path);
    GLuint GetTexture(const std::string& path);
    void Cleanup();

private:
    std::unordered_map<std::string, GLuint> textures;
};
