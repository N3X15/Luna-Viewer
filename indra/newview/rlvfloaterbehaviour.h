#ifndef RLV_FLOATER_BEHAVIOUR
#define RLV_FLOATER_BEHAVIOUR

#include "llfloater.h"
#include "rlvevent.h"

// ============================================================================

class RlvFloaterBehaviour :
	public LLFloater,
	public LLFloaterSingleton<RlvFloaterBehaviour>,
	public RlvBehaviourObserver
{
	friend class LLUISingleton<RlvFloaterBehaviour, VisibilityPolicy<LLFloater> >;
public:
	virtual ~RlvFloaterBehaviour() {}

	/*
	 * LLFloater overrides
	 */
public:
	/*virtual*/ BOOL canClose();
	/*virtual*/ void onOpen();
	/*virtual*/ void onClose(bool app_quitting);
	/*virtual*/ BOOL postBuild();

	/*
	 * RlvBehaviourObserver overrides
	 */
public:
	/*virtual*/ void changed();

public:
	static void show(void*);
	static void onNameLookup();
protected:
	void refreshAll();
private:
	RlvFloaterBehaviour(const LLSD& key = LLSD());
};

// ============================================================================

#endif // RLV_FLOATER_BEHAVIOUR
