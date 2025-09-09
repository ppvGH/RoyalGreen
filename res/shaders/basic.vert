#version 330 core
layout (location = 0) in vec3 pos3D;
layout (location = 1) in vec3 normal3D;
layout (location = 2) in vec2 tex3D;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    fragPos = vec3(model * vec4(pos3D, 1.0)); // posizione nel mondo
    normal = mat3(transpose(inverse(model))) * normal3D; // normale corretta in world space
    texCoord = tex3D;
    gl_Position = proj * view * vec4(fragPos,1.0);
}
