#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform vec3 matEmission;
uniform float matShininess;


uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);

    vec3 specular = spec * matSpecular;
    vec3 emission = matEmission;
    vec3 ambient = matAmbient * 0.01;
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * matDiffuse;
    vec3 result = ambient + diffuse + specular + emission;

    result = pow(result, vec3(1.0/2.2));
    FragColor = vec4(result, 1.0);
}
