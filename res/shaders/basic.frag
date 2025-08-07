#version 330 core
in vec3 pos;
out vec4 col;

void main()
{
	col = vec4(0.8*pos.x, 0.3*pos.y, 0.2*pos.z, 1.0);
};