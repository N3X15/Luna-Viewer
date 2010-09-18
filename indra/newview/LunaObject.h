#include "llviewerobject.h"
#include "llviewerobjectlist.h"
#include "stdtypes.h"

inline LLViewerObject	*FindObject(const LLUUID &id) { return gObjectList.findObject(id);}
inline LLViewerObject	*CreateViewerObject(const int pcode,LLViewerRegion *regionp){return gObjectList.createObjectViewer((LLPCode)pcode,regionp);}
inline bool 		KillObject(LLViewerObject *obj) {return gObjectList.killObject(obj);}

//LLUUID		Local2UUID(const LLUUID &id,const int local_id,const int ip,const int port); // I dunno
