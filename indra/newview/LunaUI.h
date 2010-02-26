
#ifdef _WITH_CEGUI
/* CEGUI UI Bindings*/
#include <CEGUI.h>
#include <CEGUILua.h>
#include <CEGUIPropertyHelper.h>
#include <RendererModules/OpenGLGUIRenderer/openglrenderer.h>
#endif

// Prototype, WIP, etc
class FlexUI
{
public:
	static void Init();
	static void Cleanup();

// Request new UI elements here.
	static bool RequestWindow(std::string name);
	//etc
};


