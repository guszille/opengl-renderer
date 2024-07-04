#version 460 core

#define GAMMA 2.2

in vec2 ioTexCoords;
  
uniform sampler2D uScreenTex;
uniform int uActiveEffect = 0;
uniform bool uGammaCorrection = false;
uniform float uExposure = 1.0; // Only used when HDR is activated.

out vec4 oFragColor;

void main()
{
    vec4 texel = texture(uScreenTex, ioTexCoords);

    // Applaying post-processing effects.
    switch (uActiveEffect)
    {
    case 0: // HDR.
        oFragColor = vec4(vec3(1.0) - exp(-texel.rgb * uExposure), 1.0);
        break;

    default:
        oFragColor = texel;
        break;
    }

    // Applaying manual gamma correction.
    if (uGammaCorrection)
    {
        oFragColor = vec4(pow(oFragColor.rgb, vec3(1.0 / GAMMA)), 1.0);
    }
}
