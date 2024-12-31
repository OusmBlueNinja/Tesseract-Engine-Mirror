#version 330 core

struct TextureArray {
    sampler2D texture_diffuse[32]; // Array of diffuse textures
    // You can add more texture types here (e.g., specular, normal) if needed
};

uniform TextureArray uTextures;           // Array of textures
uniform int uNumDiffuseTextures;         // Number of active diffuse textures

in vec2 TexCoord;       // From vertex shader
in vec3 Normal;         // From vertex shader
in vec3 FragPos;        // From vertex shader

out vec4 FragColor;     // Final fragment color

// Example lighting parameters
uniform vec3 lightPos;   // Position of the light source
uniform vec3 viewPos;    // Position of the camera/viewer

void main()
{
    // Normalize the normal vector
    vec3 norm = normalize(Normal);
    
    // Calculate the direction from the fragment to the light
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Compute the diffuse intensity
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Initialize diffuse color
    vec4 diffuseColor = vec4(0.0);
    
    // Sample and accumulate diffuse textures
    for(int i = 0; i < uNumDiffuseTextures; ++i)
    {
        diffuseColor += texture(uTextures.texture_diffuse[i], TexCoord);
    }
    
    // Apply the diffuse intensity
    diffuseColor *= diff;
    
    // Simple ambient lighting
    vec3 ambient = 0.1 * diffuseColor.rgb;
    
    // Final color combining ambient and diffuse components
    FragColor = vec4(ambient + diffuseColor.rgb, diffuseColor.a);
}
