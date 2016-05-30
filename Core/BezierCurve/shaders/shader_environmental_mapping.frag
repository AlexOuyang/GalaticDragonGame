#version 330 core

in vec3 fragVert;
in vec3 fragNormal;
in mat4 view;

uniform samplerCube skybox;

out vec4 color;

void main()
{
    
    vec3 reflected = reflect (normalize (fragVert), normalize(fragNormal)); // reflected color of the skybox
    reflected = vec3 (inverse (view) * vec4 (reflected, 0.0));
    
    color = texture (skybox, reflected);
}