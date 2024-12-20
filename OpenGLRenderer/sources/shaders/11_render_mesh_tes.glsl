#version 460 core

layout (quads, fractional_odd_spacing, ccw) in;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

uniform sampler2D uHeightMap;

in vec2 oiTCSTexCoords[];

out float oiTESFragHeight;

void main()
{
    // Get patch coordinates.
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // Retrieve control point texture coordinates.
    vec2 t00 = oiTCSTexCoords[0];
    vec2 t01 = oiTCSTexCoords[1];
    vec2 t10 = oiTCSTexCoords[2];
    vec2 t11 = oiTCSTexCoords[3];

    // Bilinearly interpolate texture coordinate across the patch.
    vec2 tA = (t01 - t00) * u + t00;
    vec2 tB = (t11 - t10) * u + t10;
    vec2 t = (tB - tA) * v + tA;

    // Look-up texel at patch coordinate for height and scale + shift as desired.
    oiTESFragHeight = texture(uHeightMap, t).y * 64.0 - 16.0;

    // Retrieve control point position coordinates.
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // Compute patch surface normal.
    vec4 pU = p01 - p00;
    vec4 pV = p10 - p00;
    vec4 n = normalize(vec4(cross(pV.xyz, pU.xyz), 0));

    // Bilinearly interpolate position coordinate across the patch.
    vec4 pA = (p01 - p00) * u + p00;
    vec4 pB = (p11 - p10) * u + p10;
    vec4 p = (pB - pA) * v + pA;

    // Displace point along normal.
    p += n * oiTESFragHeight;

    // Output patch point position in clip space.
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * p;
}
