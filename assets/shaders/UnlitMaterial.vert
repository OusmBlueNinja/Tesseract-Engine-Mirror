#version 330 core

// Input vertex attributes (from the VAO)
layout(location = 0) in vec3 aPos;         // Vertex position
layout(location = 1) in vec3 aNormal;      // Vertex normal
layout(location = 2) in vec2 aTexCoords;   // Vertex texture coordinates
layout(location = 3) in int aTexIndex;     // Texture index (integer)

// Uniforms
uniform mat4 uMVP;    // Model-View-Projection matrix
uniform mat4 uModel;  // Model matrix

// Output variables to the fragment shader
out vec2 TexCoords;        // Passed texture coordinates
flat out int TextureIndex; // Passed texture index

void main()
{
    // Transform vertex position to clip space
    gl_Position = uMVP * vec4(aPos, 1.0);
    
    // Transform vertex position to world space
    vec3 FragPos = vec3(uModel * vec4(aPos, 1.0));
    
    // Transform normal to world space
    vec3 Normal = mat3(transpose(inverse(uModel))) * aNormal;
    
    // Pass through texture coordinates
    TexCoords = aTexCoords;
    
    // Pass through the texture index
    TextureIndex = aTexIndex;
}
