#version 330 core

layout(location = 0) in vec2 pos2D;
layout(location = 1) in vec2 tex2D;

out vec2 texCoords; 

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

uniform vec4 uvCoords;

void main()
{
    gl_Position = projection * view * model * vec4(pos2D, 0.0, 1.0);
    texCoords = mix(uvCoords.xy, uvCoords.zw, tex2D); 
}
