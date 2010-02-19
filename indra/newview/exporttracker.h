/* Copyright (c) 2009
 *
 * Modular Systems All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *   3. Neither the name Modular Systems nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MODULAR SYSTEMS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "llagent.h"
#include "llvoinventorylistener.h"


class JCExportTracker : public LLVOInventoryListener
{
public:
	JCExportTracker();
	~JCExportTracker();

private:
	static JCExportTracker* sInstance;
	static void init();
	static LLSD* getprim(LLUUID id);
	static void completechk();
public:
	static void processObjectProperties(LLMessageSystem* msg, void** user_data);
	void inventoryChanged(LLViewerObject* obj,
								 InventoryObjectList* inv,
								 S32 serial_num,
								 void* data);
	static void			onFileLoadedForSave( 
							BOOL success,
							LLViewerImage *src_vi,
							LLImageRaw* src, 
							LLImageRaw* aux_src,
							S32 discard_level, 
							BOOL final,
							void* userdata );


	static JCExportTracker* getInstance(){ init(); return sInstance; }

	static bool serialize(LLDynamicArray<LLViewerObject*> objects);
	static bool serializeSelection();
	static void finalize(LLSD data);

	static BOOL mirror(LLInventoryObject* item, LLViewerObject* container = NULL, std::string root = "", std::string iname = "");

private:
	static LLSD subserialize(LLViewerObject* linkset);

	enum ExportState { IDLE, EXPORTING };

	static U32 status;

	//enum ExportLevel { DEFAULT, PROPERTIES, INVENTORY };

	static BOOL export_properties;
	static BOOL export_inventory;
	static BOOL export_textures;

	//static U32 level;

	static U32 propertyqueries;
	static U32 invqueries;
	static U32 totalprims;
	static LLSD data;

	static std::string destination;
	static std::string asset_dir;
	static std::set<LLUUID> requested_textures;
};


