#version 330 core

layout(location = 0) in vec3 aPos;  // Vertex position
layout(location = 1) in vec2 aUV;   // Texture UV coordinate (optional)

uniform mat4 uMVP;  // Combined Model-View-Projection matrix

out vec2 vUV;       // Pass UV to the fragment shader

void main()
{
    vUV = aUV;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
