#pragma once
#include <string>
#include <GL/glew.h>

class Shader
{
public:
    Shader() = default;
    ~Shader();

    // Load & compile from files (vertex & fragment)
    bool Load(const std::string& vertexPath, const std::string& fragmentPath);

    void Use() const { glUseProgram(m_ProgramID); }

    // Uniform helper
    GLuint GetProgramID() const { return m_ProgramID; }

private:
    bool CompileShader(GLuint shaderID, const std::string& source);
    std::string LoadSourceFromFile(const std::string& filePath);

private:
    GLuint m_ProgramID = 0;
};
