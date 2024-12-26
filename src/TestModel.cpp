#include "TestModel.h"
#include <GL/glew.h>
#include <cstdio>


GLuint CreateCubeVAO()
{
    // Define cube vertices (Position + UVs)
    static float g_CubeVertices[] =
    {
        // Front face
        -1.f, -1.f,  1.f,  0.f, 0.f,
         1.f, -1.f,  1.f,  1.f, 0.f,
         1.f,  1.f,  1.f,  1.f, 1.f,
        -1.f,  1.f,  1.f,  0.f, 1.f,

        // Back face
        -1.f, -1.f, -1.f,  1.f, 0.f,
         1.f, -1.f, -1.f,  0.f, 0.f,
         1.f,  1.f, -1.f,  0.f, 1.f,
        -1.f,  1.f, -1.f,  1.f, 1.f,

        // Left face
        -1.f, -1.f, -1.f,  0.f, 0.f,
        -1.f, -1.f,  1.f,  1.f, 0.f,
        -1.f,  1.f,  1.f,  1.f, 1.f,
        -1.f,  1.f, -1.f,  0.f, 1.f,

        // Right face
         1.f, -1.f, -1.f,  1.f, 0.f,
         1.f, -1.f,  1.f,  0.f, 0.f,
         1.f,  1.f,  1.f,  0.f, 1.f,
         1.f,  1.f, -1.f,  1.f, 1.f,

        // Top face
        -1.f,  1.f, -1.f,  0.f, 0.f,
         1.f,  1.f, -1.f,  1.f, 0.f,
         1.f,  1.f,  1.f,  1.f, 1.f,
        -1.f,  1.f,  1.f,  0.f, 1.f,

        // Bottom face
        -1.f, -1.f, -1.f,  1.f, 0.f,
         1.f, -1.f, -1.f,  0.f, 0.f,
         1.f, -1.f,  1.f,  0.f, 1.f,
        -1.f, -1.f,  1.f,  1.f, 1.f,
    };

    // Define cube indices
    static unsigned int g_CubeIndices[] =
    {
        // Front face
        0, 1, 2,   2, 3, 0,
        // Back face
        4, 5, 6,   6, 7, 4,
        // Left face
        8, 9, 10,  10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20
    };

    GLuint VAO, VBO, EBO;

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_CubeVertices), g_CubeVertices, GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_CubeIndices), g_CubeIndices, GL_STATIC_DRAW);

    // Define vertex attributes
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV attribute (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO (not EBO!)
    glBindVertexArray(0);

    // Optionally, unbind VBO and EBO for cleanliness
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Debug: Print VAO ID
    printf("[MeshUtils] Initialized CubeVAO with ID: %u\n", VAO);

    return VAO;
}