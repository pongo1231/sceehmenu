#pragma once
#include "stdafx.h"

class menu;

enum menu_item_type
{
	MENU_ITEM_TYPE_ACTION,
	MENU_ITEM_TYPE_TOGGLE,
	MENU_ITEM_TYPE_SUBMENU,
	MENU_ITEM_TYPE_SLIDER
};

struct menu_item
{
	menu_item(const char *label);
	menu_item(const char *label, bool state);
	menu_item(const char *label, menu *submenu);
	menu_item(const char *label, std::vector<float> values, int default_index = 0);
	menu_item(const menu_item &other);
	menu_item operator =(const menu_item &other);
	~menu_item();

	const char *label;
	const menu_item_type type;
	uint values_index;
	bool hovered = false;
	bool selected = false;
	union
	{
		bool state;
		menu *submenu;
		std::vector<float> values;
	};

	float slider_get_selected_value() const;
};

class menu
{
public:
	const char *title;
	bool visible = false;
	menu *parent;

	menu(const char *title);
	~menu();
	void add_item(menu_item *item);
	void clear_items();
	void tick();
	void handle_keyboard(DWORD key);

private:
	std::vector<menu_item*> menu_items;
	uint cursor;
	uint visibleTime;
	menu_item *selected_item;

	void draw_header() const;
	void draw_items() const;
};

class menu_pool
{
public:
	~menu_pool();
	void add_menu(menu *menu);
	void tick() const;
	void handle_keyboard(DWORD key) const;
	const std::vector<menu*> get_menus() const;

private:
	std::vector<menu*> menus;
};