uniform float4x4 modelView;
uniform float4x4 modelViewProj;
uniform float4x4 billboard;
uniform float4   flipTCoordsAndFogDensity;
uniform float4   fogColor;
uniform float4   billboardColor;
uniform float4   fade;

Texture2D gDiffuseMap;

SamplerState gTriLinearSamClamp
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = WRAP;
};

void VS(  float4 position     : POSITION,
			float4 color        : COLOR,
			float3 texCoord     : TEXCOORD,
			
       	out float4 oPosition    : SV_POSITION,
		out float4 oColor       : COLOR,
        out float3 oTexCoord    : TEXCOORD
)
{
    float4x4 xlate = float4x4(1, 0, 0, position.x,
                              0, 1, 0, position.y,
                              0, 0, 1, position.z,
                              0, 0, 0, 1);
                              
	float3 eyePosition = mul(modelView, position).xyz;
	float fogDistance = length(eyePosition);
	float fogExponent = fogDistance * flipTCoordsAndFogDensity.y;
	float fogFactor = exp(-abs(fogExponent));
	float4 preFadedColor;
	
	if (billboardColor.w > 0)
	{
		preFadedColor.xyz = lerp(fogColor.xyz, billboardColor.xyz, fogFactor);
		preFadedColor.w = billboardColor.w;
	}
	else
	{
		preFadedColor.xyz = lerp(fogColor.xyz, color.xyz, fogFactor);
		preFadedColor.w = color.w;
	}
	
	oColor = lerp(float4(0, 0, 0, 0), preFadedColor, fade);
	
    float size = texCoord.x;
    
    float4 vertPos;
    float3 uv;
    
    if (texCoord.y == 1)
    {
		vertPos = float4(size * -1, size * 1, 0, 1);
		uv = float3(-1, 1, 0);
	}
	else if (texCoord.y == 2)
	{
		vertPos = float4(size * -1, size * -1, 0, 1);
		uv = float3(-1, -1, 0);
	}
	else if (texCoord.y == 3)
	{
		vertPos = float4(size * 1, size * -1, 0, 1);
		uv = float3(1, -1, 0);
	}
	else 
	{
		vertPos = float4(size, size, 0, 1);
		uv = float3(1, 1, 0);
	}

	float4 rotatedPos = vertPos;
	
	float angle = texCoord.z;
	float cosLength, sinLength;
	sincos(angle, sinLength, cosLength);
	rotatedPos.x = cosLength * vertPos.x + -sinLength * vertPos.y;
	rotatedPos.y = sinLength * vertPos.x + cosLength * vertPos.y;
        
    oTexCoord = 0.5 + (uv * 0.5);
	oTexCoord.y *= flipTCoordsAndFogDensity.x;
	
    float4x4 xlateMat = mul(modelViewProj, xlate);
    
    oPosition = mul(xlateMat, mul(billboard, rotatedPos));
}

float4 PS(float4 posH : SV_POSITION,
		  float4 color : COLOR,
		  float3 texCoord : TEXCOORD ) : SV_TARGET
{
	float4 fragIn;
	fragIn = gDiffuseMap.Sample(gTriLinearSamClamp, texCoord);
	
	return fragIn * color;
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