#version 460 core

in vec4 oiCSPos;

uniform sampler2D uReflectionTex;
uniform sampler2D uRefractionTex;

out vec4 oFragColor;

void main()
{
    vec2 ndc = 0.5 + (oiCSPos.xy / oiCSPos.w) / 2.0;
    vec2 reflectTexCoords = vec2(ndc.x, -1.0 * ndc.y);
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);

    vec4 reflectColor = texture(uReflectionTex, reflectTexCoords);
    vec4 refractColor = texture(uRefractionTex, refractTexCoords);

    oFragColor = mix(reflectColor, refractColor, 0.5);
}
