#version 460 core

in float oiHeight;

out vec4 oFragColor;

void main()
{
    float normalizedHeight = (oiHeight + 16.0) / 32.0;

    oFragColor = vec4(normalizedHeight, normalizedHeight, normalizedHeight, 1.0);
}
