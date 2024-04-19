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
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

in VS_OUT {
    vec3 fragPos;
    vec4 fragPosLightSpace;
    vec3 normal;
} fs_in;

uniform Light uLight;
uniform Material uMaterial;
uniform sampler2D uShadowMap;
uniform vec3 uViewPos;

out vec4 oFragColor;

float calcShadowingFactor(vec3 lightDir)
{
    // Perform perspective division.
    vec3 projCoords = fs_in.fragPosLightSpace.xyz / fs_in.fragPosLightSpace.w;

    // Transform to [0,1] range.
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective.
    // float closestDepth = texture(uShadowMap, projCoords.xy).r;

    // Get depth of current fragment from light's perspective.
    float currentDepth = projCoords.z;

    // Check whether current fragment position is in shadow.
    if(projCoords.z > 1.0)
    {
        return 0.0;
    }
    else
    {
        // WARNING:
        //
        // Applying a bias to prevent shadow acne. Choosing the correct bias value(s) requires
        // some tweaking as this will be different for each scene, but most of the time it's simply
        // a matter of slowly incrementing the bias until all acne is removed.
        //
        float bias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005);
        float factor = 0.0;

        vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);

        // Applying percentage-closer filtering.
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;

                factor += (currentDepth - bias > pcfDepth ? 1.0 : 0.0);        
            }   
        }

        factor /= 9.0;

        return factor;
    }
}

void main()
{
    vec3 fragNormal = normalize(fs_in.normal);
    vec3 lightDir = normalize(uLight.position - fs_in.fragPos);
    vec3 viewDir = normalize(uViewPos - fs_in.fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diffuseStr = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = pow(max(dot(fragNormal, halfwayDir), 0.0), uMaterial.shininess);

    // Calculating Blinn-Phong lighting components.
    vec3 ambient = uLight.ambient * uMaterial.diffuse;
    vec3 diffuse = uLight.diffuse * (diffuseStr * uMaterial.diffuse);
    vec3 specular = uLight.specular * (specularStr * uMaterial.specular);

    // Claculating shadowing.
    float shadowingFactor = calcShadowingFactor(lightDir);

    oFragColor = vec4(ambient + ((1.0 - shadowingFactor) * (diffuse + specular)), 1.0);
}

