#pragma once
#include "stdafx.h"

class hooking
{
public:
	hooking();
	void set_player_speed(float speed);

private:
	DWORD scripthookaddr;

	uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
};

