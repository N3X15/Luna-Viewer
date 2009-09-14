			
  uniform float4x4 modelViewProj;
  uniform float4x4 shadowXform;
  uniform float3 fadeDistVec;
  
  void VS(  float4 position     : POSITION,
			float4 color        : COLOR,
			float3 texCoord     : TEXCOORD,
			
       	out float4 oPosition    : SV_POSITION,
		out float4 oColor       : COLOR,
		out float3 oTexCoord	: TEXCOORD
  )
  
  {
  		oPosition = mul(modelViewProj, position);
  		oTexCoord = texCoord;
  		
  		float4 shadowPos = mul(position, shadowXform);
  		
  		float d = texCoord.z;
		float4 density = float4(d, d, d, 1.0);

		float fade = fadeDistVec.y;
		if (shadowPos.y <= 0) 
		{
			fade = 0.0;
			
			float fadeDist = -fadeDistVec.x;

			if (shadowPos.y > fadeDist)
			{
				fade = shadowPos.y / fadeDist;
				fade = 1.0 - fade;
			}
		}

		float4 fadeColor = float4(fade, fade, fade, 1.0);
		float4 baseColor = float4(0.5, 0.5, 1.0, 1.0);

		oColor = baseColor * density * fadeColor;
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