#version 330 core

layout(location = 0) in vec3 aPos;      // Vertex position
layout(location = 1) in vec2 aTexCoord; // Texture coordinate
layout(location = 2) in vec3 aNormal;   // Vertex normal

uniform mat4 uMVP;    // Model-View-Projection matrix
uniform mat4 uModel;  // Model matrix

out vec2 TexCoord;    // Passed to fragment shader
out vec3 Normal;      // Passed to fragment shader
out vec3 FragPos;     // Passed to fragment shader

void main()
{
    // Compute the fragment position in world space
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    
    // Transform the normal vector
    Normal = mat3(transpose(inverse(uModel))) * aNormal;  
    
    // Pass through the texture coordinate
    TexCoord = aTexCoord;
    
    // Final vertex position
    gl_Position = uMVP * vec4(aPos, 1.0);
}
