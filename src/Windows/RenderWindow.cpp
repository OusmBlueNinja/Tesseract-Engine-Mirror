// RenderWindow.cpp

#include "RenderWindow.h"
#include <vector> // Add this line

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"

#include "gcml.h"

#include "Componenets/GameObject.h"
#include "Componenets/mesh.h"
#include "Componenets/transform.h"

extern std::vector<std::shared_ptr<GameObject>> g_GameObjects;

#define CAM_FOV 45.0f
#define CAM_NEAR_PLAIN 0.1f
#define CAM_FAR_PLAIN 1000.0f

// Include your AssetManager & Shader headers
#include "Engine/AssetManager.h"
#include "Rendering/Shader.h"

// Extern reference to our global (or extern) asset manager
extern AssetManager g_AssetManager;


extern std::shared_ptr<CameraComponent> g_RuntimeCameraObject;



extern int g_GPU_Triangles_drawn_to_screen;

// Example cube data (position + UVs)
static float g_CubeVertices[] =
    {
        // FRONT (z=+1)
        -1.f,
        -1.f,
        1.f,
        0.f,
        0.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        0.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        0.f,
        1.f,

        // BACK (z=-1)
        -1.f,
        -1.f,
        -1.f,
        1.f,
        0.f,
        1.f,
        -1.f,
        -1.f,
        0.f,
        0.f,
        1.f,
        1.f,
        -1.f,
        0.f,
        1.f,
        -1.f,
        1.f,
        -1.f,
        1.f,
        1.f,

        // LEFT (x=-1)
        -1.f,
        -1.f,
        -1.f,
        0.f,
        0.f,
        -1.f,
        -1.f,
        1.f,
        1.f,
        0.f,
        -1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        -1.f,
        0.f,
        1.f,

        // RIGHT (x=+1)
        1.f,
        -1.f,
        -1.f,
        1.f,
        0.f,
        1.f,
        -1.f,
        1.f,
        0.f,
        0.f,
        1.f,
        1.f,
        1.f,
        0.f,
        1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        1.f,

        // TOP (y=+1)
        -1.f,
        1.f,
        -1.f,
        0.f,
        0.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        0.f,
        1.f,
        1.f,
        1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        0.f,
        1.f,

        // BOTTOM (y=-1)
        -1.f,
        -1.f,
        -1.f,
        1.f,
        0.f,
        1.f,
        -1.f,
        -1.f,
        0.f,
        0.f,
        1.f,
        -1.f,
        1.f,
        0.f,
        1.f,
        -1.f,
        -1.f,
        1.f,
        1.f,
        1.f,
};

static unsigned int g_CubeIndices[] =
    {
        // Front
        0, 1, 2, 2, 3, 0,
        // Back
        4, 5, 6, 6, 7, 4,
        // Left
        8, 9, 10, 10, 11, 8,
        // Right
        12, 13, 14, 14, 15, 12,
        // Top
        16, 17, 18, 18, 19, 16,
        // Bottom
        20, 21, 22, 22, 23, 20};




bool PlayPauseButton(const char* label, bool* isPlaying)
{
    // Define button size
    ImVec2 buttonSize = ImVec2(50, 50); // Adjust size as needed

    // Begin the button
    if (ImGui::Button(label, buttonSize))
    {
        // Toggle the state
        *isPlaying = !(*isPlaying);
        return true; // Indicate that the state was toggled
    }

    // Add tooltip
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(*isPlaying ? "Pause (Space)" : "Play (Space)");
    }

    // Get the current window's draw list
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Get the position of the button
    ImVec2 button_pos = ImGui::GetItemRectMin();
    ImVec2 button_size = ImGui::GetItemRectSize();
    ImVec2 center = ImVec2(button_pos.x + button_size.x * 0.5f, button_pos.y + button_size.y * 0.5f);

    // Define icon size
    float icon_size = 20.0f;
    float half_icon_size = icon_size / 2.0f;

    // Define colors
    ImU32 icon_color = ImGui::GetColorU32(ImGuiCol_Text);

    if (*isPlaying)
    {
        // Draw Pause Icon (two vertical bars)
        float bar_width = 4.0f;
        float spacing = 6.0f;

        // Left bar
        ImVec2 left_bar_p1 = ImVec2(center.x - spacing - bar_width, center.y - half_icon_size);
        ImVec2 left_bar_p2 = ImVec2(center.x - spacing, center.y + half_icon_size);
        draw_list->AddRectFilled(left_bar_p1, left_bar_p2, icon_color, 2.0f);

        // Right bar
        ImVec2 right_bar_p1 = ImVec2(center.x + spacing, center.y - half_icon_size);
        ImVec2 right_bar_p2 = ImVec2(center.x + spacing + bar_width, center.y + half_icon_size);
        draw_list->AddRectFilled(right_bar_p1, right_bar_p2, icon_color, 2.0f);
    }
    else
    {
        // Draw Play Icon (triangle)
        ImVec2 p1 = ImVec2(center.x - half_icon_size, center.y - half_icon_size);
        ImVec2 p2 = ImVec2(center.x - half_icon_size, center.y + half_icon_size);
        ImVec2 p3 = ImVec2(center.x + half_icon_size, center.y);
        draw_list->AddTriangleFilled(p1, p2, p3, icon_color);
    }

    return false; // No toggle occurred
}



void RenderWindow::Show(bool *GameRunning)
{

    ImGui::Begin("Editor##EditorWindow");

    ImVec2 size = ImGui::GetContentRegionAvail();
    int w = static_cast<int>(size.x);
    int h = static_cast<int>(size.y);

    if (!m_Initialized)
    {
        InitGLResources();
        m_Initialized = true;
    }

    // Center the button
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 60) * 0.5f);

    // Render the Play/Pause button
    // Render the Play/Pause button
    PlayPauseButton("##PlayPauseButton", GameRunning);
        


    // If there's space, render to the FBO, then show it as an ImGui image

    if (w > 0 && h > 0)
    {
        if (w != m_LastWidth || h != m_LastHeight)
        {

            m_FBO.Create(w, h);
            m_LastWidth = w;
            m_LastHeight = h;
        }

        RenderSceneToFBO(GameRunning);

        ImGui::Image(m_FBO.GetTextureID(), size, ImVec2(0, 0), ImVec2(1, 1));
    }
    else
    {
        ImGui::Text("No space to render.");
    }

    

    ImGui::End();
}

void RenderWindow::InitGLResources()
{
    // ----------------------------------------------------
    // 1) Load SHADER from the asset manager
    // ----------------------------------------------------

    {
        Shader *shaderAsset = g_AssetManager.loadAsset<Shader *>(AssetType::SHADER, "assets/shaders/UnlitMaterial");
        if (!shaderAsset)
        {
            fprintf(stderr, "[RenderWindow] Failed to load shader via AssetManager.\n");
            return;
        }
        // Cast back to your Shader class
        m_ShaderPtr = static_cast<Shader *>(shaderAsset);
    }

    // ----------------------------------------------------
    // 2) Create VAO/VBO/EBO for the cube
    // ----------------------------------------------------
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_CubeVertices), g_CubeVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_CubeIndices), g_CubeIndices, GL_STATIC_DRAW);

    // Position = location 0, UV = location 1
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // ----------------------------------------------------
    // 3) Load TEXTURE from the asset manager
    // ----------------------------------------------------
    {
        GLuint texAsset = g_AssetManager.loadAsset<GLuint>(AssetType::TEXTURE, "assets/textures/wood.png");
        if (!texAsset)
        {
            fprintf(stderr, "[RenderWindow] Failed to load texture.\n");
        }
        else
        {
            // Cast from void* to GLuint
            m_TextureID = texAsset;
        }
    }

    // ----------------------------------------------------
    // 4) Initialize GameObjects
    // ----------------------------------------------------
}


void CheckOpenGLError(const std::string &location)
{
    GLenum err;
    bool hasError = false;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "[OpenGL Error] (" << err << ") at " << location << std::endl;
        hasError = true;
    }
    if (hasError)
    {
        // Optionally, you can throw an exception or handle the error as needed
    }
}




void RenderWindow::RenderSceneToFBO(bool *GameRunning)
{
    m_RotationAngle += 0.001f; // Spin per frame

    // Bind the FBO
    m_FBO.Bind();
    glViewport(0, 0, m_LastWidth, m_LastHeight);
    CheckOpenGLError("After glViewport");

    glEnable(GL_DEPTH_TEST);
    CheckOpenGLError("After glEnable(GL_DEPTH_TEST)");

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CheckOpenGLError("After glClear");

    // Use our loaded shader
    if (!m_ShaderPtr)
    {
        std::cerr << "[RenderWindow] Shader pointer is null. Cannot render." << std::endl;
        m_FBO.Unbind();
        return; // Can't render without a shader
    }

    m_ShaderPtr->Use();
    GLuint programID = m_ShaderPtr->GetProgramID();
    CheckOpenGLError("After shader use");

    // Define view and projection matrices once
    std::shared_ptr<CameraComponent> activeCamera = nullptr;

    glm::mat4 view;
    glm::mat4 proj;

    if (*GameRunning && g_RuntimeCameraObject)
    {
        activeCamera = g_RuntimeCameraObject;
    }

    // Ensure that an active camera is available
    if (activeCamera)
    {
        // Obtain view and projection matrices from the active camera
        view = activeCamera->GetViewMatrix();
        proj = activeCamera->GetProjectionMatrix();
    }
    else
    {
        // Fallback to default view and projection if no camera is available
        view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -5.f));
        float aspect = (m_LastHeight != 0) ? (float)m_LastWidth / (float)m_LastHeight : 1.0f;
        proj = glm::perspective(glm::radians(CAM_FOV), aspect, CAM_NEAR_PLAIN, CAM_FAR_PLAIN);
    }

    // Iterate over each GameObject and render it
    for (auto &obj : g_GameObjects)
    {
        glm::mat4 model = glm::mat4(1.f);

        std::shared_ptr<TransformComponent> transform = obj->GetComponent<TransformComponent>();
        std::shared_ptr<MeshComponent> mesh = obj->GetComponent<MeshComponent>();

        if (transform && mesh)
        {
            // Validate VAO
            if (mesh->vao == 0)
            {
                std::cerr << "[RenderWindow] Warning: Mesh VAO is not initialized." << std::endl;
                continue;
            }

            // Update triangle count
            g_GPU_Triangles_drawn_to_screen += static_cast<int>(mesh->indexCount);

            // Apply transformations
            model = glm::translate(model, transform->position);
            model = glm::rotate(model, glm::radians(transform->rotation.x), glm::vec3(1.f, 0.f, 0.f));
            model = glm::rotate(model, glm::radians(transform->rotation.y), glm::vec3(0.f, 1.f, 0.f));
            model = glm::rotate(model, glm::radians(transform->rotation.z), glm::vec3(0.f, 0.f, 1.f));
            model = glm::scale(model, transform->scale);

            // Compute MVP
            glm::mat4 mvp = proj * view * model;

            // Pass MVP to the shader
            GLint mvpLoc = glGetUniformLocation(programID, "uMVP");
            if(mvpLoc != -1)
            {
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
            }
            else
            {
                std::cerr << "[RenderWindow] Warning: Uniform 'uMVP' not found in shader." << std::endl;
            }

            // Pass Model matrix to the shader
            GLint modelLoc = glGetUniformLocation(programID, "uModel");
            if(modelLoc != -1)
            {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            }
            else
            {
                std::cerr << "[RenderWindow] Warning: Uniform 'uModel' not found in shader." << std::endl;
            }

            // -----------------------------------
            // 2) Bind the object's diffuse textures
            // -----------------------------------
            // Define the maximum number of diffuse textures as per the shader
            const int MAX_DIFFUSE = 32; // Must match the shader's MAX_DIFFUSE
            int textureUnit = 0;

            // Iterate through all textures and bind those with type "texture_diffuse"
            for (const auto &texture : mesh->textures)
            {
                if (texture.type == "texture_diffuse")
                {
                    if (textureUnit >= MAX_DIFFUSE)
                    {
                        std::cerr << "[RenderWindow] Warning: Exceeded maximum number of diffuse textures (" 
                                  << MAX_DIFFUSE << ") for shader." << std::endl;
                        break; // Prevent exceeding the array bounds in the shader
                    }

                    // Activate the appropriate texture unit
                    glActiveTexture(GL_TEXTURE0 + textureUnit);
                    glBindTexture(GL_TEXTURE_2D, texture.id);
                    CheckOpenGLError("After glBindTexture");

                    // Construct the uniform name dynamically (e.g., "uTextures.texture_diffuse[0]")
                    std::string uniformName = "uTextures.texture_diffuse[" + std::to_string(textureUnit) + "]";
                    GLint texLoc = glGetUniformLocation(programID, uniformName.c_str());

                    if (texLoc != -1)
                    {
                        glUniform1i(texLoc, textureUnit);
                        CheckOpenGLError("After glUniform1i for texture");
                    }
                    else
                    {
                        std::cerr << "[RenderWindow] Warning: Uniform '" << uniformName 
                                  << "' not found in shader." << std::endl;
                    }

                    textureUnit++;
                }
            }

            // Assign default texture to unused texture slots
            for(int i = textureUnit; i < MAX_DIFFUSE; ++i)
            {
                std::string uniformName = "uTextures.texture_diffuse[" + std::to_string(i) + "]";
                GLint texLoc = glGetUniformLocation(programID, uniformName.c_str());
                if(texLoc != -1)
                {
                    glUniform1i(texLoc, 0); // Assign texture unit 0 (ensure texture 0 is a valid default)
                    CheckOpenGLError("After glUniform1i for default texture");
                }
            }

            // Set the number of active diffuse textures
            GLint numDiffuseLoc = glGetUniformLocation(programID, "uNumDiffuseTextures");
            if(numDiffuseLoc != -1)
            {
                glUniform1i(numDiffuseLoc, textureUnit);
                CheckOpenGLError("After glUniform1i for uNumDiffuseTextures");
            }
            else
            {
                std::cerr << "[RenderWindow] Warning: Uniform 'uNumDiffuseTextures' not found in shader." << std::endl;
            }

            // -----------------------------------
            // 3) Draw the object's mesh
            // -----------------------------------
            glBindVertexArray(mesh->vao);
            glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
            CheckOpenGLError("After glDrawElements");

            // Reset active texture
            glActiveTexture(GL_TEXTURE0);
            CheckOpenGLError("After glActiveTexture(GL_TEXTURE0)");
        }
    }

    // Cleanup
    glUseProgram(0);
    CheckOpenGLError("After glUseProgram(0)");

    m_FBO.Unbind();
    CheckOpenGLError("After FBO Unbind");
}
