#include "stdafx.h"
#include "menu.h"
#include "util.h"

#define MENU_POS_X .2f
#define MENU_POS_Y .2f
#define MENU_WIDTH .25f
#define MENU_HEADER_HEIGHT .1f
#define MENU_ITEM_HEIGHT .05f
#define MENU_MAX_VISIBLE 5

menu_item::menu_item(std::string label) : label(label), type(MENU_ITEM_TYPE_ACTION) {}

menu_item::menu_item(std::string label, bool state) : label(label), state(state), type(MENU_ITEM_TYPE_TOGGLE) {}

menu_item::menu_item(std::string label, menu *submenu) : label(label), submenu(submenu), type(MENU_ITEM_TYPE_SUBMENU) {}

menu_item::menu_item(std::string label, std::vector<float> values, int default_index) : label(label), values(values), values_index(default_index), type(MENU_ITEM_TYPE_SLIDER) {}

menu_item::menu_item(const menu_item &other) : label(other.label), type(other.type)
{
	switch (type)
	{
	case MENU_ITEM_TYPE_TOGGLE:
		state = other.state;
		break;
	case MENU_ITEM_TYPE_SUBMENU:
		submenu = other.submenu;
		break;
	case MENU_ITEM_TYPE_SLIDER:
		values = other.values;
		values_index = other.values_index;
		break;
	}
}

menu_item menu_item::operator =(const menu_item &other)
{
	return other;
}

menu_item::~menu_item()
{

}

float menu_item::slider_get_selected_value() const
{
	return !values.empty() ? values[values_index] : 0.f;
}



menu::menu(std::string title) : title(title) {}

menu::~menu()
{
	for (menu_item *item : menu_items)
	{
		delete(item);
	}
}

void menu::add_item(menu_item *item)
{
	menu_items.push_back(item);
}

void menu::clear_items()
{
	for (menu_item *item : menu_items)
	{
		delete(item);
	}
	menu_items.clear();
}

void menu::tick()
{
	for (menu_item *item : menu_items)
	{
		item->hovered = false;
		item->selected = false;
	}
	if (selected_item)
	{
		selected_item->selected = true;
		selected_item = nullptr;
	}

	if (visible)
	{
		visibleTime++;
		if (!menu_items.empty())
		{
			menu_items[cursor]->hovered = true;
		}
		draw_header();
		draw_items();
	}
	else
		visibleTime = 0;
}

void menu::handle_keyboard(DWORD key)
{
	if (visibleTime < 1) // Don't fetch input from submenu item enter
	{
		return;
	}

	bool handled = true;
	switch (key)
	{
	case VK_UP:
		if (cursor > 0)
		{
			cursor--;
		}
		else
		{
			cursor = menu_items.size() - 1;
		}
		util::play_sound("NAV", "HUD_AMMO_SHOP_SOUNDSET");
		break;
	case VK_DOWN:
		if (cursor < menu_items.size() - 1)
		{
			cursor++;
		}
		else
		{
			cursor = 0;
		}
		util::play_sound("NAV", "HUD_AMMO_SHOP_SOUNDSET");
		break;
	case VK_RETURN:
	{
		menu_item *item = menu_items[cursor];
		switch (item->type)
		{
		case MENU_ITEM_TYPE_TOGGLE:
			item->state = !item->state;
			break;
		case MENU_ITEM_TYPE_SUBMENU:
			item->submenu->parent = this;
			item->submenu->visible = true;
			visible = false;
			break;
		}
		selected_item = item;
		util::play_sound("SELECT", "HUD_FRONTEND_MP_SOUNDSET");
		break;
	}
	case VK_BACK:
		if (parent)
			parent->visible = true;
		visible = false;
		util::play_sound("BACK", "HUD_FRONTEND_MP_SOUNDSET");
		break;
	case VK_LEFT:
	{
		menu_item *item = menu_items[cursor];
		if (item->type == MENU_ITEM_TYPE_SLIDER)
		{
			if (item->values_index > 0)
			{
				item->values_index--;
			}
			else
			{
				item->values_index = (int) item->values.size() - 1;
			}
		}
		break;
	}
	case VK_RIGHT:
	{
		menu_item *item = menu_items[cursor];
		if (item->type == MENU_ITEM_TYPE_SLIDER)
		{
			if (item->values_index < item->values.size() - 1)
			{
				item->values_index++;
			}
			else
			{
				item->values_index = 0;
			}
		}
		break;
	}
	default:
		handled = false;
		break;
	}

	// Disable game input (e.g. open phone)
	if (handled)
	{
		CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(1);
	}
}

void menu::draw_header() const
{
	// Rainbowy
	static ULONG cnt = 0;
	static float freq = .01f;
	GRAPHICS::DRAW_RECT(MENU_POS_X, MENU_POS_Y, MENU_WIDTH, MENU_HEADER_HEIGHT, std::sin(freq * cnt) * 127 + 128, std::sin(freq * cnt + 2) * 127 + 128, std::sin(freq * cnt + 4) * 127 + 128, 230);
	if (++cnt >= (ULONG) - 1)
	{
		cnt = 0;
	}

	util::draw_text(MENU_POS_X, MENU_POS_Y - MENU_HEADER_HEIGHT / 3, 1.5f, title);
	if (parent)
	{
		util::draw_text(MENU_POS_X - MENU_WIDTH / 3, MENU_POS_Y - MENU_HEADER_HEIGHT / 3, 1.5f, "<");
	}
}

void menu::draw_items() const
{
	float new_y = MENU_POS_Y + MENU_HEADER_HEIGHT / 2 + MENU_ITEM_HEIGHT / 2;
	uint items_size = menu_items.size();
	int bottom_dist = items_size - 1 - cursor;
	bool capped_up = cursor > MENU_MAX_VISIBLE;
	bool capped_down = bottom_dist > MENU_MAX_VISIBLE;
	if (capped_up)
	{
		GRAPHICS::DRAW_RECT(MENU_POS_X, new_y, MENU_WIDTH, MENU_ITEM_HEIGHT, 60, 60, 60, 230);
		util::draw_text(MENU_POS_X, new_y - MENU_ITEM_HEIGHT / 3, 1.4f, "^");
		new_y += MENU_ITEM_HEIGHT;
	}
	for (int i = 0; i < items_size; i++)
	{
		if (i < (int) cursor - MENU_MAX_VISIBLE - (!capped_down ? MENU_MAX_VISIBLE - bottom_dist : 0) || i > cursor + MENU_MAX_VISIBLE + (!capped_up ? MENU_MAX_VISIBLE - cursor : 0))
			continue;

		menu_item *item = menu_items[i];

		UCHAR shade = 60;
		if (i == cursor)
		{
			shade = 120;
		}
		GRAPHICS::DRAW_RECT(MENU_POS_X, new_y, MENU_WIDTH, MENU_ITEM_HEIGHT, shade, shade, shade, 230);

		UCHAR r = 255, g = 255, b = 255; // White
		switch (item->type)
		{
		case MENU_ITEM_TYPE_TOGGLE:
			if (item->state)
			{
				r = 100, g = 255, b = 100; // Green
			}
			else
			{
				r = 255, g = 100, b = 100; // Red
			}
			break;
		case MENU_ITEM_TYPE_SUBMENU:
			util::draw_text(MENU_POS_X + MENU_WIDTH / 3, new_y - MENU_ITEM_HEIGHT / 3, 1.f, ">>", r, g, b);
			break;
		case MENU_ITEM_TYPE_SLIDER:
			char buffer[32];
			sprintf_s(buffer, "< %.1f >", item->slider_get_selected_value());
			util::draw_text(MENU_POS_X + MENU_WIDTH / 3, new_y - MENU_ITEM_HEIGHT / 3, 1.f, buffer, 100, 100, 255);
			break;
		}
		util::draw_text(MENU_POS_X, new_y - MENU_ITEM_HEIGHT / 3, 1.f, item->label, r, g, b);

		new_y += MENU_ITEM_HEIGHT;
	}
	if (capped_down)
	{
		GRAPHICS::DRAW_RECT(MENU_POS_X, new_y, MENU_WIDTH, MENU_ITEM_HEIGHT, 60, 60, 60, 230);
		util::draw_text(MENU_POS_X, new_y - MENU_ITEM_HEIGHT / 3, 1.f, "v");
	}
}



menu_pool::~menu_pool()
{
	for (menu *menu : menus)
	{
		delete(menu);
	}
}

void menu_pool::add_menu(menu *menu)
{
	menus.push_back(menu);
}

void menu_pool::tick() const
{
	for (menu *menu : menus)
	{
		menu->tick();
	}
}

void menu_pool::handle_keyboard(DWORD key) const
{
	for (menu *menu : menus)
	{
		menu->handle_keyboard(key);
	}
}

const std::vector<menu*> menu_pool::get_menus() const
{
	return menus;
}