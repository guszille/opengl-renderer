#version 460 core

uniform vec3 uColor;

out vec4 oFragColor;

void main()
{
    oFragColor = vec4(uColor, 1.0);
}
