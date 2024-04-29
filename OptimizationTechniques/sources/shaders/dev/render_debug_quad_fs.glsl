#version 460 core

in vec2 ioTexCoords;

uniform sampler2D uTexture;

out vec4 FragColor;

void main()
{
    float value = texture(uTexture, ioTexCoords).r;

    FragColor = vec4(vec3(value), 1.0);
}
