#version 330 core

layout (location = 0) in vec4 vert;

uniform mat4 perspective;
uniform mat4 camera; // View matrix
uniform mat4 model;  // ModelToWorld Matrix


void main()
{
    gl_Position = perspective * camera * model * vert;
}