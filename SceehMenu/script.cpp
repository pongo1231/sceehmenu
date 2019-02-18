#include "stdafx.h"
#include "script.h"
#include "util.h"
#include "hooking.h"
#include "vehicle_list.h"

hooking mhooking;
menu_pool pool;

menu *main_menu = new menu("SceehMenu");

menu *online_menu = new menu("Online");
menu *playerlist_menu = new menu("Players");

menu *player_menu = new menu("Player");
menu_item *player_godmode = new menu_item("Godmode", false);
menu_item *player_invisible = new menu_item("Invisibility", false);
menu_item *player_speedmultiplier = new menu_item("Speed Multiplier", std::vector<float>({ 1.f, 1.1f, 1.2f, 1.3f, 1.4f, 1.49f }));

menu *vehicle_menu = new menu("Vehicle");
menu *vehicle_spawn_menu = new menu("Spawn Vehicle");
std::vector<menu_item*> vehicle_spawn_items;
menu *vehicle_color_menu = new menu("Vehicle Color");
menu_item *vehicle_color_red;
menu_item *vehicle_color_green;
menu_item *vehicle_color_blue;
menu_item *vehicle_invincible = new menu_item("Invincible", false);

menu *world_menu = new menu("World");
menu *world_time_menu = new menu("Time");
menu_item *time_hour = new menu_item("Hour", std::vector<float>({ 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f, 17.f, 18.f, 19.f, 20.f, 21.f, 22.f, 23.f }));
menu_item *time_minute;
menu_item *time_second;
menu_item *time_apply = new menu_item("Apply Time");
menu_item *time_freeze = new menu_item("Freeze Time", false);

menu *misc_menu = new menu("Misc");
menu *misc_settings_menu = new menu("Settings");
menu_item *settings_selectlastvehicle = new menu_item("Select Last Vehicle", false);

static void playerlist_update()
{
	if (!playerlist_menu->visible)
	{
		return;
	}

	playerlist_menu->clear_items();
	for (Player player = 0; player < 30; player++)
	{
		if (!NETWORK::NETWORK_IS_PLAYER_ACTIVE(player))
		{
			continue;
		}

		playerlist_menu->add_item(new menu_item(PLAYER::GET_PLAYER_NAME(player)));
	}
}

void set_time()
{
	NETWORK::NETWORK_OVERRIDE_CLOCK_TIME((int) time_hour->slider_get_selected_value(), (int) time_minute->slider_get_selected_value(), (int) time_second->slider_get_selected_value());
}

void spawn_vehicle(int index)
{
	Hash veh_hash = vehicle_list[index];
	STREAMING::REQUEST_MODEL(veh_hash);
	while (!STREAMING::HAS_MODEL_LOADED(veh_hash))
	{
		WAIT(0);
	}
	Ped ped = PLAYER::PLAYER_PED_ID();
	float ped_heading = ENTITY::GET_ENTITY_HEADING(ped);
	Vector3 spawn_pos = ENTITY::GET_ENTITY_COORDS(ped, true);
	Vehicle veh = VEHICLE::CREATE_VEHICLE(veh_hash, spawn_pos.x, spawn_pos.y, spawn_pos.z, ped_heading, false, true);
	NETWORK::VEH_TO_NET(veh);
	PED::SET_PED_INTO_VEHICLE(ped, veh, -1);
	VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(veh);
	VEHICLE::SET_VEHICLE_ENGINE_ON(veh, true, true, true);
	ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&veh);
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(veh_hash);
}

static void check_for_selections()
{
	if (time_apply->selected)
	{
		set_time();
	}
	else
	{
		for (int i = 0; i < vehicle_spawn_items.size(); i++)
		{
			menu_item *item = vehicle_spawn_items[i];
			if (item->selected)
			{
				spawn_vehicle(i);
				break;
			}
		}
	}
}

static void tick()
{
	playerlist_update();
	pool.tick();
	check_for_selections();

	Player player = PLAYER::PLAYER_ID();

	Ped ped = PLAYER::PLAYER_PED_ID();
	ENTITY::SET_ENTITY_INVINCIBLE(ped, player_godmode->state);
	ENTITY::SET_ENTITY_VISIBLE(ped, !player_invisible->state, false);
	//mhooking.set_player_speed(player_speedmultiplier->slider_get_selected_value());
	PLAYER::SET_RUN_SPRINT_MULTIPLIER_FOR_PLAYER(player, player_speedmultiplier->slider_get_selected_value());

	Vehicle vehicle = PED::GET_VEHICLE_PED_IS_IN(ped, settings_selectlastvehicle->state ? true : false);
	if (ENTITY::DOES_ENTITY_EXIST(vehicle))
	{
		ENTITY::SET_ENTITY_INVINCIBLE(vehicle, vehicle_invincible->state ? true : false);
		float r = vehicle_color_red->slider_get_selected_value(), g = vehicle_color_green->slider_get_selected_value(), b = vehicle_color_blue->slider_get_selected_value();
		VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, (int) r, (int) g, (int) b);
		VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle, (int) r, (int) g, (int) b);
	}

	if (time_freeze->state)
	{
		set_time();
	}
}

static void init_menus()
{
	main_menu->add_item(new menu_item("ONLINE", online_menu));
	main_menu->add_item(new menu_item("PLAYER", player_menu));
	main_menu->add_item(new menu_item("VEHICLE", vehicle_menu));
	main_menu->add_item(new menu_item("PROPS"));
	main_menu->add_item(new menu_item("WORLD", world_menu));
	main_menu->add_item(new menu_item("MISC", misc_menu));
	pool.add_menu(main_menu);

	online_menu->add_item(new menu_item("Players", playerlist_menu));
	pool.add_menu(online_menu);
	
	pool.add_menu(playerlist_menu);

	player_menu->add_item(player_godmode);
	player_menu->add_item(player_invisible);
	player_menu->add_item(player_speedmultiplier);
	pool.add_menu(player_menu);

	vehicle_menu->add_item(new menu_item("Spawn Vehicle", vehicle_spawn_menu));
	for (Hash veh_hash : vehicle_list)
	{
		char *display_name = VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(veh_hash);
		menu_item *item = new menu_item(display_name);
		vehicle_spawn_menu->add_item(item);
		vehicle_spawn_items.push_back(item);
	}
	pool.add_menu(vehicle_spawn_menu);
	vehicle_menu->add_item(new menu_item("Color", vehicle_color_menu));
	std::vector<float> color_values;
	for (float i = 0.f; i < 256; i++)
	{
		color_values.push_back(i);
	}
	vehicle_color_red = new menu_item("Red", color_values);
	vehicle_color_menu->add_item(vehicle_color_red);
	vehicle_color_green = new menu_item("Green", color_values);
	vehicle_color_menu->add_item(vehicle_color_green);
	vehicle_color_blue = new menu_item("Blue", color_values);
	vehicle_color_menu->add_item(vehicle_color_blue);
	pool.add_menu(vehicle_color_menu);
	vehicle_menu->add_item(vehicle_invincible);
	pool.add_menu(vehicle_menu);

	world_menu->add_item(new menu_item("Time", world_time_menu));
	world_time_menu->add_item(time_hour);
	std::vector<float> minute_second_values;
	for (float i = 0.f; i < 60; i++)
	{
		minute_second_values.push_back(i);
	}
	time_minute = new menu_item("Minute", minute_second_values);
	world_time_menu->add_item(time_minute);
	time_second = new menu_item("Second", minute_second_values);
	world_time_menu->add_item(time_second);
	world_time_menu->add_item(time_apply);
	world_time_menu->add_item(time_freeze);
	pool.add_menu(world_time_menu);
	pool.add_menu(world_menu);

	misc_menu->add_item(new menu_item("Settings", misc_settings_menu));
	misc_settings_menu->add_item(settings_selectlastvehicle);
	pool.add_menu(misc_settings_menu);
	pool.add_menu(misc_menu);
}

void script_main()
{
	init_menus();

	srand(GetTickCount());
	util::draw_notification("~g~SceehMenu initialized!");

	while (true)
	{
		WAIT(0);
		tick();
	}
}

void on_key(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow)
{
	if (wasDownBefore)
	{
		return;
	}

	if (key == VK_F4)
	{
		for (menu *menu : pool.get_menus())
		{
			menu->visible = false;
		}
		main_menu->visible = true;
	}

	pool.handle_keyboard(key);
}