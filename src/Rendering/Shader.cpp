#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::~Shader()
{
    if (m_ProgramID)
        glDeleteProgram(m_ProgramID);
}

bool Shader::Load(const std::string& vertexPath, const std::string& fragmentPath)
{
    // 1) Create shader objects
    GLuint vertexShader   = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // 2) Load sources
    std::string vertSource = LoadSourceFromFile(vertexPath);
    std::string fragSource = LoadSourceFromFile(fragmentPath);
    if (vertSource.empty() || fragSource.empty())
    {
        std::cerr << "[Shader] Failed to read shader files." << std::endl;
        return false;
    }

    // 3) Compile vertex shader
    {
        const char* src = vertSource.c_str();
        glShaderSource(vertexShader, 1, &src, nullptr);
        glCompileShader(vertexShader);
        if (!CompileShader(vertexShader, vertexPath))
            return false;
    }

    // 4) Compile fragment shader
    {
        const char* src = fragSource.c_str();
        glShaderSource(fragmentShader, 1, &src, nullptr);
        glCompileShader(fragmentShader);
        if (!CompileShader(fragmentShader, fragmentPath))
            return false;
    }

    // 5) Create program and link
    m_ProgramID = glCreateProgram();
    glAttachShader(m_ProgramID, vertexShader);
    glAttachShader(m_ProgramID, fragmentShader);
    glLinkProgram(m_ProgramID);

    // Check link status
    GLint success;
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(m_ProgramID, 1024, nullptr, infoLog);
        std::cerr << "[Shader] Program linking failed:\n" << infoLog << std::endl;
        return false;
    }

    // Cleanup shader objects after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

bool Shader::CompileShader(GLuint shaderID, const std::string& filePath)
{
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shaderID, 1024, nullptr, infoLog);
        std::cerr << "[Shader] Compilation error in " << filePath << ":\n" 
                  << infoLog << std::endl;
        return false;
    }
    return true;
}

std::string Shader::LoadSourceFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) 
    {
        std::cerr << "[Shader] Could not open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
