#ifndef LUABUILD_F_H
#define LUABUILD_F_H
#include "llviewerobject.h"

//Determines the shape of the prim.
enum PCode
{
	PCODE_CUBE 			= 1,
	PCODE_PRISM 		= 2,
	PCODE_TETRAHEDRON 	= 3,
	PCODE_PYRAMID 		= 4,
	PCODE_CYLINDER 		= 5,
	PCODE_CONE 			= 6,
	PCODE_SPHERE 		= 7,
	PCODE_TORUS			= 8,
	PCODE_VOLUME		= 9
};

//Makes this shit MUCH easier.
class Object
{
public:
	Object(int pcode,bool viewerside=false);
	Object(LLUUID objID);
	~Object(void);

	void SetSize(		const double x, const double y, const double z);
	void SetLocation(	const double x, const double y, const double z);
	void SetTexture(	const char* uuid,	const U8 FaceID);
	void SetShape(LuaPCode pc);

	bool Commit();

	// Cube waiting
	static LLUUID mReady;
private:
	//Primitive to fuck with.
	LLViewerObject *mObject;

	// Waiting for cube.
	static bool mWaiting;
};
#endif //LUABUILD_F_H
