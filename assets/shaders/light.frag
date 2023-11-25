#version 430

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D ourTexture;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirectionLight
{
    int enable;
    vec3 direction;
    vec3 lightColor;
};

struct PointLight {
    int enable;
    vec3 position;  
    vec3 lightColor;

    float constant;
    float linear;
    float quadratic;
};

struct Spotlight {
    int enable;
    vec3 position;
    vec3 direction;
    vec3 lightColor;
    float cutOff;

    // Parameters for attenuation formula
    float constant;
    float linear;
    float quadratic;      
}; 

uniform Material material;
uniform DirectionLight dl;
uniform PointLight pl;
uniform Spotlight sl;

void main() {
    // Calculate lighting components
    vec3 ambient = dl.enable * dl.lightColor * material.ambient;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Directional Light
    vec3 lightDir = normalize(-dl.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dl.enable * dl.lightColor * (diff * material.diffuse);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = dl.enable * dl.lightColor * (spec * material.specular);

    // Point Light
    if (pl.enable == 1) {
        float distance = length(pl.position - FragPos);
        float attenuation = 1.0 / (pl.constant + pl.linear * distance + pl.quadratic * (distance * distance));

        vec3 lightDirPoint = normalize(pl.position - FragPos);
        float diffPoint = max(dot(norm, lightDirPoint), 0.0);
        vec3 diffusePoint = pl.enable * pl.lightColor * (diffPoint * material.diffuse) * attenuation;

        vec3 reflectDirPoint = reflect(-lightDirPoint, norm);
        float specPoint = pow(max(dot(viewDir, reflectDirPoint), 0.0), material.shininess);
        vec3 specularPoint = pl.enable * pl.lightColor * (specPoint * material.specular) * attenuation;

        diffuse += diffusePoint;
        specular += specularPoint;
    }

    // Spotlight
    if (sl.enable == 1) {
        vec3 lightDirSpot = normalize(sl.position - FragPos);
        float distanceSpot = length(sl.position - FragPos);

        // Attenuation
        float attenuationSpot = 1.0 / (sl.constant + sl.linear * distanceSpot + sl.quadratic * (distanceSpot * distanceSpot));

        // Spotlight cutoff angle
        float theta = dot(lightDirSpot, normalize(-sl.direction));
        float spotEffect = smoothstep(sl.cutOff, 1.0, theta);

        // Diffuse reflection
        float diffSpot = max(dot(norm, lightDirSpot), 0.0);
        vec3 diffuseSpot = sl.enable * sl.lightColor * (diffSpot * material.diffuse) * attenuationSpot * spotEffect;

        // Specular reflection
        vec3 reflectDirSpot = reflect(-lightDirSpot, norm);
        float specSpot = pow(max(dot(viewDir, reflectDirSpot), 0.0), material.shininess);
        vec3 specularSpot = sl.enable * sl.lightColor * (specSpot * material.specular) * attenuationSpot * spotEffect;

        // Add to final result
        diffuse += diffuseSpot;
        specular += specularSpot;
    }


    vec3 result = ambient + diffuse + specular;
    color = texture(ourTexture, TexCoord) * vec4(result, 1.0);
}