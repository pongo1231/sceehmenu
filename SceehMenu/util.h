#pragma once
#include "stdafx.h"

namespace util
{
	void draw_notification(const char *text);
	void draw_text(float x, float y, float scale, const char *text, int r = 255, int g = 255, int b = 255);
	void play_sound(const char *sound, const char *set);
};

