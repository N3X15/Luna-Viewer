#ifndef LUATYPES_H
#define LUATYPES_H

#include "llviewerprecompiledheaders.h"
#include "v2math.h";
#include "v3math.h";
#include "v4math.h";

class LuaVector2
{
public:
	float x;
	float y;
	
	LuaVector2& operator=(const LLVector2& in)
	{
		x=in.mV[VX];
		y=in.mV[VY];
	}

	LuaVector2();				// (0,0)
	LuaVector2(float nx,float ny);		// (nx,ny)
	LuaVector2(const LLVector2& val);	// You get the idea

	operator LLVector2()
	{ 
		return new LLVector2(x,y); 
	}
}

inline LuaVector2::LuaVector2()
{
	x=0f;
	y=0f;
}

inline LuaVector2::LuaVector2(float nx,float nz)
{
	x=nx;
	y=ny;
}

inline LuaVector2::LuaVector2(const LLVector2& val)
{
	x=val.mV[VX];
	y=val.mV[VY];
}

class LuaVector3
{
public:
	float x;
	float y;
	float z;

	LuaVector3& operator=(const LLVector3& in)
	{
		x=in.mV[VX];
		y=in.mV[VY];
		z=in.mV[VZ];
	}

	LuaVector3();					// (0, 0, 0)
	LuaVector3(float nx,float ny,float nz);		// (nx,ny,nz)
	LuaVector3(const LLVector3& val);		// You get the idea

	operator LLVector3()
	{ 
		return new LLVector3(x,y,z); 
	}
}
inline LuaVector3::LuaVector3()
{
	x=0f;
	y=0f;
	z=0f;
}

inline LuaVector3::LuaVector3(float nx,float ny,float nz)
{
	x=nx;
	y=ny;
	z=nz;
}

inline LuaVector2::LuaVector3(const LLVector3& val)
{
	x=val.mV[VX];
	y=val.mV[VY];
	z=val.mV[VZ];
}

class LuaVector4
{
public:
	float x;
	float y;
	float z;
	float w;
	LuaVector4& operator=(const LLVector4& in)
	{
		x=in.mV[VX];
		y=in.mV[VY];
		z=in.mV[VZ];
		w=in.mV[VW];
	}

	LuaVector4();							// (0, 0, 0, 0)
	LuaVector4(float nx,float ny,float nz,float nw);		// (nx,ny,nz,nw)
	LuaVector4(const LLVector4& val);				// You get the idea

	operator LLVector4()
	{ 
		return new LLVector4(x,y,z,w); 
	}

	operator LLColor4()
	{ 
		return new LLVector4(x,y,z,w); 
	}
}
inline LuaVector4::LuaVector4()
{
	x=0f;
	y=0f;
	z=0f;
	w=0f;
}

inline LuaVector4::LuaVector4(float nx,float ny,float nz,float nw)
{
	x=nx;
	y=ny;
	z=nz;
	w=nw;
}

inline LuaVector4::LuaVector4(const LLVector4& val)
{
	x=val.mV[VX];
	y=val.mV[VY];
	z=val.mV[VZ];
	w=val.mV[VW];
}

#endif
