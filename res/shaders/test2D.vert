#version 330 core

layout(location = 0) in vec2 pos2D;
layout(location = 1) in vec3 color2D;

out vec3 fragColor; 

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(pos2D, 0.0, 1.0);
    fragColor = color2D; 
}
