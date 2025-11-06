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

// camera position
uniform vec3 viewPos;       

// Arcade display texture
uniform bool useTex;
uniform bool mixTex;
uniform sampler2D sampleTex;

/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* MAIN Point Light (PL) *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

uniform vec3 mainPLPos;                 // light source position
uniform samplerCube mainPLDepthMap;     // depth map FBO texture
uniform float mainPLFarPlane;           // far plane

/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* TABLE Point Lights (PLs) *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

                                                    /* COMMON */
// shadow mapping
uniform float tablePLFarPlane;          
// fixing near plane issue
uniform float tableLightYoffset;
// smoothing edges
uniform float tableCosInner;
uniform float tableCosOuter;
uniform float invCosDelta;
                                                    /* X MIN */
uniform vec3 tableXMinPLPos;                // light source position
uniform samplerCube tableXMinPLDepthMap;    // depth map FBO texture

                                                    /* X PLUS */
uniform vec3 tableXPlusPLPos;                // light source position
uniform samplerCube tableXPlusPLDepthMap;    // depth map FBO texture

/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* Constants *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

// modulate how dark is the shadow (from 0.0 to 1.0, higher = darker)
#define mainShadowIntensity 0.95             
#define tableShadowIntensity 1.0
// source light intensity (from 0.0 to 1.0, higher = brighter)
#define mainLightIntensity 1.0             
#define tableLightIntensity 2.0   
// how ambient factor impact the lighting
#define ambientIntensity 0.0001           
// give color to light
#define mainColorFilter normalize(vec3(0.1, 0.1, 0.8)) 
// gamma factor 1.0/2.2
#define gamma 0.454545454545
// -Y (direction of fake spotlight shed by table lights)
#define tableDir normalize(vec3(0.0, -1.0, 0.0))

/* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float PLShadowCalcPCF(vec3 pointL, vec3 pointLightPos, samplerCube pointLightDepthMap, float pointLightFarPlane, float shadowIntensity)
{
    // Vector from light to frag position
    vec3 lightToFrag = fragPos - pointLightPos;

    // Current depth is the length of light to frag vector.
    float currentDepth = length(lightToFrag);

    // If current fragment depth (minus a bias) is greater than the depth sampled from the depthMap
    // means it is behind another object, so it is in shadow. Otherwise it is lightened.
    float bias = max(0.2 * (1.0 - dot(normal, pointL)), 0.25);
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.75 + (viewDistance / pointLightFarPlane)) / 100.0;
    float shadow = 0.0f;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointLightDepthMap, lightToFrag + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= pointLightFarPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth) shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}  




float simplePLShadowCalc(vec3 pointL, vec3 pointLightPos, samplerCube pointLightDepthMap, float pointLightFarPlane, float shadowIntensity)
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
    float bias = max(0.2 * (1.0 - dot(normal, pointL)), 0.1);
    float shadow = currentDepth -  bias > closestDepth ?  shadowIntensity : 0.0;

    return shadow;
}  


/* Light source is just above the center of a lightbulb, behind a lampshade. 
* The near plane can't be too big because it would miss the lampshade intern, nor too small because it would project the lightbulb shadow. 
* This hack is to cutoff some of the unwanted lights coming out of the lampshade (because the near plane is too big). 
* Harcoded values are chosen somewhat empirically. */
void lampshadeHack(vec3 lightPos, inout float shadowMap)
{
    bool condYforXZ = fragPos.y > lightPos.y - 0.7;                                         // fragments above the quote at which unwanted lights appear that
    bool condXZ = abs(fragPos.x - lightPos.x) > 0.3 || abs(fragPos.z - lightPos.z) > 0.3;   // are also out of a square centered at the light src 
    if(condYforXZ && condXZ) shadowMap = tableShadowIntensity;                              // are put in shadow
    bool condY = (fragPos.y > lightPos.y + tableLightYoffset);                              // the part before left a square on the ceiling not in shadow, so with this
    if(condY) shadowMap = tableShadowIntensity;                                             // those frags are also put in shadow
}  


// unused
float computePLIntensity(vec3 lightPos, float farPlane, float maxIntensity)
{
   return mix(0.0, maxIntensity, (length(lightPos - fragPos)) / farPlane);
}





 void main()
 {
    /* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  Common variables *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */  

    // common vectors
    vec3 N = normalize(normal);                             // should be already normalized but
    vec3 V = normalize(viewPos - fragPos);                  // viewer direction

    // coefficient vectors
    vec3 E = matEmission;
    vec3 A = matAmbient;
    vec3 D = matDiffuse;
    if(useTex) D = texture(sampleTex, texCoord).rgb;        // useTex is used by Arcade Display to show its FBO texture
    vec3 S = matSpecular;

    // coefficient scalars
    float n = matShininess;         
    float a = ambientIntensity;     

    /* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

    /* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  Main light (PL) *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

    // vectors
    vec3 mainPointL = normalize(mainPLPos - fragPos);       // main point light direction  
    vec3 mainPointH = normalize(mainPointL + V);            // main half vector for blinn-phong

    // coefficients
    float mainPoint_l =  mainLightIntensity; //   computePLIntensity(mainPLPos, mainPLFarPlane, mainLightIntensity);   //
    float mainPoint_d =  max(dot(mainPointL,N), 0.0);               // main light diffusion intensity
    float mainPoint_s = pow(max(dot(mainPointH,N), 0.0), n);        // main light specular intensity

    // shadow
    float mainShadowPL = PLShadowCalcPCF(mainPointL, mainPLPos, mainPLDepthMap, mainPLFarPlane, mainShadowIntensity);

    // final intensity
    vec3 mainIPL = E + a*A + mainPoint_l*(mainPoint_d*D+mainPoint_s*S)*(1.0 - mainShadowPL);

    // color filtering
    mainIPL *= mainColorFilter;

    /* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

    /* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  Table lights (PL) *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */
    /* This part is a sort of combination between a spotlight with conic shape to render lights towards the pool table, 
     * and a pointlight with a cubemap just inside the lampshade to render lights inside of it. */

  
   
                                                    /* X MIN */
    // vectors
    vec3 tableXMinL = normalize(tableXMinPLPos - fragPos);     // point light direction  
    vec3 tableXMinH = normalize(tableXMinL + V);          // table half vector for blinn-phong

    // coefficients
    float tableXMin_d =  max(dot(tableXMinL,N), 0.0);             // table light diffusion intensity
    float tableXMin_s = pow(max(dot(tableXMinH,N), 0.0), n);      // table light specular intensity

    // shadow
    float tableXMinShadowPL = PLShadowCalcPCF(tableXMinL, tableXMinPLPos, tableXMinPLDepthMap, tablePLFarPlane, tableShadowIntensity);

    // unwanted lights correction
    lampshadeHack(tableXMinPLPos, tableXMinShadowPL);

    // final intensity
    vec3 tableXMinIPL = tableLightIntensity * (tableXMin_d*D+tableXMin_s*S)*(1.0 - tableXMinShadowPL);  // no ambient or emission to reduce cumulative lighting effects

    // Smoothing edges while simulating a spotlight
    // cosine of angle between PL direction and "spotlight" direction
    float cosThetaXMin = dot(tableXMinL, -tableDir);       // tableDir negated to make it point to the light source
    // smooth mask
    float coneXMin = clamp((cosThetaXMin - tableCosOuter) * invCosDelta, 0.0, 1.0);
    // apply smooth mask only on floor
    if (fragPos.y < 0.00001) tableXMinIPL *= coneXMin;

                                                                        /* X PLUS */
    // vectors
    vec3 tableXPlusL = normalize(tableXPlusPLPos - fragPos);     // point light direction  
    vec3 tableXPlusH = normalize(tableXPlusL + V);          // table half vector for blinn-phong

    // coefficients
    float tableXPlus_d =  max(dot(tableXPlusL,N), 0.0);             // table light diffusion intensity
    float tableXPlus_s = pow(max(dot(tableXPlusH,N), 0.0), n);      // table light specular intensity

    // shadow
    float tableXPlusShadowPL = PLShadowCalcPCF(tableXPlusL, tableXPlusPLPos, tableXPlusPLDepthMap, tablePLFarPlane, tableShadowIntensity);

    // unwanted lights correction
    lampshadeHack(tableXPlusPLPos, tableXPlusShadowPL);

    // final intensity
    vec3 tableXPlusIPL = tableLightIntensity * (tableXPlus_d*D+tableXPlus_s*S)*(1.0 - tableXPlusShadowPL); // no ambient or emission to reduce cumulative lighting effects

    // Smoothing edges while simulating a spotlight
    // cosine of angle between PL direction and "spotlight" direction
    float cosThetaXPlus = dot(tableXPlusL, -tableDir);       // tableDir negated to make it point to the light source
    // smooth mask
    float coneXPlus = clamp((cosThetaXPlus - tableCosOuter) * invCosDelta, 0.0, 1.0);
    // apply smooth mask only on floor
    if (fragPos.y < 0.00001) tableXPlusIPL *= coneXPlus;

    /* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */
    
    /* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  Total light intensity  *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

    // final light intensity (sum of intensities). N.B. only mainIPL has emission and ambient factors
    vec3 I = mainIPL + tableXMinIPL + tableXPlusIPL;

    // gamma correction
    I = pow(I, vec3(gamma));
    
    // Just for the Display, because floor has mixTex = 1. 
    // Used to set just the diffusion factor without ambient, emission, gamma correction.
    if(useTex && !mixTex) I = mainPoint_l*mainPoint_d*D;

    // Fragment color output
    FragColor = vec4(I, 1.0);
    //FragColor = vec4(vec3(length(tableXMinPLPos - fragPos)/tablePLFarPlane), 1.0); 

    /* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

 }


 /* THEORY
 * PHONG: I = e + aA + dD + sS; 
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


 // NOT USED

    // Spot light shadow mapping
// in vec4 fragPosLS;  // fragPos in lightSpace coordinates from vertex shader
// uniform vec3 spotLightPos;
// uniform vec3 spotLightTar;
// uniform sampler2D spotLightDepthMap;

//float spotLightShadowCalc()
//{
//    // Perspective division ( from [-w,w] -> [-1,1])
//    vec3 projCoords = fragPosLS.xyz / fragPosLS.w;      // TODO: study this
//    // Mapping in [0,1]
//    projCoords = projCoords*0.5 + 0.5;
//    // Sampling from texture depth map
//    float closestDepth = texture(spotLightDepthMap, projCoords.xy).r;
//    // Current depth is in the z coord
//    float currentDepth = projCoords.z;
//    // Depth testing
//    float shadow = currentDepth - 0.005 > closestDepth ? 1.0 : 0.0;
//    return shadow;
//}
    /* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*  Spot light (SL)  *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */

    //vec3 spotL = normalize(spotLightPos - fragPos);
    //vec3 spotH = normalize(spotL + V);
    //float spot_d =  max(dot(spotL,N), 0.0);
    //float spot_s = pow(max(dot(spotH,N), 0.0), n);
    //float shadowSL = spotLightShadowCalc();
    //vec3 ISL = intensity*l*(spot_d*D+spot_s*S)*(1.0 - shadowSL);

    /* *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* */
