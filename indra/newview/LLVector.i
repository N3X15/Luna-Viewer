
%{
/**
*  Lua(Swig) <-> LuaVector* <-> LLLVector*
*/
#include "llviewerprecompiledheaders.h"
#include "v2math.h"
#include "v3math.h"
#include "v4math.h"
#include "v4color.h"
%}

// Unimplemented crap
%ignore LLVector4::rotVec(F32, LLVector4 const&);
%ignore LLVector4::rotVec(F32, F32, F32, F32);

%include "../llmath/v2math.h"
%include "../llmath/v3math.h"
%include "../llmath/v4math.h"
%include "../llmath/v4color.h"

/////////////////////////////////////////////////////////
// LLVector2 Extensions
/////////////////////////////////////////////////////////

%extend LLVector2
{	
	float x;
	float y;

	std::string __str__()
	{
		std::ostringstream s;
		s << "( " << $self->mV[0] << ", " << $self->mV[1] << " )";
		return s.str();
	}

};

%{
float LLVector2_x_get(LLVector2 *v)
{
	return v->mV[VX];
}

void LLVector2_x_set(LLVector2 *v,float val)
{
	v->mV[VX]=val;
}

float LLVector2_y_get(LLVector2 *v)
{
	return v->mV[VY];
}

void LLVector2_y_set(LLVector2 *v,float val)
{
	v->mV[VY]=val;
}
%}
/////////////////////////////////////////////////////////
// LLVector3 Extensions
/////////////////////////////////////////////////////////

%extend LLVector3
{
	float x;
	float y;
	float z;
	
	float r;
	float g;
	float b;

	std::string __str__()
	{
		std::ostringstream s;
		s << "< " << $self->mV[0] << ", " << $self->mV[1] << ", " << $self->mV[2] << " >";
		return s.str();
	}

};

%{
// Accessors for LLVector3::x
float LLVector3_x_get(LLVector3 *v)
{
	return v->mV[VX];
}

void LLVector3_x_set(LLVector3 *v,float val)
{
	v->mV[VX]=val;
}

// Accessors for LLVector3::r
float LLVector3_r_get(LLVector3 *v)
{
	return v->mV[VX];
}

void LLVector3_r_set(LLVector3 *v,float val)
{
	v->mV[VX]=val;
}

// Accessors for LLVector3::y
float LLVector3_y_get(LLVector3 *v)
{
	return v->mV[VY];
}

void LLVector3_y_set(LLVector3 *v,float val)
{
	v->mV[VY]=val;
}

// Accessors for LLVector3::g
float LLVector3_g_get(LLVector3 *v)
{
	return v->mV[VY];
}

void LLVector3_g_set(LLVector3 *v,float val)
{
	v->mV[VY]=val;
}

// Accessors for LLVector3::z
float LLVector3_z_get(LLVector3 *v)
{
	return v->mV[VZ];
}

void LLVector3_z_set(LLVector3 *v,float val)
{
	v->mV[VZ]=val;
}

// Accessors for LLVector3::b
float LLVector3_b_get(LLVector3 *v)
{
	return v->mV[VZ];
}

void LLVector3_b_set(LLVector3 *v,float val)
{
	v->mV[VZ]=val;
}
%}

/////////////////////////////////////////////////////////
// LLVector4 Extensions
/////////////////////////////////////////////////////////

%extend LLVector4
{
	float x;
	float y;
	float z;
	float w;

	float r;
	float g;
	float b;
	float a;

	std::string __str__()
	{
		std::ostringstream s;
		s << "< " << $self->mV[0] << ", " << $self->mV[1] << ", " << $self->mV[2] << ", " << $self->mV[3] << " >";
		return s.str();
	}
};

%{
// Accessors for LLVector4::x
float LLVector4_x_get(LLVector4 *v)
{
	return v->mV[VX];
}

void LLVector4_x_set(LLVector4 *v,float val)
{
	v->mV[VX]=val;
}

// Accessors for LLVector4::r
float LLVector4_r_get(LLVector4 *v)
{
	return v->mV[VX];
}

void LLVector4_r_set(LLVector4 *v,float val)
{
	v->mV[VX]=val;
}

// Accessors for LLVector4::y
float LLVector4_y_get(LLVector4 *v)
{
	return v->mV[VY];
}

void LLVector4_y_set(LLVector4 *v,float val)
{
	v->mV[VY]=val;
}

// Accessors for LLVector4::g
float LLVector4_g_get(LLVector4 *v)
{
	return v->mV[VY];
}

void LLVector4_g_set(LLVector4 *v,float val)
{
	v->mV[VY]=val;
}

// Accessors for LLVector4::z
float LLVector4_z_get(LLVector4 *v)
{
	return v->mV[VZ];
}

void LLVector4_z_set(LLVector4 *v,float val)
{
	v->mV[VZ]=val;
}

// Accessors for LLVector4::b
float LLVector4_b_get(LLVector4 *v)
{
	return v->mV[VZ];
}

void LLVector4_b_set(LLVector4 *v,float val)
{
	v->mV[VZ]=val;
}

// Accessors for LLVector4::w
float LLVector4_w_get(LLVector4 *v)
{
	return v->mV[VW];
}

void LLVector4_w_set(LLVector4 *v,float val)
{
	v->mV[VW]=val;
}

// Accessors for LLVector4::a
float LLVector4_a_get(LLVector4 *v)
{
	return v->mV[VW];
}

void LLVector4_a_set(LLVector4 *v,float val)
{
	v->mV[VW]=val;
}
%}

