#version 330 core

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 vertNormal;

uniform mat4 perspective;
uniform mat4 camera; // View matrix
uniform mat4 model;  // ModelToWorld Matrix


void main()
{
    gl_Position = perspective * camera * model * vec4(vert, 1.0);
}
