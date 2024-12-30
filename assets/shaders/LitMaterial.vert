#version 330 

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 uMVP;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    Normal = aNormal;
    FragPos = vec3(uMVP * vec4(aPos, 1.0));
}
