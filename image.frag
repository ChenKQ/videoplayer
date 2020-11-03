#version 330 core

in vec2 exTexCood;

out vec4 FragColor;

//uniform sampler2D ourtexture;
uniform sampler2D tex;

void main()
{
    FragColor = texture(tex, exTexCood);
}
