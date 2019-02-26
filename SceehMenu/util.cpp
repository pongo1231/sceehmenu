#include "stdafx.h"
#include "util.h"

namespace util
{
	void draw_notification(std::string text)
	{
		UI::_SET_NOTIFICATION_TEXT_ENTRY((char*) "STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((char*) text.c_str());
		UI::_DRAW_NOTIFICATION(false, false);
	}

	void draw_text(float x, float y, float scale, std::string text, int r, int g, int b)
	{
		UI::_SET_TEXT_ENTRY((char*) "STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((char*) text.c_str());
		UI::SET_TEXT_SCALE(.0f, .55f * scale);
		UI::SET_TEXT_COLOUR(r, g, b, 255);
		UI::SET_TEXT_FONT(4);
		UI::SET_TEXT_OUTLINE();
		UI::SET_TEXT_DROP_SHADOW();
		UI::SET_TEXT_PROPORTIONAL(true);
		UI::SET_TEXT_CENTRE(true);
		UI::_DRAW_TEXT(x, y);
	}

	void play_sound(const char *sound, const char *set)
	{
		//AUDIO::PLAY_SOUND_FRONTEND(-1, (char*) sound, (char*) set, false);
	}
}