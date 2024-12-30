#version 330 core

// Struct to hold an array of diffuse textures
struct TextureArray {
    sampler2D texture_diffuse[32]; // Array of diffuse texture samplers
};

// Uniforms
uniform TextureArray uTextures;          // Array of diffuse textures
uniform int uNumDiffuseTextures;         // Number of active diffuse textures

// Input variables from the vertex shader
in vec2 TexCoords;       // Texture coordinates
flat in int TextureIndex; // Texture index for this fragment

// Output fragment color
out vec4 FragColor;

void main()
{
    // Clamp the texture index to prevent out-of-bounds access
    int texIndex = clamp(TextureIndex, 0, uNumDiffuseTextures - 1);

    // Sample the texture using the provided index and texture coordinates
    vec4 sampledColor = texture(uTextures.texture_diffuse[texIndex], TexCoords);

    // Set the final fragment color
    FragColor = sampledColor;
}
