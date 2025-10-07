#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;

uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform vec3 matEmission;
uniform float matShininess;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool useTex;
uniform sampler2D sampleTex;

void main()
{
    // Vectors
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = spec * matSpecular;

    // Ambient
    vec3 ambient = vec3(0.005);
    
    // Diffuse coeff with albedo (tex or mat) 
    vec3 albedo;
    if(useTex) albedo = texture(sampleTex, texCoord).rgb;
    else albedo = matDiffuse;

    vec3 diffuse = max(dot(norm, lightDir), 0.0) * albedo; //* matDiffuse;

    // Emission
    vec3 emission = matEmission * albedo;

    // Combine colors
    vec3 result = ambient + diffuse + specular + emission;

    // gamma correction
    result = pow(result, vec3(1.0/2.2));
    if(useTex && emission != vec3(0.0)) result = emission;
    else if (useTex) result = pow(albedo, vec3(1.0/2.2));
    
    FragColor = vec4(result, 1.0);
}
