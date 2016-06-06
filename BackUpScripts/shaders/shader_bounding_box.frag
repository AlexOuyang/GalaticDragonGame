#version 330 core


out vec4 color;

uniform bool collided;

void main()
{
    if (collided) color = vec4(1, 0, 0, 1); // Red
    else color = vec4(0, 1, 0, 1);  // Green
}

