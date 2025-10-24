#version 330 core
out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform vec3 matEmission;
uniform float matShininess;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool useTex;
uniform bool mixTex;
uniform sampler2D sampleTex;
uniform samplerCube depthMap;

uniform float farPlane;

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ?  0.8 : 0.0;


    //return closestDepth;
    return shadow;
}  


/* PHONG: I = e + aA + dD + sS; 
 * E: emissive coefficient vector (material); 
 * A: ambient reflection coefficient vector (material);
 * a: ambient intensity, identical at every point in the scene; 
 * D: diffusive coefficient vector (material); 
 * d: diffusive intensity, = lcos(theta) = dot(L,N)l
      l: source light intensity
      theta: angle between normal and light direction
      N: surface normal vector
      L: direction of light source (pointing towards light source)
 * S: specular coefficient vector (material)
 * s: specular intensity, = lcos(phi)^n = ldot(V,R)^n
      l: source light intensity
      phi: angle between ideal reflection director and viewer direction
      n: shininess coefficient (material)
      V: viewer direction (pointing towards camera)
      R: ideal reflection (symmetric to l wrt n)
 * PHONG: I = E + aA + l[D dot(N,L) + S dot(V,R)^n] = E + aA + l[dD/l+sS/l]
 * BLINN: specular term is s = L dot(H,N)^n
      H : half vector = normalize(L,H)
 * MULTIPLE LIGHTS: I = e + Aa + sum_{l=1}^#lights (Dd_l + Ss_l)
 */

 void main()
 {
    // direction vectors
    vec3 N = normal;
    vec3 L = normalize(lightPos - fragPos); // light direction
    vec3 V = normalize(viewPos - fragPos); // viewer direction
    //vec3 R = reflect(-L,N);     // minus sign because L points towards light
    vec3 H = normalize(L + V);  // half vector for blinn-phong


    // coefficients vectors + shininess
    vec3 E = matEmission;
    vec3 A = matAmbient;
    vec3 D = matDiffuse;
    if(useTex) D = texture(sampleTex, texCoord).rgb;
    vec3 S = matSpecular;
    float n = matShininess;
    

    // intensities
    float a = 0.005;
    float l = 1.0;
    float d =  max(dot(L,N), 0.0);
    //float s = pow(max(dot(R,V), 0.0), n);
    float s = pow(max(dot(H,N), 0.0), n);

    // SHADOW
    float shadow = ShadowCalculation(fragPos);

    // final light intensity vector
    vec3 I = E + a*A + l*(d*D+s*S)*(1.0 - shadow);

    // gamma correction
    I = pow(I, vec3(1.0/2.2));

    

    // if using a texture no gamma correction TODO(create a way to do this just for the screen)
    if(useTex && !mixTex) I = l*d*D;

    //FragColor = vec4(vec3(shadow), 1.0);  
    FragColor = vec4(I, 1.0);
 }

