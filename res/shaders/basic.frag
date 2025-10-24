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


#define bias 0.06                       // avoids self-shadowing and shadow-acne. (Up this value to decrease shadowacne)
#define shadowIntensity 0.8             // modulate how dark is the shadow (from 0.0 to 1.0, higher = darker)
#define lightIntensity 1.0              // source light intensity (from 0.0 to 1.0, higher = brighter)
#define ambientIntensity 0.005          // how ambient factor impact the lighting
#define colorFilter vec3(0.2, 0.2, 1.0) // give color to light
#define gamma 0.454545454545            // gamma factor 1.0/2.2



float shadowCalculation(vec3 fragPos)
{
    // Vector from light to frag position
    vec3 lightToFrag = fragPos - lightPos;
    // The closest depth is given by sampling from the depthMap using the lightToFrag vector. 
    // The resulting float is in [0,1], so one has to get its un-normalized value with *farPlane
    float closestDepth = texture(depthMap, lightToFrag).r * farPlane;
    // Current depth is the length of light to frag vector.
    float currentDepth = length(lightToFrag);
    // If current fragment depth (minus a bias) is greater than the depth sampled from the depthMap
    // means it is behind another object, so it is in shadow. Otherwise it is lightened.
    float shadow = currentDepth -  bias > closestDepth ?  shadowIntensity : 0.0;

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
    float a = ambientIntensity;
    float l = lightIntensity;
    float d =  max(dot(L,N), 0.0);
    //float s = pow(max(dot(R,V), 0.0), n);
    float s = pow(max(dot(H,N), 0.0), n);

    // SHADOW
    float shadow = shadowCalculation(fragPos);

    // final light intensity vector
    vec3 I = E + a*A + l*(d*D+s*S)*(1.0 - shadow);

    // light color filtering
    I *= colorFilter;

    // gamma correction
    I = pow(I, vec3(gamma));


    // if using a texture no gamma correction TODO(create a way to do this just for the screen)
    if(useTex && !mixTex) I = l*d*D;

    FragColor = vec4(I, 1.0);
 }

