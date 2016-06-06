#version 330 core

in vec3 TexCoords;

layout (location = 2) out vec4 color;

uniform samplerCube skybox;

void main()
{
    color = texture(skybox, TexCoords);
//    color = vec4(0.72f, 0.43f, 0.56f, 1.0f);
}