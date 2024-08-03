#version 460 core

in vec4 oiCSPos;
in vec2 oiTexCoords;
in vec3 oiCameraDir;
in vec3 oiLightInvDir;

uniform sampler2D uReflectionTex;
uniform sampler2D uRefractionTex;
uniform sampler2D uDepthMap;
uniform sampler2D uDuDvMap;
uniform sampler2D uNormalMap;

uniform float uWaveStrength;
uniform float uWaveStride;
uniform float uShininess;
uniform float uReflectivity;
uniform float uNear = 0.1;
uniform float uFar = 1000.0;

uniform vec3 uWaterColor;
uniform vec3 uLightColor;

out vec4 oFragColor;

void main()
{
    vec2 ndc = 0.5 + (oiCSPos.xy / oiCSPos.w) / 2.0;
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
    vec2 refractTexCoords = vec2(ndc.x,  ndc.y);

    float floorDistance = texture(uDepthMap, refractTexCoords).r;
    float waterDistance = gl_FragCoord.z;
    float linearFloorDistance = 2.0 * uNear * uFar / (uFar + uNear - (2.0 * floorDistance - 1.0) * (uFar - uNear));
    float linearWaterDistance = 2.0 * uNear * uFar / (uFar + uNear - (2.0 * waterDistance - 1.0) * (uFar - uNear));
    float waterDepth = linearFloorDistance - linearWaterDistance;
    
    // Sampling distortion from a map.
    vec2 distortedTexCoords = texture(uDuDvMap, vec2(oiTexCoords.x + uWaveStride, oiTexCoords.y)).xy * 0.1;
    distortedTexCoords = oiTexCoords + vec2(oiTexCoords.x, oiTexCoords.y + uWaveStride);
    distortedTexCoords = (texture(uDuDvMap, distortedTexCoords).xy * 2.0 - 1.0) * uWaveStrength;
    distortedTexCoords = distortedTexCoords * clamp(waterDepth / 20.0, 0.0, 1.0);

    reflectTexCoords += distortedTexCoords;
    refractTexCoords += distortedTexCoords;

    // Clamping texture coordinates to avoid some border atifacts.
    reflectTexCoords.x = clamp(reflectTexCoords.x,  0.001,  0.999);
    reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);
    refractTexCoords.x = clamp(refractTexCoords.x,  0.001,  0.999);
    refractTexCoords.y = clamp(refractTexCoords.y,  0.001,  0.999);

    vec4 reflectColor = texture(uReflectionTex, reflectTexCoords);
    vec4 refractColor = texture(uRefractionTex, refractTexCoords);

    // Sampling normal from a map.
    vec4 normalMapColor = texture(uNormalMap, distortedTexCoords);
    vec3 fragNormal = normalize(vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0)); // In this case, the blue channel is the Y axis.

    // Calculating fresnel effect factor.
    float refractiveFactor = dot(oiCameraDir, fragNormal);
    refractiveFactor = pow(refractiveFactor, 0.5);
    refractiveFactor = clamp(refractiveFactor, 0.0, 1.0);

    // Calculating light specular contribution.
    vec3 reflectedLight = reflect(oiLightInvDir, fragNormal);

    float specularComp = max(dot(reflectedLight, oiCameraDir), 0.0);
    specularComp = pow(specularComp, uShininess);

    vec3 specularHighlights = uLightColor * specularComp * uReflectivity * clamp(waterDepth / 5.0, 0.0, 1.0);

    oFragColor = mix(reflectColor, refractColor, refractiveFactor);
    oFragColor = mix(oFragColor, vec4(uWaterColor, 1.0), 0.2);
    oFragColor = oFragColor + vec4(specularHighlights, 0.0);

    oFragColor.a = clamp(waterDepth / 5.0, 0.0, 1.0);
}
