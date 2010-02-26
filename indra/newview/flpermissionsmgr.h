/**
* Luna Permissions Manager
*
* 
*/

// TRY to use simple, Lua-compatible types.
// If you cannot, provide a toString/toDouble function or something.
class LunaPermissionsMgr
{
public:
	// Aggregate permissions
	std::map<LLUUID,LLAggregatePerms> mPerms;

	// Aggregate texture permissions
	std::map<LLUUID,LLAggregatePerms> mTexturePerms;


};

