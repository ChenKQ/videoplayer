#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTexCood;

out vec2 exTexCood;

void main()
{
    gl_Position = vec4(inPos, 1.0);
    exTexCood = inTexCood;
}
