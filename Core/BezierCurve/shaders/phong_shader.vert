#version 330 core

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 vertNormal;

uniform mat4 perspective;
uniform mat4 camera; // View matrix
uniform mat4 model;  // ModelToWorld Matrix

out vec3 fragVert;
out vec3 fragNormal;

void main()
{
    // Pass some variables to the fragment shader
    fragNormal = vertNormal;
    fragVert = vert;
    
    gl_Position = perspective * camera * model * vec4(vert, 1.0);
}
