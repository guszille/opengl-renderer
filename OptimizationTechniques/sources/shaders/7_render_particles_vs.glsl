#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 aModelMatrix;
layout (location = 5) in vec4 aColor;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec4 ioColor;

void main()
{
	gl_Position = uProjectionMatrix * uViewMatrix * aModelMatrix * vec4(aPos, 1.0f);

	ioColor = aColor;
}
