#version 460 core

in vec4 oiCSPos;
in vec2 oiTexCoords;
in vec3 oiCameraDir;
in vec3 oiLightInvDir;

uniform sampler2D uReflectionTex;
uniform sampler2D uRefractionTex;
uniform sampler2D uDuDvMap;
uniform sampler2D uNormalMap;

uniform float uWaveStrength;
uniform float uWaveStride;
uniform float uShininess;
uniform float uReflectivity;

uniform vec3 uLightColor;

out vec4 oFragColor;

void main()
{
    vec3 cameraDir = normalize(oiCameraDir);
    vec3 lightInvDir = normalize(oiLightInvDir);

    vec2 ndc = 0.5 + (oiCSPos.xy / oiCSPos.w) / 2.0;
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
    vec2 refractTexCoords = vec2(ndc.x,  ndc.y);
    
    // Sampling distortion from a map.
    vec2 distortedTexCoords = texture(uDuDvMap, vec2(oiTexCoords.x + uWaveStride, oiTexCoords.y)).xy * 0.1;
    distortedTexCoords = oiTexCoords + vec2(oiTexCoords.x, oiTexCoords.y + uWaveStride);
    distortedTexCoords = (texture(uDuDvMap, distortedTexCoords).xy * 2.0 - 1.0) * uWaveStrength;

    reflectTexCoords += distortedTexCoords;
    refractTexCoords += distortedTexCoords;

    // Clamping texture coordinates to avoid some border atifacts.
    reflectTexCoords.x = clamp(reflectTexCoords.x,  0.001,  0.999);
    reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);
    refractTexCoords.x = clamp(refractTexCoords.x,  0.001,  0.999);
    refractTexCoords.y = clamp(refractTexCoords.y,  0.001,  0.999);

    vec4 reflectColor = texture(uReflectionTex, reflectTexCoords);
    vec4 refractColor = texture(uRefractionTex, refractTexCoords);

    // Calculating fresnel effect factor.
    float refractiveFactor = dot(oiCameraDir, vec3(0.0, 1.0, 0.0));
    refractiveFactor = pow(refractiveFactor, 0.5);

    // Sampling normal from a map.
    vec4 normalMapColor = texture(uNormalMap, distortedTexCoords);
    vec3 normal = normalize(vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0));

    // Calculating light specular contribution.
    vec3 reflectedLight = reflect(lightInvDir, normal);

    float specularComp = max(dot(reflectedLight, cameraDir), 0.0);
    specularComp = pow(specularComp, uShininess);

    vec3 specularHighlights = uLightColor * specularComp * uReflectivity;

    oFragColor = mix(reflectColor, refractColor, refractiveFactor);
    oFragColor = mix(oFragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
    oFragColor = oFragColor + vec4(specularHighlights, 0.0);
}
