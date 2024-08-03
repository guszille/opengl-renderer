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
    sampler2D emissionMap;
    sampler2D normalMap; // WARN: Not used. Check texture size to use this map.
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

layout (location = 0) out vec4 oFragColor;
layout (location = 1) out vec4 oFragBrightness;

vec3 calcFragColor() // Applying Blinn-Phong lighting model.
{
    vec3 lightDir = normalize(uLight.position - fs_in.fragPos);
    vec3 viewDir = normalize(uViewPos - fs_in.fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diffuseStr = max(dot(fs_in.fragNormal, lightDir), 0.0);
    float specularStr = pow(max(dot(fs_in.fragNormal, halfwayDir), 0.0), uMaterial.shininess);

    vec3 diffuseColor = vec3(texture(uMaterial.diffuseMap, fs_in.texCoords));
    vec3 specularColor = vec3(texture(uMaterial.specularMap, fs_in.texCoords));
    vec3 emissionColor = vec3(texture(uMaterial.emissionMap, fs_in.texCoords));

    vec3 ambient = uLight.ambient * diffuseColor;
    vec3 diffuse = uLight.diffuse * (diffuseStr * diffuseColor);
    vec3 specular = uLight.specular * (specularStr * specularColor);
    vec3 emission = emissionColor;

    float lightDistance = length(uLight.position - fs_in.fragPos);
    float lightAttenuation = 1.0; // 1.0 / (lightDistance * lightDistance);

    return ambient + ((diffuse + specular) * lightAttenuation) + emission;
}

void main()
{
    oFragColor = vec4(calcFragColor(), 1.0);

    float brightness = dot(oFragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > 1.0)
    {
        oFragBrightness = vec4(oFragColor.rgb, 1.0);
    }
    else
    {
        oFragBrightness = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
