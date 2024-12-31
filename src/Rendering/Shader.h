// Shader.h
#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h> // or appropriate OpenGL headers
#include <glm/glm.hpp> // For glm::mat4

class Shader
{
public:
    GLuint ID;

    // Constructors
    Shader();
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader(); // Destructor to clean up shader program

    // Use/activate the shader
    void Use();

    // Utility functions to set uniforms
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetBool(const std::string &name, bool value) const;
    void SetMat4(const std::string &name, const glm::mat4 &mat) const; // For setting 4x4 matrices

private:
    // Caching uniform locations for performance
    mutable std::unordered_map<std::string, GLint> uniformLocationCache;

    // Retrieves the location of a uniform variable, with caching
    GLint GetUniformLocation(const std::string &name) const;
};
