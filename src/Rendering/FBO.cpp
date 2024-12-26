// src/Rendering/FBO.cpp

#include "FBO.h"
#include <cstdio>

bool FBO::Create(int width, int height)
{
    Cleanup(); // In case we already had one

    m_Width  = width;
    m_Height = height;

    // 1) Generate FBO
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    // 2) Create Texture
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, m_TextureID, 0);

    // 3) Create RBO for depth/stencil
    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, m_RBO);

    // 4) Check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "[FBO] Framebuffer not complete!\n");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        Cleanup();
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void FBO::Cleanup()
{
    if (m_TextureID)
    {
        glDeleteTextures(1, &m_TextureID);
        m_TextureID = 0;
    }
    if (m_RBO)
    {
        glDeleteRenderbuffers(1, &m_RBO);
        m_RBO = 0;
    }
    if (m_FBO)
    {
        glDeleteFramebuffers(1, &m_FBO);
        m_FBO = 0;
    }
}

void FBO::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void FBO::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ImTextureID FBO::GetTextureID() const
{
    // For OpenGL + ImGui, typically cast GLuint to (ImTextureID) via uintptr_t
    return (ImTextureID)(uintptr_t)m_TextureID;
}
