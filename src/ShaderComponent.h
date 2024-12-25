// src/ShaderComponent.h
#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>



class Shader {
public:
    GLuint ID;

    Shader() : ID(0) {}
    Shader(const char* vertexPath, const char* fragmentPath);
    void Use();
    void SetBool(const std::string &name, bool value) const;  
    void SetInt(const std::string &name, int value) const;   
    void SetFloat(const std::string &name, float value) const;
    void SetMat4(const std::string &name, const glm::mat4 &mat) const;
    void SetVec3(const std::string &name, const glm::vec3 &vec) const;
};
