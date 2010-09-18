
// This will cause errors on Windows because Visual Studio is a retard
#include "llviewerprecompiledheaders.h"

#include "LunaGL.h"

#include "llrender.h"


// Refreshes renderer state to the cached values
// Needed when the render context has changed and invalidated the current state
void refreshState()
{
	gGL.refreshState();
}

void translatef(const GLfloat& x, const GLfloat& y, const GLfloat& z)
{
	gGL.translatef(x,y,z);
}
void scalef(	const GLfloat& x, const GLfloat& y, const GLfloat& z)
{
	gGL.scalef(x,y,z);
}

// Push a matrix onto the stack?
void pushMatrix() { gGL.pushMatrix(); }
void popMatrix()  { gGL.popMatrix(); }

// Presumably flush a buffer
void flush()
{
	gGL.flush();
}

void begin(const int mode)
{
	gGL.begin(mode);
}

void end() {gGL.end();}

void vertex2i(const int& x, const int& y) 						{gGL.vertex2i(x,y);}
void vertex2f(const GLfloat& x, const GLfloat& y) 					{gGL.vertex2f(x,y);}
void vertex3f(const GLfloat& x, const GLfloat& y, const GLfloat& z) 			{gGL.vertex3f(x,y,z);}
void vertex2fv(const GLfloat* v) 							{gGL.vertex2fv(v);}
void vertex3fv(const GLfloat* v) 							{gGL.vertex3fv(v);}

void texCoord2i(const int& x, const int& y) 						{gGL.texCoord2i(x,y);}
void texCoord2f(const GLfloat& x, const GLfloat& y)					{gGL.texCoord2f(x,y);}
void texCoord2fv(const GLfloat* tc)							{gGL.texCoord2fv(tc);}

void color4ub(const int& r, const int& g, const int& b, const int& a) 			{gGL.color4ub(r,g,b,a);}
void color4f(const GLfloat& r, const GLfloat& g, const GLfloat& b, const GLfloat& a) 	{gGL.color4f(r,g,b,a); }
void color4fv(const GLfloat* c) 							{gGL.color4fv(c); }
void color3f(const GLfloat& r, const GLfloat& g, const GLfloat& b) 			{gGL.color3f(r,g,b); }
void color3fv(const GLfloat* c) 							{gGL.color3fv(c); }
void color4ubv(const GLubyte* c)  							{gGL.color4ubv(c); }

void setColorMask(bool writeColor, bool writeAlpha) 					{gGL.setColorMask(writeColor,writeAlpha); }
void setColorMask(bool writeColorR, bool writeColorG, bool writeColorB, bool writeAlpha) 
											{gGL.setColorMask(writeColorR,writeColorG,writeColorB,writeAlpha); }

void setSceneBlendType(BlendType type) 							{gGL.setSceneBlendType((LLRender::eBlendType)type);}

void setAlphaRejectSettings(CompareFunc func, float value)	 			{gGL.setAlphaRejectSettings((LLRender::eCompareFunc)func,value); }

void blendFunc(BlendFactor sfactor, BlendFactor dfactor) 				{gGL.blendFunc((LLRender::eBlendFactor)sfactor,(LLRender::eBlendFactor)dfactor);}

LLTexUnit* getTexUnit(int index) 							{ return gGL.getTexUnit((U32)index); }

int getCurrentTexUnitIndex() 								{ return gGL.getCurrentTexUnitIndex(); }

bool verifyTexUnitActive(int unitToVerify) 						{ return gGL.verifyTexUnitActive(unitToVerify);}

void debugTexUnits() 									{ gGL.debugTexUnits(); }

void clearErrors() 									{ gGL.clearErrors(); }
