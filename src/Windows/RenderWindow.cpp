#include "RenderWindow.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"

// Include your AssetManager header

// Assume we have an extern or global asset manager declared somewhere
// e.g. in "Engine.cpp":    AssetManager g_AssetManager;
// and in "AssetManager.h": extern AssetManager g_AssetManager;
extern AssetManager g_AssetManager;

// --------------------------------------------------
// Cube vertex data and indices (unchanged)
// --------------------------------------------------
static float g_CubeVertices[] =
{
    // FRONT (z=+1)
    -1.f, -1.f,  1.f,   0.f, 0.f,
     1.f, -1.f,  1.f,   1.f, 0.f,
     1.f,  1.f,  1.f,   1.f, 1.f,
    -1.f,  1.f,  1.f,   0.f, 1.f,

    // BACK (z=-1)
    -1.f, -1.f, -1.f,   1.f, 0.f,
     1.f, -1.f, -1.f,   0.f, 0.f,
     1.f,  1.f, -1.f,   0.f, 1.f,
    -1.f,  1.f, -1.f,   1.f, 1.f,

    // LEFT (x=-1)
    -1.f, -1.f, -1.f,   0.f, 0.f,
    -1.f, -1.f,  1.f,   1.f, 0.f,
    -1.f,  1.f,  1.f,   1.f, 1.f,
    -1.f,  1.f, -1.f,   0.f, 1.f,

    // RIGHT (x=+1)
     1.f, -1.f, -1.f,   1.f, 0.f,
     1.f, -1.f,  1.f,   0.f, 0.f,
     1.f,  1.f,  1.f,   0.f, 1.f,
     1.f,  1.f, -1.f,   1.f, 1.f,

    // TOP (y=+1)
    -1.f,  1.f, -1.f,   0.f, 0.f,
     1.f,  1.f, -1.f,   1.f, 0.f,
     1.f,  1.f,  1.f,   1.f, 1.f,
    -1.f,  1.f,  1.f,   0.f, 1.f,

    // BOTTOM (y=-1)
    -1.f, -1.f, -1.f,   1.f, 0.f,
     1.f, -1.f, -1.f,   0.f, 0.f,
     1.f, -1.f,  1.f,   0.f, 1.f,
    -1.f, -1.f,  1.f,   1.f, 1.f,
};

static unsigned int g_CubeIndices[] =
{
    // Front
    0,1,2,   2,3,0,
    // Back
    4,5,6,   6,7,4,
    // Left
    8,9,10,  10,11,8,
    // Right
    12,13,14, 14,15,12,
    // Top
    16,17,18, 18,19,16,
    // Bottom
    20,21,22, 22,23,20
};

// --------------------------------------------------
// Show() - ImGui window to display the FBO
// --------------------------------------------------
void RenderWindow::Show()
{
    ImGui::Begin("OpenGL Output");

    ImVec2 size = ImGui::GetContentRegionAvail();
    int w = static_cast<int>(size.x);
    int h = static_cast<int>(size.y);

    if (!m_Initialized)
    {
        InitGLResources();
        m_Initialized = true;
    }

    if (w > 0 && h > 0)
    {
        if (w != m_LastWidth || h != m_LastHeight)
        {
            m_FBO.Create(w, h);
            m_LastWidth  = w;
            m_LastHeight = h;
        }

        RenderSceneToFBO();
        ImGui::Image(m_FBO.GetTextureID(), size, ImVec2(0,0), ImVec2(1,1));
    }
    else
    {
        ImGui::Text("No space to render.");
    }

    ImGui::End();
}

// --------------------------------------------------
// InitGLResources() - Setup VAO/VBO/EBO and load texture
// --------------------------------------------------
void RenderWindow::InitGLResources()
{
    // 1) Load & compile our unlit texture shader (with UV support)
    if (!m_Shader.Load("shaders/UnlitMaterial.vert", "shaders/UnlitMaterial.frag"))
    {
        fprintf(stderr, "[RenderWindow] Failed to load unlit material shader.\n");
        return;
    }

    // 2) Create a VAO for the cube
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // 3) Create a VBO
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_CubeVertices), g_CubeVertices, GL_STATIC_DRAW);

    // 4) Create an EBO
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_CubeIndices), g_CubeIndices, GL_STATIC_DRAW);

    // 5) Setup vertex attribs
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 6) Load the texture via AssetManager
    //    This call returns a void*, which we cast to a GLuint
    {
        void* assetPtr = g_AssetManager.loadAsset(AssetType::TEXTURE, "assets/textures/default.png");
        if (!assetPtr)
        {
            fprintf(stderr, "[RenderWindow] Failed to load texture via AssetManager.\n");
        }
        else
        {
            // Convert void* to GLuint
            m_TextureID = static_cast<GLuint>(reinterpret_cast<uintptr_t>(assetPtr));
        }
    }
}

// --------------------------------------------------
// RenderSceneToFBO() - Offscreen render of the spinning cube
// --------------------------------------------------
void RenderWindow::RenderSceneToFBO()
{
    m_RotationAngle += 0.5f; // degrees per frame

    m_FBO.Bind();
    glViewport(0, 0, m_LastWidth, m_LastHeight);

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.1f, 0.15f, 0.2f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our unlit texture shader
    m_Shader.Use();

    // Build MVP
    glm::mat4 model = glm::rotate(glm::mat4(1.0f),
                                  glm::radians(m_RotationAngle),
                                  glm::vec3(1.f, 1.f, 0.f));

    glm::mat4 view  = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, -5.f));
    float aspect     = (m_LastHeight != 0) ? (float)m_LastWidth / (float)m_LastHeight : 1.0f;
    glm::mat4 proj   = glm::perspective(glm::radians(45.f), aspect, 0.1f, 100.f);
    glm::mat4 mvp    = proj * view * model;

    // Pass MVP
    GLint mvpLoc = glGetUniformLocation(m_Shader.GetProgramID(), "uMVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    // Texture uniform -> unit 0
    GLint texLoc = glGetUniformLocation(m_Shader.GetProgramID(), "uTexture");
    glUniform1i(texLoc, 0);

    // Bind the texture from asset manager
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // Draw
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    FBO::Unbind();
}
