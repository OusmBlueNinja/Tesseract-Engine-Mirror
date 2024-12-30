#version 330 core

// Input vertex attributes (from the VAO)
layout(location = 0) in vec3 aPos;        // Vertex position
layout(location = 1) in vec3 aNormal;     // Vertex normal
layout(location = 2) in vec2 aTexCoords;  // Vertex texture coordinates

// Uniforms
uniform mat4 uMVP;    // Model-View-Projection matrix
uniform mat4 uModel;  // Model matrix

// Output variables to the fragment shader
out vec2 TexCoords;    // Passed texture coordinates
out vec3 Normal;       // Passed normal vector
out vec3 FragPos;      // Passed fragment position

void main()
{
    // Calculate the position of the vertex in clip space
    gl_Position = uMVP * vec4(aPos, 1.0);
    
    // Calculate the position of the vertex in world space
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    
    // Calculate the normal vector in world space
    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    
    // Pass through the texture coordinates
    TexCoords = aTexCoords;
}
