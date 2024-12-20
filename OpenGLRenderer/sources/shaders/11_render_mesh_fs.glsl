#version 460 core

in float oiTESFragHeight;

out vec4 oFragColor;

void main()
{
    float normalizedHeight = (oiTESFragHeight + 16.0) / 64.0;

    oFragColor = vec4(normalizedHeight, normalizedHeight, normalizedHeight, 1.0);
}
