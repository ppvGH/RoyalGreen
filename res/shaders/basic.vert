#version 330 core
layout (location = 0) in vec3 pos3D;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    FragPos = vec3(model * vec4(pos3D, 1.0)); // posizione nel mondo
    Normal = mat3(transpose(inverse(model))) * normal; // normale corretta in world space
    TexCoord = texCoord;
    gl_Position = proj * view * vec4(FragPos,1.0);
}
