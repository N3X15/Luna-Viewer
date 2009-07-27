/** 
 * @file terrainF.glsl
 *
 * Copyright (c) 2007-$CurrentYear$, Linden Research, Inc.
 * $License$
 */

uniform sampler2D detail_0;
uniform sampler2D detail_1;
uniform sampler2D detail_2;
uniform sampler2D detail_3;
uniform sampler2D alpha_ramp;

varying vec3 vary_normal;
varying vec4 vary_position;

void main()
{
	/// Note: This should duplicate the blending functionality currently used for the terrain rendering.
	
	vec4 color0 = texture2D(detail_0, gl_TexCoord[0].xy);
	vec4 color1 = texture2D(detail_1, gl_TexCoord[0].xy);
	vec4 color2 = texture2D(detail_2, gl_TexCoord[0].xy);
	vec4 color3 = texture2D(detail_3, gl_TexCoord[0].xy);

	float alpha1 = texture2D(alpha_ramp, gl_TexCoord[0].zw).a;
	float alpha2 = texture2D(alpha_ramp,gl_TexCoord[1].xy).a;
	float alphaFinal = texture2D(alpha_ramp, gl_TexCoord[1].zw).a;
	vec4 outColor = mix( mix(color3, color2, alpha2), mix(color1, color0, alpha1), alphaFinal );
	
	gl_FragData[0] = vec4(outColor.rgb, 1.0);
	gl_FragData[1] = vec4(outColor.rgb*0.2, 0.2);
	gl_FragData[2] = vec4(normalize(vary_normal), 0.0);
	gl_FragData[3] = vary_position;
}

