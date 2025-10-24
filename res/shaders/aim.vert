#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 col;

uniform mat4 model, projection;

out vec3 outCol;

void main()
{
	gl_Position = projection * model * vec4(pos, 0.0, 1.0);
	outCol = col;
}