#ifndef __Player_H__
#define __Player_H__

#include "Module.h"
#include "Troop.h"
#include "Building.h"
#include "Building.h"
#include "Troop.h"
#include "SDL\include\SDL_gamecontroller.h"

struct SDL_Texture;
struct GamePad
{
	SDL_GameController* GameController = nullptr;
	SDL_Joystick* joy = nullptr;

	bool Connected = false;
	bool State[MAX_BUTTONS];

	j1KeyState Controller[MAX_BUTTONS]; //This is the one you check. EXAMPLE: App->input->name.Controller[JOY_RIGHT] == KEY_REPEAT 

	int LeftAxisX = 0;
	int LeftAxisY = 0;

};

struct GeneralUI
{
	bool upgrade, repair;
	string name;
	uint level, health, unique;
};

struct Collider
{
	pair<int, int> dimensions;
	vector<pair<int,int>> tiles;
};


class Player : public Module
{
public:
	enum CURRENT_UI
	{
		CURR_MAIN = 1,
		CURR_BUILD,
		CURR_DEPLOY,
		CURR_CAST,
		CURR_GENERAL
	};

	Player();
	~Player();

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void GetCurrentTile(int& x, int& y);

	void UpdateVisibility(); //update gui visibility
	void DoLogic(UI_Element* data); //gui actions

	bool CheckBuildingPos();
	void UpdateWalkabilityMap(bool isWalkable);
	bool DeleteEntity(Entity* entity);

public:
	bool onUI;
	bool isBuilding;
	bool isDeploying;
	bool isCasting;

	Collider collider;
	pair<int, int> offset;
	Entity::entityType type;

	bool isPlayer1;
	string team;
	uint gold;
	uint currentUI;
	int actual_capacity;

	GamePad gamepad;

	pair<int,int> currentTile;
	pair<int, int> x_limits, y_limits;

	list<Building*> buildings;
	list<Troop*> troops;


	//---
	list<UI_Element*> UI_elements;

	UI_Element* Health_UI;
	UI_Element* Gold_UI;

	UI_Element* Main_UI;
	UI_Element* Build_icon;
	UI_Element* Deploy_icon;
	UI_Element* Cast_icon;

	UI_Element* Build_UI;
	UI_Element* Def_AOE_icon;
	UI_Element* Def_Target_icon;
	UI_Element* Mines_icon;
	UI_Element* Barracks_icon;

	UI_Element* Deploy_UI;
	UI_Element* Soldier_icon;
	UI_Element* Tankman_icon;
	UI_Element* Infiltrator_icon;
	UI_Element* Engineer_icon;
	UI_Element* War_hound_icon;

	UI_Element* Cast_UI;
	UI_Element* Missiles_icon;
	UI_Element* Cast2_icon;
	UI_Element* Cast3_icon;

	// Selected Building UI
	UI_Element* General_UI;
	UI_Element* Upgrade_icon;
	UI_Element* Repair_icon;
	UI_Element* Name_text;
	UI_Element* Level_text;
	UI_Element* Health_text;
	UI_Element* UpgradeCost_text;
	UI_Element* RepairCost_text;
	UI_Element* Damage_text; //only for defense buildings
	UI_Element* Prod_text; //only for townhall & mines
	UI_Element* Capacity_text; //only for barracks

};

#endif // !__Player_H__