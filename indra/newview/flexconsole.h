#pragma once
#include "llfloater.h"
#include "llthread.h"
#include "lllineeditor.h"
//#include "tslua.h"

#include <queue>

/**
 * TODO:  Copy paste TSStuff license here.
 */
class LLFloaterLuaConsole : public LLFloater
{
public:
        LLFloaterLuaConsole();
        ~LLFloaterLuaConsole();

        /*virtual*/ BOOL postBuild();
        /*virtual*/ void draw();
        /*virtual*/ void onClose(bool app_quitting = false);

        static LLFloaterLuaConsole* getInstance();
        static void show(void*);

        static bool isVisible() { if(sInstance) return (sInstance->getVisible() != FALSE); return false; }
        static void toggle(void*);

private:
        static void onClickSend(void*);
        static void onClickClear(void*);
        static void onClickAbort(void*);
        static void onClickReset(void*);

private:
        static LLFloaterLuaConsole *sInstance;
};

