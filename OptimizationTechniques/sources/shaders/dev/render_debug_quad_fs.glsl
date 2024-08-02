#version 460 core

in vec2 oiTexCoords;

uniform sampler2D uTexture;
uniform int uColorChannels = 1;
uniform float uNear = 0.1;
uniform float uFar = 1000.0;

out vec4 oFragColor;

void main()
{
    if (uColorChannels == 3)
    {
        oFragColor = vec4(texture(uTexture, oiTexCoords).rgb, 1.0);
    }
    else // Expecting "uColorChannels == 1".
    {
        float expDepth = texture(uTexture, oiTexCoords).r;
        float linearDepth = 2.0 * uNear * uFar / (uFar + uNear - (2.0 * expDepth - 1.0) * (uFar - uNear));
        float normalizationFactor = uNear * uFar;

        oFragColor = vec4(vec3(linearDepth / normalizationFactor), 1.0);
    }
}
