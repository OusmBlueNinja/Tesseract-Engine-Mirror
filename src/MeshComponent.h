// src/MeshComponent.h
#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct MeshComponent {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;

    MeshComponent() : VAO(0), VBO(0), EBO(0) {}
};
