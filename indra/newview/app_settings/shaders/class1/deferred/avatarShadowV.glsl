/** 
 * @file avatarShadowV.glsl
 *
 * Copyright (c) 2007-$CurrentYear$, Linden Research, Inc.
 * $License$
 */

mat4 getSkinnedTransform();

attribute vec4 weight;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
				
	vec4 pos;
	vec3 norm;
	
	mat4 trans = getSkinnedTransform();
	pos.x = dot(trans[0], gl_Vertex);
	pos.y = dot(trans[1], gl_Vertex);
	pos.z = dot(trans[2], gl_Vertex);
	pos.w = 1.0;
	
	norm.x = dot(trans[0].xyz, gl_Normal);
	norm.y = dot(trans[1].xyz, gl_Normal);
	norm.z = dot(trans[2].xyz, gl_Normal);
	norm = normalize(norm);
	
	pos = gl_ProjectionMatrix * pos;
	//smash geometry against near clip plane
	pos.z = max(pos.z, -1.0);
	gl_Position = pos;
	
	gl_FrontColor = gl_Color;
}


