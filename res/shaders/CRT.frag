#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D sampleTex;

uniform vec2 displaySize;

void main()
{
	// Fetching texture data.
	vec4 texData = texture(sampleTex, texCoord);
	vec3 color = texData.rgb;
	float alpha = texData.a;

	/* Scanlines. */

	// Line intensity.
	float lineIntensity = 0.95; // default value at 5% darker than normal 
	// Control value for horizontal scanlines: it is [0,2[ ==> if < 1 line is odd and darker, else brighter
	float scanModHorizontal = mod(texCoord.y * displaySize.y, 6.0);
	if(scanModHorizontal < 3.0) lineIntensity = 0.55; // 15% darker than normal

	// Color filtering.
//	vec3 colorFiltering = vec3(1.0, 1.0, 1.0);
//	// Control value for vertical scanlines: it is [0,3[ ===> if < 1 line is redish, if in [1,2[ is blueish, else is greenish
//	float scanModVertical = mod(texCoord.x * displaySize.x, 18.0);
//	if(scanModVertical < 6.0) colorFiltering = vec3(1.0, 0.5, 0.5) * (scanModVertical) * 0.167;
//	else if(scanModVertical < 12.0) colorFiltering = vec3(0.5, 0.5, 1.0) * (scanModVertical-6.0) * 0.167;
//	else colorFiltering = vec3(0.5, 1.0, 0.5) * (scanModVertical-12.0) * 0.167;


	// Update color
	color *= lineIntensity; //* mix(color, colorFiltering, 0.2);

	// CRT gamma
	//color = pow(color,vec3(1.3));

	FragColor = vec4(color, alpha);

}


//#version 330 core
//out vec4 FragColor;
//
//in vec2 v_TexCoord; // Coordinate UV interpolate dal Vertex Shader
//
//uniform sampler2D u_ScreenTexture; // La texture della scena renderizzata
//uniform vec2 u_Resolution;        // Risoluzione dello schermo (larghezza, altezza)
//uniform float u_Time;             // Tempo (opzionale, per effetto rumore)
//
//void main()
//{
//    // 1. Base del Colore dalla Scena
//    vec3 color = texture(u_ScreenTexture, v_TexCoord).rgb;
//	  float alpha = texture(u_ScreenTexture, v_TexCoord).a;
//    
//    // 2. Scanlines (Linee Orizzontali)
//    // Usiamo la coordinata Y per determinare la riga.
//    // u_Resolution.y ci dà il numero di righe.
//    float scanline_intensity = 0.95; // 0.95 = 5% di oscuramento
//    
//    // Controlla se siamo su una riga pari o dispari
//    // fract(coordinata * numero_righe) ci dà un pattern ripetuto
//    float scan_mod = mod(v_TexCoord.y * u_Resolution.y, 2.0);
//
//    // Se scan_mod è vicino a 1.0 (riga dispari), riduci l'intensità del colore
//    if (scan_mod < 1.0) {
//        scanline_intensity = 0.85; // Oscura un po' di più
//    }
//    
//    color *= scanline_intensity;
//
//
//    // 3. Vignettatura (Oscuramento sui Bordi)
//    // Calcola la distanza dal centro (0.5, 0.5)
//    vec2 offset = v_TexCoord - 0.5;
//    
//    // Calcola il fattore di oscuramento (più si è lontani dal centro, più si oscura)
//    float vignette = 1.0 - (dot(offset, offset) * 1.5); // 1.5 è la forza
//    vignette = clamp(vignette, 0.4, 1.0); // Non farla mai scendere sotto 0.4
//
//    color *= vignette;
//
//
//    // 4. Correzione Gamma (essenziale per l'aspetto finale)
//    // Non strettamente un effetto CRT, ma rende l'immagine corretta
//    color = pow(color, vec3(1.0/2.2));
//
//    FragColor = vec4(color, alpha);
//}