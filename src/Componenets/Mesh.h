
// Mesh.h
#pragma once
#include <GL/glew.h>

// A simple mesh storing a VAO, index count, and texture ID
struct Mesh
{
    GLuint vao = 0;           // Vertex Array Object
    GLuint indexCount = 0;    // Number of indices to draw
    GLuint textureID = 0;     // The texture handle
};