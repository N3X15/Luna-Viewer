Texture2D gDiffuseMap;
uniform float4 intensity; 
float4x4 gMVP;

SamplerState gTriLinearSamWrap
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

void VS(float3 iPosL : POSITION,
        float4 iColor : COLOR,
        float3 iTexCoord : TEXCOORD,

        out float4 oPosH : SV_POSITION,
        out float4 oColor : COLOR,
        out float3 oTexCoord : TEXCOORD )
{
	oPosH = mul(float4(iPosL, 1.0f), gMVP);
	oColor = iColor;
	oTexCoord = iTexCoord;
}

float4 PS(float4 posH : SV_POSITION,
		  float4 color : COLOR,
		  float3 texCoord : TEXCOORD ) : SV_TARGET
{
	float4 oColor;
	
	float3 fragIn = gDiffuseMap.Sample(gTriLinearSamWrap, texCoord).rgb;
    oColor.rgb = intensity.xyz * fragIn * color.rgb;
    oColor.a = 1.0;
    
    return oColor;
}

technique
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_2_0, VS() ) );
		SetPixelShader( CompileShader( ps_2_0, PS() ) );
	}
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