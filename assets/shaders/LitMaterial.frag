#version 330

struct Textures {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normal;
    // Add more as needed
};

uniform Textures uTextures;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main()
{
    // Example: Combine diffuse and specular textures
    vec4 diffuseColor = texture(uTextures.texture_diffuse, TexCoord);
    vec4 specularColor = texture(uTextures.texture_specular, TexCoord);

    // Simple lighting calculation (for demonstration)
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor.rgb;

    vec3 viewDir = normalize(-FragPos); // Assuming camera at origin
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * specularColor.rgb;

    vec3 result = diffuse + specular;
    FragColor = vec4(result, diffuseColor.a);
}
