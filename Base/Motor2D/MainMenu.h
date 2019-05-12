#ifndef __j1MAIN_MENU_H__
#define __j1MAIN_MENU_H__

#include "Module.h"
#include "UI_Element.h"

struct SDL_Texture;



class MainMenu : public Module
{
public:
	enum CURRENT_MM_UI
	{
		NONE,
		//MM = MAINMENU
		CURR_MM_SETTINGS = 16,
		CURR_MM_CREDITS,
	};
	MainMenu();

	// Destructor
	virtual ~MainMenu();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void UpdateVisibility(PlayerUI player);
	void DoLogic(UI_Element* data);


public:
	string menu_bg_file_name;
	string settings_main_menu_name;
	SDL_Texture* settings_texture;

	UI_Element* menu_background = nullptr;
	//------PLAY
	UI_Element* new_game_button = nullptr;
	UI_Element* new_game_text = nullptr;
	char new_game_label[5] = "PLAY";
	//------SETTINGS
	UI_Element* settings_button = nullptr;
	UI_Element* settings_text = nullptr;
	char settings_label[9] = "SETTINGS";

	UI_Element* Settings_Title = nullptr;
	char Settings_Title_label[9] = "Settings";

	//SETTINGS SLIDERS
	UI_Element* Music_Settings = nullptr;
	UI_Element* Music_text = nullptr;
	char Music_label[14] = "Music Volume";
	UI_Element* Music_Slider = nullptr;
	UI_Element* Music_Slider_text = nullptr;
	char Music_Slider_label[5] = "0";
	UI_Element* Music_Slider_Button = nullptr;

	UI_Element* FX_Settings = nullptr;
	UI_Element* FX_text = nullptr;
	char FX_label[10] = "FX Volume";
	UI_Element* FX_Slider = nullptr;
	UI_Element* FX_Slider_text = nullptr;
	char FX_Slider_label[5] = "0";
	UI_Element* FX_Slider_Button = nullptr;
	//CREDITS
	UI_Element* credits_button = nullptr;
	UI_Element* credits_text = nullptr;
	char credits_label[9] = "CREDITS";

	UI_Element* exit_button = nullptr;
	UI_Element* exit_text = nullptr;
	char exit_label[5] = "EXIT";

	UI_Element* MM_Settings_UI = nullptr;

	UI_Element* Credits_UI = nullptr;

	string current_track;
	string current_fx;
private:
	bool close_app = true;
};

#endif // __j1MAIN_MENU_H__