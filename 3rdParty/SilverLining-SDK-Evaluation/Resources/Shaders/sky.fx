uniform float4x4 modelViewProj;
uniform float4x4 XYZtoRGB;
uniform float3 xPerezABC;
uniform float3 xPerezDE;
uniform float3 yPerezABC; 
uniform float3 yPerezDE;
uniform float3 YPerezABC; 
uniform float3 YPerezDE;
uniform float3 sunPerez;
uniform float3 zenithPerez;
uniform float3 sunPos;
uniform float3 luminanceScales;
uniform float3 kAndLdmax;
uniform float4 overcast;
uniform float4 fog;
	
float Perez(float3 ABC, float3 DE, float costheta, float gamma, float cosGamma)
{
    float perez = (1.0 + ABC.x * exp(ABC.y / costheta)) *
        (1.0 + ABC.z * exp(DE.x * gamma) + DE.y * cosGamma * cosGamma);

    return perez;
}

float3 toneMap(float3 xyY, float mC, float mR, float k, float Ldmax)
{
    // This is based on Durand's operator, which is based on Ferwerda
    // which is based on Ward...

    // Convert Kcd/m2 to cd/m2
    float Y = xyY.z * 1000.0;

    // deal with negative luminances (nonsensical)
    // max() doesn't work here for some reason...
    if (Y < 0)
    {
       Y = 0;
    }

    float3 XYZ;
    float R;
    // Convert xyY to XYZ
    XYZ.x = xyY.x * (xyY.z / xyY.y);
    XYZ.y = xyY.z;
    XYZ.z = (1.0 - xyY.x - xyY.y) * (xyY.z / xyY.y);
    
	const float3 scotopic = float3(-0.702, 1.039, 0.433);
    R = dot(XYZ, scotopic);

    // Apply perceptual blue-shift in scotopic conditions (Durand)
    const float3 blue = float3(0.3, 0.3, 1);
    xyY = lerp(xyY, blue, k);
    
    // Apply the photopic and scotopic scales that were precomputed in 
    // the app per Ferwerda and Durand's operator
    float Ldp = Y * mC;
    float Lds = R * mR;

    Y = Ldp + k * Lds;
    // Normalize to display range
    Y = Y / Ldmax;

    xyY.z = Y;

    return xyY;
}

float4 xyYtoRGB(float3 xyY, float4x4 XYZtoRGB, float oneOverGamma)
{
    float4 XYZ, RGB;

    // Convert xyY to XYZ
    XYZ.x = xyY.x * (xyY.z / xyY.y);
    XYZ.y = xyY.z;
    XYZ.z = (1.0 - xyY.x - xyY.y) * (xyY.z / xyY.y);
	XYZ.w = 1.0;

    // Convert XYZ to RGB
    RGB = mul(XYZ, XYZtoRGB);

    // Scale down if necessary, preserving color
    float f = max(RGB.y, RGB.z);
    f = max(RGB.x, f); // f is now largest color component
    f = max(1.0, f); // if f is less than 1.0, set to 1.0 (to do nothing)

    RGB.xyz = RGB.xyz / f;

	RGB = saturate(RGB);
	RGB = pow(RGB, oneOverGamma); // apply gamma correction

	RGB.w = 1.0;
    
    return RGB;
}

void VS(  float4 position     : POSITION,
			float4 color        : COLOR,
			float3 texCoord     : TEXCOORD,
			
       	out float4 oPosition    : SV_POSITION,
		out float4 oColor       : COLOR,
		out float3 oTexCoord		: TEXCOORD
    )
{
	oPosition = mul(modelViewProj, position);
	oTexCoord = texCoord;

    float3 xyY;
    float3 xyYo = float3(0.310, 0.316, 0);

    // Find gamma angles
    float3 normalPos = normalize(position).xyz;
    float cosGammaS = dot(sunPos, normalPos);
    float gammaS = acos(cosGammaS);

    // cos(Theta) was stuffed in u coord
    float costheta = texCoord.x;

    // Evaluate Perez functions
    xyY.x = Perez(xPerezABC, xPerezDE, costheta, gammaS, cosGammaS);
    xyY.y = Perez(yPerezABC, yPerezDE, costheta, gammaS, cosGammaS);
    xyY.z = Perez(YPerezABC, YPerezDE, costheta, gammaS, cosGammaS);

    // Normalize against zenith and sun
    xyY = zenithPerez * (xyY / sunPerez);
    
    float overcastBlend = overcast.y;
    float overcastFactor = (1.0 + 2.0 * costheta) / 3.0;
    xyYo.z = zenithPerez.z * overcastFactor;

	xyY = lerp(xyY, xyYo, overcastBlend);
	
    // Map luminance to 0-1, plus do scotopic / mesopic effects
    xyY = toneMap(xyY, luminanceScales.y, luminanceScales.x, kAndLdmax.x, 
        kAndLdmax.y);

	// Add fog
	
	float4 fogColor;
	fogColor.xyz = fog.xyz;
	fogColor.w = 1.0;
	float fogDensity = fog.w;
	float volumeDistance = overcast.w;
	float fogDistance = volumeDistance / costheta;
	float f = exp(-(fogDensity * fogDistance));

	float4 skyColor = xyYtoRGB(xyY, XYZtoRGB, kAndLdmax.z);

	oColor = lerp(fogColor, skyColor, f);
}

float4 PS(float4 posH : SV_POSITION,
		  float4 color : COLOR,
		  float3 texCoord : TEXCOORD ) : SV_TARGET
{
	return color;
}

technique10 ColorTech
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}

technique
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_2_0, VS() ) );
		SetPixelShader( CompileShader( ps_2_0, PS() ) );
	}
}