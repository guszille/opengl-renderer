#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aModelPos;
layout (location = 2) in vec2 aModelRotationAndScale;
layout (location = 3) in vec4 aColor;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform vec3 uCameraPos;
uniform bool uBuillboarding = false;

out vec4 ioColor;

mat4 calcGeneralRotationMatrix(vec3 axis, float theta)
{
    mat4 R = mat4(0.0);

    R[0][0] = cos(theta) + pow(axis.x, 2) * (1 - cos(theta));
    R[0][1] = (axis.x * axis.y) * (1 - cos(theta)) + axis.z * sin(theta);
    R[0][2] = (axis.x * axis.z) * (1 - cos(theta)) - axis.y * sin(theta);
    R[0][3] = 0.0;
    R[1][0] = (axis.x * axis.y) * (1 - cos(theta)) - axis.z * sin(theta);
    R[1][1] = cos(theta) + pow(axis.y, 2) * (1 - cos(theta));
    R[1][2] = (axis.y * axis.z) * (1 - cos(theta)) + axis.x * sin(theta);
    R[1][3] = 0.0;
    R[2][0] = (axis.x * axis.z) * (1 - cos(theta)) + axis.y * sin(theta);
    R[2][1] = (axis.y * axis.z) * (1 - cos(theta)) - axis.x * sin(theta);
    R[2][2] = cos(theta) + pow(axis.z, 2) * (1 - cos(theta));
    R[2][3] = 0.0;
    R[3][0] = 0.0;
    R[3][1] = 0.0;
    R[3][2] = 0.0;
    R[3][3] = 1.0;

    return R;
}

mat4 calcModelMatrix(bool billboarding = false)
{
    if (billboarding)
    {
        vec3 axis = normalize(uCameraPos - aModelPos);

        // Rotation matrix.
        mat4 R = calcGeneralRotationMatrix(axis, aModelRotationAndScale[0]);

        // Translation matrix.
        mat4 T = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, aModelPos, 1.0);

        // Scale matrix.
        mat4 S = mat4(aModelRotationAndScale[1], 0.0, 0.0, 0.0, 0.0, aModelRotationAndScale[1], 0.0, 0.0, 0.0, 0.0, aModelRotationAndScale[1], 0.0, 0.0, 0.0, 0.0, 1.0);
    
        return S * (T * R);
    }
    else
    {
        // Rotation matrix (over Z-axis).
        mat4 R = mat4(cos(aModelRotationAndScale[0]), sin(aModelRotationAndScale[0]), 0.0, 0.0, -sin(aModelRotationAndScale[0]), cos(aModelRotationAndScale[0]), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
        
        // Translation matrix.
        mat4 T = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, aModelPos, 1.0);

        // Scale matrix.
        mat4 S = mat4(aModelRotationAndScale[1], 0.0, 0.0, 0.0, 0.0, aModelRotationAndScale[1], 0.0, 0.0, 0.0, 0.0, aModelRotationAndScale[1], 0.0, 0.0, 0.0, 0.0, 1.0);
    
        return S * (T * R);
    }
}

mat4 calcBillboardingMatrix()
{
    vec3 pD = normalize(uCameraPos - aModelPos);
    vec3 pR = normalize(cross(vec3(0.0, 1.0, 0.0), pD));
    vec3 pU = normalize(cross(pD, pR));

    // Look at matrix.
    mat4 A = mat4(pR.x, pU.x, pD.x, 0.0, pR.y, pU.y, pD.y, 0.0, pR.z, pU.z, pD.z, 0.0, 0.0, 0.0, 0.0, 1.0);
    mat4 B = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -aModelPos, 1.0);
    mat4 V = inverse(A * B);

    return V;
}

vec3 calcBillboardingPos(vec3 worldPos)
{
    vec3 pD = normalize(uCameraPos - aModelPos);
    vec3 pR = normalize(cross(vec3(0.0, 1.0, 0.0), pD));
    vec3 pU = normalize(cross(pD, pR));

    return (pR * worldPos.x) + (pU * worldPos.y);
}

void main()
{
    if (uBuillboarding)
    {
        vec3 billboardingPos = calcBillboardingPos(aPos);
        mat4 modelMatrix = calcModelMatrix(true);

        gl_Position = uProjectionMatrix * uViewMatrix * modelMatrix * vec4(billboardingPos, 1.0f);
    }
    else
    {
        mat4 modelMatrix = calcModelMatrix();

        gl_Position = uProjectionMatrix * uViewMatrix * modelMatrix * vec4(aPos, 1.0f);
    }

    ioColor = aColor;
}
