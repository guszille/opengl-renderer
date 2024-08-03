#version 460 core

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

in VS_OUT {
    vec2 texCoords;
    vec3 fragPos;
    vec3 fragNormal;
} fs_in;

uniform Light uLight;
uniform Material uMaterial;
uniform vec3 uViewPos;

out vec4 oFragColor;

void main()
{
    vec3 lightDir = normalize(uLight.position - fs_in.fragPos);
    vec3 viewDir = normalize(uViewPos - fs_in.fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diffuseStr = max(dot(fs_in.fragNormal, lightDir), 0.0);
    float specularStr = pow(max(dot(fs_in.fragNormal, halfwayDir), 0.0), uMaterial.shininess);

    vec3 diffuseColor = vec3(texture(uMaterial.diffuseMap, fs_in.texCoords));
    vec3 specularColor = vec3(texture(uMaterial.specularMap, fs_in.texCoords));

    // Calculating Blinn-Phong lighting components.
    vec3 ambient = uLight.ambient * diffuseColor;
    vec3 diffuse = uLight.diffuse * (diffuseStr * diffuseColor);
    vec3 specular = uLight.specular * (specularStr * specularColor);

    float lightDistance = length(uLight.position - fs_in.fragPos);
    float attenuation = 1.0 / (lightDistance * lightDistance);

    oFragColor = vec4(ambient + ((diffuse + specular) * attenuation), 1.0);
}
