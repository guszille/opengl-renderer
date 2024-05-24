#version 460 core

in vec2 ioTexCoords;

uniform sampler2D uTexture;
uniform float uAlphaThreshold = 0.0;

out vec4 oFragColor;

void main()
{
    oFragColor = texture(uTexture, ioTexCoords);
    
    if (oFragColor.a <= uAlphaThreshold)
    {
        discard;
    }
}
