#version 460 core

in vec2 ioTexCoord;
in vec4 ioColor;

uniform sampler2D uParticleAtlas;

out vec4 oFragColor;

void main()
{
    oFragColor = texture(uParticleAtlas, ioTexCoord) * ioColor;
}
