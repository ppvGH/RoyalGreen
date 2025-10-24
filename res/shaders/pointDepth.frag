#version 330 core
in vec4 fragPos;

uniform vec3 lightPos;
uniform float invFarPlane;

void main()
{
    // distance from light to fragment
    float lightDistance = length(fragPos.xyz - lightPos);
    
    // map to [0,1] range by dividing by farPlane (multiplying for its inverse)
    lightDistance = lightDistance * invFarPlane;
    
    // new frag depth
    gl_FragDepth = lightDistance;
}  