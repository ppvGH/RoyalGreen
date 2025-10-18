// based on "advanced CRT shader" by whkrmrgks0, License GPLv3 TODO: LICENSE.md su github

#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D sampleTex;

// x, y are the viewport width, height respectively and z is the aspect ratio
uniform vec3 viewportResolution;

#define cr vec2(4.,0.)
#define cb vec2(2.,0.)
#define cg vec2(0.,0.)
#define co vec2(3.,1.)

#define actW 2.									// active width	1 should create black bands, 2 is a grid with no black scanlines
#define Tx 6.									// period on x axis
#define Ty 2.									// period on y axis
#define margin 0.05								// fake screen margin
#define hpi 1.5707963268						// pi/2 (half pi)
#define cur 0.15								// crt curvature
#define kerExp 3.0								// kernel exponent
#define horConvSamples 11.0						// convolution sample count (-1). should be always an odd integer
#define invHorConvSamples 1/horConvSamples		// inverse of convolution sample count precomputed
#define horConvWidth 0.8						// amplitude of the convolution. more of this = more horizontal blur
#define scanlines 224.0							// number of scanlines of CRT display. should be always an integer. (224 was the exact number of NEOGEO scanlines)
#define invScanlines 1/scanlines				// inverse of scanlines precomputed
#define vignStr 0.05							// vignette strength
#define verConvSamples 64.0						// vertical convolution samples count (-1) (antialiasing for moire effect)
#define invVerConvSamples 1/verConvSamples		// inverse of vertical convolution samples count precomputed
#define verConvWidth 3.3						// vertical extension of the blurring (antimoiré blur width). more of this = more vertical blur
#define lightIntensity 2.25						// intensity for vertical line bleeding
#define invLightIntensity 1/lightIntensity		// inverse of light intensity precomputed
#define bleedSuppress 1.75						// vertical bleed threshold factor
#define invViewportResY 1/viewportResolution.y	// Inverse of the height resolution of the display precomputed
#define gridIntensity 0.4						// Intensity of the phosphor grid. more of this = grid more explicit




// x is the input, activeWidth the number of pixel the channel is ON, period the cycle length
float square(float x, float activeWidth, float period)
{
    return step(fract(x / period), activeWidth / period);
}


// generates the phosphor grid
float gridGen(vec2 displayCoord, vec2 offset)
{
    return square(displayCoord.x + offset.x , actW, Tx) * square(displayCoord.y + offset.y, actW , Ty);	
}


/* pincushion distortion. 
 * uv = texCoord; 
 * edge = coord further away from the center (for the step function to compute the mask)
 * vign = parameter for vignette effect */
void pincushion(inout vec2 uv, inout float edge, inout float vign)
{
	// remap UV to [-1,1]
	uv = uv * 2. - vec2(1.);

	// add some fake margin, propotional to the framebuffer aspect ratio
	float ar = viewportResolution.z;
	uv *= vec2(1. + margin, 1 + margin * ar);

	// actual pincushion distortion. dividing by a value in [0,1] maps pixels away from center even farther
	uv = vec2(uv.x / cos(hpi * cur * abs(uv.y)), uv.y / cos(hpi * cur * ar * abs(uv.x)));

	// exceeding uv
	vec2 exUV = abs(uv) - vec2(1.);
	// looks for the coord farthest from the center and stores it
	edge = max(exUV.x, exUV.y);

	// vignette effect parameter
	vign = max(exUV.x * exUV.y, 0.);

	// remap to [0,1] but can exceed due to distortion and margin
	uv = (uv + vec2(1.)) * 0.5;
}


float sinKernel(float i)
{
	float kerPos = (i + horConvSamples * 0.5) * invHorConvSamples;
	return sin(pow(kerPos, kerExp)* 4 * hpi) + 1.;		// sin(2*pi*(kernelPosition^kernelExponent)) + 1
}


/* Takes as input UV coords and three vec4 initialized to vec4(0). */
void horizontalBlur(vec2 uv, inout vec4 currLine, inout vec4 upLine, inout vec4 lowLine)
{
	// height of the center of the band (scanlines). OpenGL has (0,0) in the low left corner so up is y+1, low is y-1. (opposite to CG standard)
	float currLineY = floor(uv.y * scanlines) * invScanlines;		// central line
	float upLineY = floor(uv.y * scanlines + 1.) * invScanlines;	// upper line
	float lowLineY = floor(uv.y * scanlines - 1.) * invScanlines;	// lower line

	// Convolution loop
	float weightsSum = 0.;
	for (float i = - horConvSamples * 0.5; i <= horConvSamples * 0.5; i++)
	{
		float weight = sinKernel(i);	// kernel((i+cS/2)/cS) = K(i); argument is [0,1]
		weightsSum += weight;														// sum of all weights

		// x coordinate for texture sampling: X = x - 2i/cS * cW/scanlines
		float sampleCoordX = uv.x - i * 2.* invHorConvSamples * horConvWidth * invScanlines;

		// input: input(x-c*i) (the signal)
		// current line
		vec4 currLineInput = texture(sampleTex, vec2(sampleCoordX, currLineY));
		currLine += weight * currLineInput;
		// upper line
		vec4 upLineInput = texture(sampleTex, vec2(sampleCoordX, upLineY));
		upLine += weight * upLineInput;		
		// lower line
		vec4 lowLineInput = texture(sampleTex, vec2(sampleCoordX, lowLineY));
		lowLine += weight * lowLineInput;	
	}
	// Normalization
	float invWeightSum = 1. / weightsSum;
	currLine *= invWeightSum;
	upLine *= invWeightSum;
	lowLine *= invWeightSum;
}

// Triangle shaped function to simulate vertical intensity profile of the electron flux of CRT (i.e. the signal). Ideally a gaussian.
float triangle(float x)
{
	return abs(2 * fract(x + 0.5) - 1.);
}

// Vertical line bleeding
vec4 lineBleed(float samplePhase, vec4 currLine, vec4 upLine, vec4 lowLine)
{
	// Final line output
	vec4 lineRGBA = vec4(0.);

	// Current line color: when triangle is peaking the intensity is currLine * lightIntensity.
	lineRGBA += max(triangle(samplePhase) - 1. + lightIntensity * currLine, 0.);

	// Bleeding color from the line above.
	float upSamplePhase = clamp(2. * samplePhase - 1., 0., 1.);		// interval is centered and clamped in [0, 1] (to take the upper half of centered interval)
	lineRGBA += max(upSamplePhase - bleedSuppress + lightIntensity * upLine, 0.);

	// Bleeding color from the line below.
	float lowSamplePhase = - clamp(2.* samplePhase -1., -1. , 0.);		// centered and clamped in [-1,0] (taking lower half of center interval). Minus sign to get abs val
	lineRGBA += max(lowSamplePhase - bleedSuppress + lightIntensity * lowLine, 0.);

	// Scaling output intensity.
	lineRGBA *= invLightIntensity;		

	return lineRGBA;
}

float tentKer(float i)
{
	return 1 - invVerConvSamples * 2. * abs(i);
}

// Anti moiré effect with vertical line bleeding
vec4 verticalBlur(vec2 uv, vec4 currLine, vec4 upLine, vec4 lowLine)
{


	// phase of the pixel inside a scanline (0 = bottom of the line, 1 = top of the line, 0.5 = center, max intensity) [y]
	float pixelLinePhase = fract(uv.y * scanlines);		

	// Final line output
	vec4 lineRGBA = vec4(0.);
	
	// Convolution loop
	// the convolution is really a correlation because of k(i)*f(y+i), but as the kernel is symmetric
	// a change of variable i -> -i results in the same integral for the convolution k(i)*f(y-i).
	float weightsSum = 0.;
	for(float i = - verConvSamples * 0.5; i <= verConvSamples * 0.5; i++)
	{
		// kernel weight for the convolution.
		// tent (triangular) kernel, 0 at the support edges (I = +-AAz/2) and 1 at the peak (i = 0)
		float weight = tentKer(i);								// symmetric [k(i) = k(-i)]
		weightsSum += weight;									// weights sum

		// offset of the pixelLinePhase. 
		float offset = i * 2. * invVerConvSamples * verConvWidth * scanlines * invViewportResY;	// o = i * 2/AA * vex * s/R.y

		// convolution contribution.
		float samplePhase = pixelLinePhase + offset;		// current phase of the sample
		lineRGBA += weight * lineBleed(samplePhase, currLine, upLine, lowLine);
	}
	// Normalization
	lineRGBA /= weightsSum;

	return lineRGBA;
}

// Sampling with blurring effects for CRT simulation and anti-aliasing
vec4 scanlineCRT(vec2 uv)
{
	// Central (current) scanline, upper and lower scanlines.
	vec4 currLine = vec4(0.);
	vec4 upLine = vec4(0.);
	vec4 lowLine = vec4(0.);

	// Horizontal blur (computed on central, upper and lower lines) + texture sampling.
	horizontalBlur(uv, currLine, upLine, lowLine);

	// Vertical blur (antialiasing) + lighting setting of the central line + line bleeding from upper and lower lines.
	return verticalBlur(uv, currLine, upLine, lowLine);
}


void main()
{
	// Pincushion distortion is applied to UV so when the texture is sampled with distorted coordinates has a barrel effect (opposite to pincushion)
	vec2 uv = texCoord;				// texCoord cant be passed to pincushion because its const
	float edge;
	float vign;
	pincushion(uv, edge, vign); 

	// Mask: if 0 < edge => pixel is black. (applies barrel effect)
	float mask = step(edge, 0.);
	// Vignette effect. (darken display corners)
	mask *= pow(vign, vignStr);

	// Phosphor grid composition (with distorted UV).
	vec2 displayCoord = uv * viewportResolution.xy;		
	vec3 grid = vec3(gridGen(displayCoord, cr), gridGen(displayCoord, cg), gridGen(displayCoord, cb));				// 1st grid
    grid += vec3(gridGen(displayCoord, cr + co), gridGen(displayCoord, cg + co), gridGen(displayCoord, cb + co));	// 2nd grid w/ offset

	// Sampling texture with CRT effects.
	vec4 scan = scanlineCRT(uv);

	// Gamma correction (
    scan.rgb = pow(scan.rgb, vec3(1./2.2));

	// Scanlines overlapped with phosphor grid points.
	vec4 gridScan = scan * gridIntensity * vec4(grid,1.);

	// Final output color. 
	// Grid is visible in dark areas (when scan is low), while barely noticeable in bright areas (when scan is high).
	vec4 finalColor = min(vec4(mix(gridScan, scan, scan)), vec4(1.)) * mask;

	// Final fragment color.
	FragColor = vec4(finalColor);
}

