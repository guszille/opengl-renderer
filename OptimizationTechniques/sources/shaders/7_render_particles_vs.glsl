#version 460 core

layout (location = 0) in vec3 aWorldPos;
layout (location = 1) in vec4 aModelPosAndScale; // XYZ (position), W (scale).
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aColor;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform vec3 uCamRight;
uniform vec3 uCamUp;

out vec2 ioTexCoord;
out vec4 ioColor;

void main()
{
	vec3 vWorldPos = aModelPosAndScale.xyz + (uCamRight * aWorldPos.x * aModelPosAndScale.w) + (uCamUp * aWorldPos.y * aModelPosAndScale.w);

	gl_Position = uProjectionMatrix * uViewMatrix * vec4(vWorldPos, 1.0f);

	ioTexCoord = aTexCoord;
	ioColor = aColor;
}
