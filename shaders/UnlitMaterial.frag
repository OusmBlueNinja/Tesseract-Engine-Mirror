#version 330 core

in vec2 vUV;               // UV from vertex shader
out vec4 FragColor;        // Final color output

uniform vec4 uColor;       // A user-set solid color
uniform sampler2D uTexture; // Optional texture

void main()
{
    // Sample the texture. If you don't want texturing, remove this.
    vec4 texColor = texture(uTexture, vUV);

    // Multiply the texture by our uniform color.
    // If you want a pure color (no texture), just do:
    // FragColor = uColor;
    FragColor = texColor * uColor;
}
