#include "stdafx.h"
#include "hooking.h"

hooking::hooking()
{
	/*HMODULE module = GetModuleHandle(nullptr);
	DWORD id = GetProcessId(module);
	scripthookaddr = GetModuleBaseAddress(id, (const wchar_t*) "ScriptHookV.dll");
	char buffer[64];
	sprintf_s(buffer, "%d", scripthookaddr);
	MessageBox(0, "", buffer, MB_OK);*/
}

void hooking::set_player_speed(float speed)
{
	//*(float*) (scripthookaddr + 0x129508) = speed;
	/*char buffer[64];
	sprintf_s(buffer, "%f", *(float*) (scripthookaddr + 0x129508));
	MessageBox(0, "", buffer, MB_OK);*/
}

uintptr_t hooking::GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp((const wchar_t*) modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t) modEntry.modBaseAddr;
					break;
				}
			}
			while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}