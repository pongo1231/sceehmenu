#include "stdafx.h"
#include "hooking.h"

namespace hooking
{
	uintptr_t gtabase;
	uintptr_t gtasize;
	uintptr_t gtaend;
	float* player_speedmtp;
	void (*player_kick)(int, int*, int, int);

	void* pattern_scan(const char *pattern, const char *mask)
	{
		uint i = 0;
		for (uintptr_t addr = gtabase; addr < gtaend - strlen(mask); addr++)
		{
			if (mask[i] == '?' || pattern[i] == *((char*) addr))
			{
				i++;
			}
			else
			{
				i = 0;
			}

			if (i == strlen(mask))
			{
				return (void*) addr;
			}
		}

		return 0;
	}

	void init_hooking()
	{
		MODULEINFO minfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle("GTA5.exe"), &minfo, sizeof(MODULEINFO));
		gtabase = (uintptr_t) minfo.lpBaseOfDll;
		gtasize = minfo.SizeOfImage;
		gtaend = ((uintptr_t) minfo.lpBaseOfDll + minfo.SizeOfImage);

		/*char buffer[128];
		player_speedmtp = (float*) pattern_scan("\x80\x3F\x11\x73\x15", "xxxxx") - 7;
		sprintf_s(buffer, "%u", player_speedmtp);
		MessageBox(0, buffer, "", MB_OK);
		player_kick = (void (*)(int, int*, int, int)) pattern_scan("\x48\x8B\xC4\x48\x89\x58\x08\x48\x89\x68\x10\x48\x89\x70\x18\x48\x89\x78\x20\x41\x56\x48\x81\xEC\x00\x00\x00\x00\x45\x8B\xF0\x41\x8B\xF9", "xxxxxxxxxxxxxxxxxxxxxxxx????xxxxxx");
		sprintf_s(buffer, "%u", player_kick);
		MessageBox(0, buffer, "", MB_OK);*/
	}

	void set_player_speedmtp(float value)
	{
		//*(float*) player_speedmtp = value;
	}

	void kick_player(Player player)
	{
		/*int args[3] = { -120668417, player, 48 };
		(*player_kick)(1, args, 3, (player << 1));*/
	}
}