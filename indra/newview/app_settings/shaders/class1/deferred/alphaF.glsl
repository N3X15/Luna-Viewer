/** 
 * @file alphaF.glsl
 *
 * Copyright (c) 2007-$CurrentYear$, Linden Research, Inc.
 * $License$
 */

#extension GL_ARB_texture_rectangle : enable

uniform sampler2D diffuseMap;
uniform sampler2DShadow shadowMap0;
uniform sampler2DShadow shadowMap1;
uniform sampler2DShadow shadowMap2;
uniform sampler2DShadow shadowMap3;
uniform sampler2D noiseMap;
uniform sampler2DRect positionMap;

uniform mat4 shadow_matrix[4];
uniform vec4 shadow_clip;
uniform vec2 screen_res;

vec3 atmosLighting(vec3 light);
vec3 scaleSoftClip(vec3 light);

varying vec3 vary_ambient;
varying vec3 vary_directional;
varying vec3 vary_fragcoord;
varying vec3 vary_position;

uniform float alpha_soften;

void main() 
{
	vec2 frag = vary_fragcoord.xy/vary_fragcoord.z*0.5+0.5;
	frag *= screen_res;
	
	vec3 samp_pos = texture2DRect(positionMap, frag).xyz;
	
	float shadow = 1.0;
	vec4 pos = vec4(vary_position, 1.0);
	
	if (pos.z > -shadow_clip.w)
	{	
		if (pos.z < -shadow_clip.z)
		{
			vec4 lpos = shadow_matrix[3]*pos;
			shadow = shadow2DProj(shadowMap3, lpos).x;
			shadow += max((pos.z+shadow_clip.z)/(shadow_clip.z-shadow_clip.w)*2.0-1.0, 0.0);
		}
		else if (pos.z < -shadow_clip.y)
		{
			vec4 lpos = shadow_matrix[2]*pos;
			shadow = shadow2DProj(shadowMap2, lpos).x;
		}
		else if (pos.z < -shadow_clip.x)
		{
			vec4 lpos = shadow_matrix[1]*pos;
			shadow = shadow2DProj(shadowMap1, lpos).x;
		}
		else
		{
			vec4 lpos = shadow_matrix[0]*pos;
			shadow = shadow2DProj(shadowMap0, lpos).x;
		}
	}
	
	vec4 col = vec4(vary_ambient + vary_directional.rgb*shadow, gl_Color.a);
	vec4 color = texture2D(diffuseMap, gl_TexCoord[0].xy) * col;
	
	color.rgb = atmosLighting(color.rgb);

	color.rgb = scaleSoftClip(color.rgb);

	if (samp_pos.z != 0.0)
	{
		float dist_factor = alpha_soften;
		float a = gl_Color.a;
		a *= a;
		dist_factor *= 1.0/(1.0-a);
		color.a *= min((pos.z-samp_pos.z)*dist_factor, 1.0);
	}
	
	//gl_FragColor = gl_Color;
	gl_FragColor = color;
	//gl_FragColor = vec4(1,0,1,1);
	
}

