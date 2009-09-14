float4x4 invBasis;
float4x4 modelViewProj;
float4x4 equatorialToHorizon;
  
  void VS(  float4 position     : POSITION,
			float4 color        : COLOR,
			float3 texCoord     : TEXCOORD0,
			
       	out float4 oPosition    : SV_POSITION,
		out float4 oColor       : COLOR,
        out float3 oTexCoord    : TEXCOORD0

)
{
    float4 horizonCoords = mul(equatorialToHorizon, position);    
	oPosition = mul(modelViewProj, horizonCoords);

    oTexCoord = texCoord;

    float magnitude = texCoord.x;
    float4 normalizedHorizonCoords = mul(normalize(horizonCoords), invBasis);

    float cosTheta = normalizedHorizonCoords.y;
    float theta = acos(cosTheta);

    // At scotopic levels when stars are visible, the varying effect of scattering by
    // wavelength usually isn't visible. So it's not modeled to gain back a little performance.

    // Convert magnitude to luminance
	float luminance = pow(100.0, (-magnitude / 5.0));

	float r2 = luminance / (pow(luminance, 2.0/3.0) + 0.02); // The .02 term controls the overall star brightness.
	float4 luminanceRGB = float4(r2, r2, r2, 1);

    float4 finalColor = color * luminanceRGB;

    oTexCoord.z = 0;
    oColor = finalColor;
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