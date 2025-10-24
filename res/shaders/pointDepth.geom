#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMats[6];     // the lightVPs computed in Scene::shadowMap()

out vec4 fragPos; 

void main()
{
    for(int face = 0; face < 6; face++)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; i++) // for each triangle vertex
        {
            fragPos = gl_in[i].gl_Position;     //TODO study geom shaders
            gl_Position = shadowMats[face] * fragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  