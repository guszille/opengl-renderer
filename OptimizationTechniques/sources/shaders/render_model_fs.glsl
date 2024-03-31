#version 460 core

struct Material
{
    sampler2D diffuseMap;
};

in vec2 ioTexCoords;

uniform Material uMaterial;

out vec4 oFragColor;

void main()
{
    oFragColor = vec4(vec3(texture(uMaterial.diffuseMap, ioTexCoords)), 1.0);
}
