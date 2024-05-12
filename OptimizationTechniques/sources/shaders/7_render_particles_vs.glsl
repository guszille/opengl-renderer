#version 460 core

layout (location = 0) in vec3 aWorldPos;
layout (location = 1) in vec4 aModelPosAndScale;
layout (location = 2) in vec4 aColor;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform vec3 uWorldCamRight;
uniform vec3 uWorldCamUp;

out vec4 ioColor;

void main()
{
	vec3 vWorldPos = aModelPosAndScale.xyz + (uWorldCamRight * aWorldPos.x * aModelPosAndScale.w) + (uWorldCamUp * aWorldPos.y * aModelPosAndScale.w);

	gl_Position = uProjectionMatrix * uViewMatrix * vec4(vWorldPos, 1.0f);

	ioColor = aColor;
}
