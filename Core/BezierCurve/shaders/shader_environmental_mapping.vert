#version 330 core


layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 vertNormal;

uniform mat4 perspective;
uniform mat4 camera; // View matrix
uniform mat4 model;  // ModelToWorld Matrix

out vec3 fragVert;
out vec3 fragNormal;
out mat4 view;

void main()
{
    // color isn't uniform, surfaces change
    fragNormal = vec3(camera * model * vec4(vertNormal, 0.0f));
    fragVert = vec3(camera * model * vec4(vert, 1.0f));
    view = camera;
    
    gl_Position = perspective * camera * model * vec4(vert, 1.0);
}
