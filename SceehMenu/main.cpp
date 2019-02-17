#include "stdafx.h"
#include "script.h"

BOOL APIENTRY DllMain(HMODULE hinstance, DWORD reason, LPVOID lpreserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		scriptRegister(hinstance, script_main);
		keyboardHandlerRegister(on_key);
		break;
	case DLL_PROCESS_DETACH:
		scriptUnregister(hinstance);
		keyboardHandlerUnregister(on_key);
		break;
	}		
	return TRUE;
}