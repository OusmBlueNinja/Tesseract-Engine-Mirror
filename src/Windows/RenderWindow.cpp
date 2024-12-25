#include "RenderWindow.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp> // for perspective, translate, rotate
#include <glm/gtc/type_ptr.hpp>         // for value_ptr
#include "imgui.h"

// A simple data structure for a colored cube
// position (x,y,z), color(r,g,b)
static float g_CubeVertices[] = {
    //  x,     y,     z,     r, g, b
    // Front face
    -1.f, -1.f,  1.f,   1.f, 0.f, 0.f,  // bottom-left
     1.f, -1.f,  1.f,   1.f, 0.f, 0.f,  // bottom-right
     1.f,  1.f,  1.f,   1.f, 0.f, 0.f,  // top-right
    -1.f,  1.f,  1.f,   1.f, 0.f, 0.f,  // top-left

    // Back face
    -1.f, -1.f, -1.f,   0.f, 1.f, 0.f,
     1.f, -1.f, -1.f,   0.f, 1.f, 0.f,
     1.f,  1.f, -1.f,   0.f, 1.f, 0.f,
    -1.f,  1.f, -1.f,   0.f, 1.f, 0.f,
};

static unsigned int g_CubeIndices[] = {
    // Front face
    0, 1, 2,
    2, 3, 0,

    // Back face
    5, 4, 7,
    7, 6, 5,

    // Left face
    4, 0, 3,
    3, 7, 4,

    // Right face
    1, 5, 6,
    6, 2, 1,

    // Top face
    3, 2, 6,
    6, 7, 3,

    // Bottom face
    4, 5, 1,
    1, 0, 4
};

void RenderWindow::Show()
{
    ImGui::Begin("OpenGL Output");

    ImVec2 size = ImGui::GetContentRegionAvail();
    int w = (int)size.x;
    int h = (int)size.y;

    // Lazy init, so we only do it once
    if (!m_Initialized)
    {
        InitGLResources();
        m_Initialized = true;
    }

    if (w > 0 && h > 0)
    {
        // Re-create FBO if size changed
        if (w != m_LastWidth || h != m_LastHeight)
        {
            m_FBO.Create(w, h);
            m_LastWidth  = w;
            m_LastHeight = h;
        }

        // Render our scene to the FBO
        RenderSceneToFBO();

        // Display the FBO texture in ImGui
        ImGui::Image(m_FBO.GetTextureID(), size, ImVec2(0,0), ImVec2(1,1));
    }
    else
    {
        ImGui::Text("No space to render.");
    }

    ImGui::End();
}

void RenderWindow::InitGLResources()
{
    // 1) Load and compile our unlit shader
    //    Adjust paths if needed. Ex: "shaders/UnlitMaterial.vert"
    if (!m_Shader.Load("shaders/UnlitMaterial.vert", "shaders/UnlitMaterial.frag"))
    {
        // Fail gracefully or throw
        fprintf(stderr, "[RenderWindow] Failed to load UnlitMaterial shader.\n");
        return;
    }

    // 2) Create VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // 3) Create VBO
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_CubeVertices), g_CubeVertices, GL_STATIC_DRAW);

    // 4) Create EBO
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_CubeIndices), g_CubeIndices, GL_STATIC_DRAW);

    // 5) Setup vertex attribs:
    //    layout(location = 0) => aPos (3 floats)
    //    layout(location = 1) => aColor (3 floats)
    // Stride: 6 floats total
    // Positions start at offset 0
    // Color starts at offset 3 * sizeof(float)

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO (optional, but good practice)
    glBindVertexArray(0);
}

void RenderWindow::RenderSceneToFBO()
{
    // Spin
    m_RotationAngle += 0.5f;

    m_FBO.Bind();
    glViewport(0, 0, m_LastWidth, m_LastHeight);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.15f, 0.2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our unlit shader
    m_Shader.Use();

    // Build a MVP matrix with GLM (model * view * proj)
    // 1) Model: rotate around Y or diagonal axis
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(m_RotationAngle), glm::vec3(1.0f, 1.0f, 0.0f));

    // 2) View: move camera back by 5 on Z
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, -5.f));

    // 3) Projection: perspective
    float aspect = (float)m_LastWidth / (float)m_LastHeight;
    glm::mat4 projection = glm::perspective(glm::radians(45.f), aspect, 0.1f, 100.f);

    glm::mat4 mvp = projection * view * model;

    // Upload MVP to the shader
    GLint mvpLoc = glGetUniformLocation(m_Shader.GetProgramID(), "uMVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

    // Draw the cube with our VAO
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    FBO::Unbind();
}
