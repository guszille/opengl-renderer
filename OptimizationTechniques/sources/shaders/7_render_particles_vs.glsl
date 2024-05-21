#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aModelPos;
layout (location = 2) in vec2 aModelRotationAndScale;
layout (location = 3) in vec4 aColor;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform vec3 uCameraPos;

out vec4 ioColor;

mat4 calcModelMatrix()
{
    vec3 camD = normalize(uCameraPos - aModelPos);
    vec3 camR = normalize(cross(camD, vec3(0.0, 1.0, 0.0)));
    vec3 camU = normalize(cross(camD, camR));
    vec3 iModelPos = -aModelPos;

    // Look at matrix.
    mat4 A = mat4(camR.x, camU.x, camD.x, 0.0, camR.y, camU.y, camD.y, 0.0, camR.z, camU.z, camD.z, 0.0, 0.0, 0.0, 0.0, 1.0);
    mat4 B = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, iModelPos, 1.0);
    mat4 V = inverse(A * B);

    // Translation matrix.
    mat4 T = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, aModelPos, 1.0);

    // Rotation matrix (over Z-axis).
    mat4 R = mat4(cos(aModelRotationAndScale[0]), sin(aModelRotationAndScale[0]), 0.0, 0.0, -sin(aModelRotationAndScale[0]), cos(aModelRotationAndScale[0]), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);

    // Scale matrix.
    mat4 S = mat4(aModelRotationAndScale[1], 0.0, 0.0, 0.0, 0.0, aModelRotationAndScale[1], 0.0, 0.0, 0.0, 0.0, aModelRotationAndScale[1], 0.0, 0.0, 0.0, 0.0, 1.0);
    
    return (T * R) * S; // return ((T * R) * S) * V;
}

void main()
{
    mat4 modelMatrix = calcModelMatrix();

    gl_Position = uProjectionMatrix * uViewMatrix * modelMatrix * vec4(aPos, 1.0f);

    ioColor = aColor;
}
