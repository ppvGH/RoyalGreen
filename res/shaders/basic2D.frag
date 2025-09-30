#version 330 core

in vec3 outCol;

out vec4 fragCol;

void main()
{
	fragCol = vec4(outCol, 1.0);
}