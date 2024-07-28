#version 460 core

in vec2 oiTexCoords;

uniform sampler2D uTexture;
uniform int uColorChannels = 1;

out vec4 oFragColor;

void main()
{
    switch (uColorChannels)
    {
    case 3:
        oFragColor = vec4(texture(uTexture, oiTexCoords).rgb, 1.0);
        break;

    default:
        oFragColor = vec4(vec3(texture(uTexture, oiTexCoords).r), 1.0);
        break;
    }
}
