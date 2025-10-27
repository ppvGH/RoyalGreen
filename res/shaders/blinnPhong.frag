#version 330 core
out vec4 FragColor;

// from vertex shader
in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

// material uniforms
uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform vec3 matEmission;
uniform float matShininess;


uniform vec3 viewPos;       // camera position


// Arcade display texture
uniform bool useTex;
uniform bool mixTex;
uniform sampler2D sampleTex;

// Point light shadow mapping
uniform vec3 pointLightPos;
uniform samplerCube pointLightDepthMap;
uniform float pointLightFarPlane;

// Spot light shadow mapping
in vec4 fragPosLS;  // fragPos in lightSpace coordinates from vertex shader
uniform vec3 spotLightPos;
uniform vec3 spotLightTar;
uniform sampler2D spotLightDepthMap;


//#define bias 0.2                       // avoids self-shadowing and shadow-acne. (Up this value to decrease shadowacne)
#define shadowIntensity 0.8             // modulate how dark is the shadow (from 0.0 to 1.0, higher = darker)
#define lightIntensity 1.0              // source light intensity (from 0.0 to 1.0, higher = brighter)
#define ambientIntensity 0.005          // how ambient factor impact the lighting
#define colorFilter vec3(0.1, 0.1, 0.8) // give color to light
#define gamma 0.454545454545            // gamma factor 1.0/2.2



float pointLightShadowCalc(vec3 pointL)
{
    // Vector from light to frag position
    vec3 lightToFrag = fragPos - pointLightPos;
    // The closest depth is given by sampling from the depthMap using the lightToFrag vector. 
    // The resulting float is in [0,1], so one has to get its un-normalized value with *farPlane
    float closestDepth = texture(pointLightDepthMap, lightToFrag).r * pointLightFarPlane;
    // Current depth is the length of light to frag vector.
    float currentDepth = length(lightToFrag);
    // If current fragment depth (minus a bias) is greater than the depth sampled from the depthMap
    // means it is behind another object, so it is in shadow. Otherwise it is lightened.
    float bias = max(0.2 * (1.0 - dot(normal, pointL)), 0.05);
    float shadow = currentDepth -  bias > closestDepth ?  shadowIntensity : 0.0;

    return shadow;
}  


float spotLightShadowCalc()
{
    // Perspective division ( from [-w,w] -> [-1,1])
    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;      // TODO: study this
    // Mapping in [0,1]
    projCoords = projCoords*0.5 + 0.5;
    // Sampling from texture depth map
    float closestDepth = texture(spotLightDepthMap, projCoords.xy).r;
    // Current depth is in the z coord
    float currentDepth = projCoords.z;
    // Depth testing
    float shadow = currentDepth - 0.005 > closestDepth ? 1.0 : 0.0;
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
    vec3 N = normalize(normal);     // should be already normalized but
    vec3 pointL = normalize(pointLightPos - fragPos); // point light direction  
    vec3 spotL = normalize(spotLightPos - fragPos);
    vec3 spotDir = normalize(spotLightTar - spotLightPos);
    float theta = dot(-spotL, spotDir);
    // Angoli del cono (in gradi)
    float innerAngle = 30.0;
    float outerAngle = 45.0;

    // Coseni dei coni
    float cosInner = cos(radians(innerAngle));
    float cosOuter = cos(radians(outerAngle));

    float intensity = clamp((theta - cosOuter) / (cosInner - cosOuter), 0.0, 1.0);
    vec3 V = normalize(viewPos - fragPos); // viewer direction
    vec3 pointH = normalize(pointL + V);  // half vector for blinn-phong
    vec3 spotH = normalize(spotL + V);


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
    float point_d =  max(dot(pointL,N), 0.0);
    float spot_d =  max(dot(spotL,N), 0.0);
    float point_s = pow(max(dot(pointH,N), 0.0), n);
    float spot_s = pow(max(dot(spotH,N), 0.0), n);

    // point light shadow
    float shadowPL = pointLightShadowCalc(pointL);
    //point light intensity vector
    vec3 IPL = E + a*A + l*(point_d*D+point_s*S)*(1.0 - shadowPL);
    //point light color filtering
    IPL *= colorFilter;

    // spot light shadow
    float shadowSL = spotLightShadowCalc();
    vec3 ISL = intensity*l*(spot_d*D+spot_s*S)*(1.0 - shadowSL);

    // final light intensity
    vec3 I = (ISL + IPL) * 0.5 ;

    // gamma correction
    I = pow(I, vec3(gamma));


    // if using a texture no gamma correction TODO(create a way to do this just for the screen)
    if(useTex && !mixTex) I = l*point_d*D;

    FragColor = vec4(I, 1.0);
    //FragColor = vec4(vec3(shadowSL) , 1.0);
 }

