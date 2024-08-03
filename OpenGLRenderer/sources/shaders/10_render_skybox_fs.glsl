#version 460 core

in vec3 ioTexCoords;

uniform samplerCube uCubeMap;

out vec4 oFragColor;

void main()
{
	oFragColor = texture(uCubeMap, ioTexCoords);
}
