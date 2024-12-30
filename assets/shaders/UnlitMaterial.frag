#version 330 core

// Struct to hold an array of diffuse textures
struct TextureArray {
    sampler2D texture_diffuse[32]; // Array of diffuse texture samplers
};

// Uniforms
uniform TextureArray uTextures;          // Array of diffuse textures
uniform int uNumDiffuseTextures;         // Number of active diffuse textures

// Input variables from the vertex shader
in vec2 TexCoords;    // Texture coordinates

// Output fragment color
out vec4 FragColor;

void main()
{
    // Define grid dimensions
    const int gridCols = 8; // Number of columns in the grid
    const int gridRows = 4; // Number of rows in the grid
    const float gridWidth = 1.0 / float(gridCols);
    const float gridHeight = 1.0 / float(gridRows);
    
    // Calculate grid cell indices based on TexCoords
    int col = int(floor(TexCoords.x / gridWidth));
    int row = int(floor(TexCoords.y / gridHeight));
    
    // Clamp indices to grid boundaries to prevent out-of-range access
    col = clamp(col, 0, gridCols - 1);
    row = clamp(row, 0, gridRows - 1);
    
    // Calculate texture index based on row and column
    int texIndex = row * gridCols + col;
    
    // Clamp texture index to the number of active textures
    texIndex = clamp(texIndex, 0, uNumDiffuseTextures - 1);
    
    // Calculate local texture coordinates within the grid cell
    vec2 localTexCoords = fract(TexCoords / vec2(gridWidth, gridHeight));
    
    // Sample the selected texture using localTexCoords
    vec4 color = texture(uTextures.texture_diffuse[texIndex], localTexCoords);
    
    // Set the final fragment color
    FragColor = color;
}
