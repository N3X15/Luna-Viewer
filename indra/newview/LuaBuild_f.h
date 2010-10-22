#ifndef LUABUILD_F_H
#define LUABUILD_F_H

#include "llviewerobject.h"

const int PCODE_CUBE 		= 1;
const int PCODE_PRISM 		= 2;
const int PCODE_TETRAHEDRON = 3;
const int PCODE_PYRAMID 	= 4;
const int PCODE_CYLINDER 	= 5;
const int PCODE_CONE 		= 6;
const int PCODE_SPHERE 		= 7;
const int PCODE_TORUS		= 8;
const int PCODE_VOLUME		= 9;

//Makes this shit MUCH easier.
class Object
{
private:
	//Primitive to fuck with.
	LLViewerObject *mObject;

	// Waiting for cube.
	static bool mWaiting;

	void plyWood();

	std::vector<LLUUID> mChildren;
public:
	Object(int pcode,bool viewerside=false);
	Object(LLUUID objID);
	~Object(void);

	LLVector3 GetSize() { return mObject->getScale(); }
	void SetSize(		double x, double y, double z);
	
	LLVector3 GetLocation() { return mObject->getPositionRegion(); }
	void SetLocation(	double x, double y, double z);
	
	LLUUID GetTexture(int Face) {return mObject->getTEImage((U8)Face)->getID();}
	void SetTexture(	LLUUID uuid,	int FaceID);

	int GetShape() { return (int)mObject->getPCode(); }
	void SetShape(int pcode);

	bool Commit();
	bool Delete();

	void AddChild(Object child);
	void DeleteChild(LLUUID ID);
	int GetNumChildren();
	Object GetChild(int idx);
	Object GetChild(LLUUID id);

	// Cube waiting
	static LLUUID mReady;
};
#endif //LUABUILD_F_H
