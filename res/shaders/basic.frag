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

uniform bool useScreenTex;
uniform sampler2D screenTex;

void main()
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = spec * matSpecular;

    // Phong base
    vec3 ambient = matAmbient * 0.01;
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * matDiffuse;
    vec3 emission = matEmission;

    // Combina colore Phong + texture
    vec3 result = ambient + diffuse + specular + emission;
    // gamma correction
    result = pow(result, vec3(1.0/2.2));
    // Texture 2D

    if(useScreenTex)
    {
        result = texture(screenTex, texCoord).rgb;
    }
    
    FragColor = vec4(result, 1.0);
}
