#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

uniform bool useMaterial;

// Material settings
uniform struct Material
{
    vec3 k_a; // Ambient coefficient, it is used to define the "inherent color" of the object.
    vec3 k_d; // Material Diffuse reflectance, diffuse material is a material that reflects light rays in many directions
    vec3 k_s; // Specular coefficient, A Specular material is to simulate the effect of shiny objects(such as in metals) creating a "spec" where the reflection direction and eye match up.
    float shininess; // Denoted alpha. It is how much the specular is dispersed over the object. The higher this value, the smaller the highlight "spec".
} material;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};
uniform Light light;

void main()
{
    vec3 lighting;
    
    if (useMaterial)
    {
        // Retrieve data from g-buffer
        vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
        vec3 Normal = texture(gNormal, TexCoords).rgb;
        vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
        float AmbientOcclusion = texture(ssao, TexCoords).r;
        
        // Then calculate lighting as usual
        vec3 ambient = vec3(0.3 * AmbientOcclusion); // <-- this is where we use ambient occlusion
        vec3 viewDir  = normalize(-FragPos); // Viewpos is (0.0.0)
        // Diffuse
        vec3 lightDir = normalize(light.Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
        // Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), material.shininess);
        vec3 specular = light.Color * spec;
        // Attenuation
        float distance = length(light.Position - FragPos);
        float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
        
        // Final lighting
        lighting = attenuation * (diffuse * material.k_d + specular * material.k_s) + ambient * material.k_a;
    }
    else
    {
        // Retrieve data from g-buffer
        vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
        vec3 Normal = texture(gNormal, TexCoords).rgb;
        vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
        float AmbientOcclusion = texture(ssao, TexCoords).r;
        
        // Then calculate lighting as usual
        vec3 ambient = vec3(0.3 * AmbientOcclusion); // <-- this is where we use ambient occlusion
        vec3 viewDir  = normalize(-FragPos); // Viewpos is (0.0.0)
        // Diffuse
        vec3 lightDir = normalize(light.Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
        // Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 8.0);
        vec3 specular = light.Color * spec;
        // Attenuation
        float distance = length(light.Position - FragPos);
        float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
        
        // Final lighting
        lighting = attenuation * (diffuse + specular) + ambient;
    }
    
    FragColor = vec4(lighting, 1.0);
}