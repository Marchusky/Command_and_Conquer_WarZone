#include "Defs.h"
#include "Log.h"

#include "App.h"
#include "Player.h"
#include "Gui.h"
#include "Input.h"
#include "Window.h"
#include "EntityManager.h"
#include "Render.h"
#include "Pathfinding.h"
#include "MainMenu.h"
#include "Fonts.h"
#include "Audio.h"
#include "Scene.h"
#include "Transitions.h"


#include "Brofiler\Brofiler.h"


Player::Player() : Module()
{
}

Player::~Player()
{
}

bool Player::Start()
{

	gold = actual_capacity = time_iterator = number_of_troops = 0;
	SoldiersCreated = TankmansCreated = InfiltratorsCreated = EngineersCreated = WarHoundsCreated = 0;


	live = 2000;

	UI_troop_type = 9;

	isBuilding = isDeploying = isCasting = Y_pressed = isPaused = false;
  
	currentTile = { 13,0 };

	preview_rects = vector<SDL_Rect>(Entity::entityType::WAR_HOUND, { 0,0,0,0 });

	pugi::xml_document	config_file;
	pugi::xml_node config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child("rect_previews").first_child();

	for (int i = Entity::entityType::TOWNHALL; i <= Entity::entityType::WAR_HOUND; i++) {
		preview_rects[i].x = config.attribute((isPlayer1) ? "rx" : "x").as_int(0);
		preview_rects[i].y = config.attribute((isPlayer1) ? "ry" : "y").as_int(0);
		preview_rects[i].w = config.attribute("w").as_int(0);
		preview_rects[i].h = config.attribute("h").as_int(0);
		config = config.next_sibling();
	}

	return true;
}

bool Player::Update(float dt)
{
	BROFILER_CATEGORY("Player Update", Profiler::Color::Black);

	if (!App->scene->endgame){
    
	//Preview all player1 entities with M
	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN) {
		isBuilding = !isBuilding;
	}

	if (isBuilding && App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN) {
		isPlayer1 = true;
		type = (Entity::entityType)((curr++) % (int)Entity::entityType::TANKMAN);
	}

	//--- Press X (Square)
	if (gamepad.Controller[BUTTON_X] == KEY_DOWN)
	{
		//--- Press X (Square)
		if (gamepad.Controller[BUTTON_X] == KEY_DOWN)
		{
			onUI = !onUI;
		}

		// Button with focus changes state to HOVER 
		if (currentUI != CURRENT_UI::NONE && currentUI != CURRENT_UI::ENDGAME && currentUI != CURRENT_UI::CURR_WIN_SCREEN && gamepad.Controller[BUTTON_A] != KEY_REPEAT && focus._Ptr != nullptr)
		{
			(*focus)->state = UI_Element::State::HOVER;
		}

		// PAUSE
		if (gamepad.Controller[START] == KEY_DOWN && App->scene->active)
		{
			if (!App->scene->pause)
			{
				App->scene->world_seconds.Stop();
				App->render->zoom = 1;
				Pause_UI->visible = true;
				isPaused = true;

				App->scene->pause = !App->scene->pause;

				if (currentUI != CURRENT_UI::NONE)
					(*focus)->state = UI_Element::State::IDLE;

				last_currentUI = currentUI;
				currentUI = CURRENT_UI::CURR_PAUSE;
				UpdateFocus(currentUI);
			}
			else if (App->scene->pause && isPaused && currentUI == CURRENT_UI::CURR_PAUSE)
			{
				App->scene->world_seconds.Start();
				App->render->zoom = 0.77;
				Pause_UI->visible = false;
				isPaused = false;

				App->scene->pause = !App->scene->pause;


				currentUI = last_currentUI;
				UpdateVisibility();
				UpdateFocus(currentUI);

			}


		}

		// --TEST-- from GENERAL UI to CREATE TROOPS UI (only for barracks)
		if (gamepad.Controller[BACK] == KEY_DOWN && currentUI == CURRENT_UI::CURR_GENERAL && App->scene->active)
    {
	// Button with focus changes state to HOVER 
	if (currentUI != CURRENT_UI::NONE && gamepad.Controller[BUTTON_A] != KEY_REPEAT && focus._Ptr != nullptr)
	{
		(*focus)->state = UI_Element::State::HOVER;
	}

	// Button A to clcik a button
	if (gamepad.Controller[BUTTON_A] == KEY_DOWN && currentUI != CURRENT_UI::NONE)
	{
		if (currentUI != CURRENT_UI::CURR_BUILD && currentUI != CURRENT_UI::CURR_DEPLOY && currentUI != CURRENT_UI::CURR_CAST)
			(*focus)->state = UI_Element::State::LOGIC;
	}

	if (gamepad.Controller[BUTTON_A] == KEY_UP && currentUI != CURRENT_UI::NONE)
	{
		if (!isBuilding)
			(*focus)->state = UI_Element::State::IDLE;
		if (App->scene->active)
			DoLogic((*focus));
		else
			App->main_menu->DoLogic((*focus));

		if ((*focus) == Build_icon || (*focus) == Deploy_icon || (*focus) == Cast_icon)
			UpdateFocus(currentUI);
	}

	if (gamepad.Controller[BUTTON_B] == KEY_DOWN && currentUI != CURRENT_UI::NONE)
	{
		(*focus)->state = UI_Element::State::IDLE;

		if (currentUI == CURR_BUILD)
		{
			currentUI = CURRENT_UI::CURR_CREATE_TROOPS;
			UpdateVisibility();
			//UpdateFocus(currentUI);
		}

		//Creating TROOPS
		if (currentUI == CURRENT_UI::CURR_CREATE_TROOPS)
		{
			if (gamepad.Controller[LEFT] == KEY_DOWN)
			{
				UI_troop_type--;
				if (UI_troop_type < 9) //soldier
				{
					UI_troop_type = 13; // war_hound
				}
				Update_troop_image(UI_troop_type);
				
			}
			
			if (gamepad.Controller[RIGHT] == KEY_DOWN)
			{
				UI_troop_type++;
				if (UI_troop_type > 13) // war_hound
				{
					UI_troop_type = 9;//soldier
				}
				Update_troop_image(UI_troop_type);
			}

			if (gamepad.Controller[LB] == KEY_DOWN)
			{
				number_of_troops--;
				if (number_of_troops < 0)
				{
					number_of_troops = 0;
				}

			}

			if (gamepad.Controller[RB] == KEY_DOWN)
			{
				number_of_troops++;
				/*if (number_of_troops + number_troops_in_barrack > barracks->capacity) 
				{
					number_of_troops = barracks->capacity - number_troops_in_barrack; 
				}*/
			}

			if (gamepad.Controller[BUTTON_A] == KEY_UP)
			{
				CreateTroop(UI_troop_type, number_of_troops);
				Update_troop_image(UI_troop_type);
				GotoPrevWindows(currentUI);
			}
			
		}

		// --TEST-- GENERAL UI (menu of selected building)
		if (gamepad.Controller[BACK] == KEY_DOWN && currentUI == CURRENT_UI::NONE && App->scene->active)
		{
			currentUI = CURRENT_UI::CURR_GENERAL;
			UpdateVisibility();
			UpdateFocus(currentUI);
		}		

		// Button A to clcik a button
		if (gamepad.Controller[BUTTON_A] == KEY_DOWN && currentUI != CURRENT_UI::NONE && currentUI != CURRENT_UI::CURR_PAUSE_SETTINGS && currentUI != CURRENT_UI::CURR_CREATE_TROOPS)
		{
			if (currentUI != CURRENT_UI::CURR_BUILD && currentUI != CURRENT_UI::CURR_DEPLOY && currentUI != CURRENT_UI::CURR_CAST)
				(*focus)->state = UI_Element::State::LOGIC;
		}
	}

	if (gamepad.Controller[BUTTON_Y] == KEY_DOWN && currentUI == CURRENT_UI::NONE)
	{
		if (App->scene->active)
			currentUI = CURRENT_UI::CURR_MAIN;
		else if (App->main_menu->active)
			currentUI = CURRENT_UI::CURR_MAIN_MENU;
    
		// Do button action
		if (gamepad.Controller[BUTTON_A] == KEY_UP && currentUI != CURRENT_UI::NONE && currentUI != CURRENT_UI::CURR_PAUSE_SETTINGS && currentUI != CURRENT_UI::CURR_CREATE_TROOPS)
		{
			if (App->scene->pause && isPaused == true)
			{
				(*focus)->state = UI_Element::State::IDLE;
				DoLogic((*focus));
				UpdateFocus(currentUI);
			}
			else if (!App->scene->pause)
			{
				if (!isBuilding)
					(*focus)->state = UI_Element::State::IDLE;
				if (App->scene->active)
					DoLogic((*focus));
				else
					App->main_menu->DoLogic((*focus));

				if ((*focus) == Build_icon || (*focus) == Deploy_icon || (*focus) == Cast_icon)
					UpdateFocus(currentUI);
			}

		}

		// Go back
		if (gamepad.Controller[BUTTON_B] == KEY_DOWN && currentUI != CURRENT_UI::NONE)
		{
			(*focus)->state = UI_Element::State::IDLE;

			if (App->scene->pause && isPaused && currentUI == CURRENT_UI::CURR_PAUSE)
			{
				App->scene->world_seconds.Start();
				App->render->zoom = 0.77;
				Pause_UI->visible = false;
				isPaused = false;

				App->scene->pause = !App->scene->pause;

				currentUI = last_currentUI;
				UpdateVisibility();
				UpdateFocus(currentUI);
			}
			else if (App->scene->pause && isPaused && currentUI != CURRENT_UI::CURR_PAUSE)
			{
				GotoPrevWindows(currentUI);
				UpdateFocus(currentUI);
			}
			else if (!App->scene->pause)
			{
				if (currentUI == CURR_BUILD)
				{
					if (isBuilding == true)
					{
						isBuilding = false;
						App->map->debug = false;
					}
					else if (isBuilding == false)
					{

						GotoPrevWindows(currentUI);
						UpdateFocus(currentUI);
					}
				}
				else
				{
					GotoPrevWindows(currentUI);
					UpdateFocus(currentUI);
				}
			}
      // BUILDING MERGE LINE BELOW
			focus = GetUI_Element(currentUI)->children.begin();
		}

		// Enter to UI ingame Menus
		if (gamepad.Controller[BUTTON_Y] == KEY_DOWN && currentUI == CURRENT_UI::NONE)
		{
			if (App->scene->active)
				currentUI = CURRENT_UI::CURR_MAIN;

			Y_pressed = true;

			UpdateFocus(currentUI);
		}


		//Change the side images from the menus
		if (App->scene->active)
		{
			if (Y_pressed == false)
			{
				Y_to_Main->visible = true;
				Y_to_Main2->visible = true;
				RB_img->visible = false;
				LB_img->visible = false;
			}
			else if (Y_pressed == true)
			{
				Y_to_Main->visible = false;
				Y_to_Main2->visible = false;
				RB_img->visible = true;
				LB_img->visible = true;
			}
		}
		// Travel through the different buttons
		if (gamepad.Controller[RB] == KEY_DOWN && currentUI != CURRENT_UI::NONE && currentUI != CURRENT_UI::CURR_CREATE_TROOPS && gamepad.Controller[BUTTON_A] != KEY_REPEAT && isBuilding == false && !App->scene->pause && App->scene->active)
		{
			(*focus)->state = UI_Element::State::IDLE;

			if (focus == last_element)
			{
				focus = GetUI_Element(currentUI)->children.begin();

			}
			else
			{
				focus++;
			}

		}
		// Travel through the different buttons
		if (gamepad.Controller[LB] == KEY_DOWN && currentUI != CURRENT_UI::NONE && currentUI != CURRENT_UI::CURR_CREATE_TROOPS && gamepad.Controller[BUTTON_A] != KEY_REPEAT && isBuilding == false && !App->scene->pause && App->scene->active)
		{
			(*focus)->state = UI_Element::State::IDLE;
			if (focus == GetUI_Element(currentUI)->children.begin())
			{
				focus = last_element;
			}
			else
			{
				focus--;
			}

		}

		//Travel through buttons with DPAD in pause and mainmenu
		if (App->main_menu->active || App->scene->pause)
		{
			if (gamepad.Controller[UP] == KEY_DOWN && currentUI != CURRENT_UI::NONE && gamepad.Controller[BUTTON_A] != KEY_REPEAT)
			{
				(*focus)->state = UI_Element::State::IDLE;

				if (focus == last_element)
				{
					focus = GetUI_Element(currentUI)->children.begin();

				}
				else
				{
					focus++;
				}

			}

			if (gamepad.Controller[DOWN] == KEY_DOWN && currentUI != CURRENT_UI::NONE && gamepad.Controller[BUTTON_A] != KEY_REPEAT)
			{
				(*focus)->state = UI_Element::State::IDLE;
				if (focus == GetUI_Element(currentUI)->children.begin())
				{
					focus = last_element;
				}
				else
				{
					focus--;
				}

			}
		}

		// Increase or decrease volume
		if (gamepad.Controller[RIGHT] == KEY_DOWN && currentUI == CURRENT_UI::CURR_PAUSE_SETTINGS)
		{
			if ((*focus) == Music_Settings && App->audio->musicVolume < 100)
			{
				App->audio->musicVolume += 10;
				App->audio->SetMusicVolume();


			}
			else if ((*focus) == FX_Settings && App->audio->sfxVolume < 100)
			{
				App->audio->sfxVolume += 10;

				App->audio->SetSfxVolume();
			}
		}
		else if (gamepad.Controller[LEFT] == KEY_DOWN && currentUI == CURRENT_UI::CURR_PAUSE_SETTINGS)
		{
			if ((*focus) == Music_Settings && App->audio->musicVolume > 0)
			{
				App->audio->musicVolume -= 10;

				App->audio->SetMusicVolume();
			}
			else if ((*focus) == FX_Settings && App->audio->sfxVolume > 0)
			{
				App->audio->sfxVolume -= 10;
			
				App->audio->SetSfxVolume();
			}
		}

		// Just to test the LiveBar. Can only be used in GodMode (Press F10)
		if (!App->scene->pause && gamepad.Controller[UP] == KEY_DOWN && live > 0 && App->scene->active && App->scene->godmode)
		{
			live -= 1000;
			if (live < 0)
			{
				live = 0;
			}
			
		}
	
  }

	
	if (App->scene->endgame && gamepad.Controller[BUTTON_A] == KEY_DOWN)
	{
		App->scene->ResetGame();
	}

	//--- Building ---------------------
	if (isBuilding && !App->scene->pause)
	{
		//--- Movement
		if (gamepad.Controller[JOY_UP] == KEY_REPEAT || gamepad.Controller[UP] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_B) == KEY_REPEAT)
		{
			currentTile.second--;
		}
		else if (gamepad.Controller[JOY_DOWN] == KEY_REPEAT || gamepad.Controller[DOWN] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT)
		{
			currentTile.second++;
		}

		if (gamepad.Controller[JOY_RIGHT] == KEY_REPEAT || gamepad.Controller[RIGHT] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
		{
			currentTile.first++;
		}
		else if (gamepad.Controller[JOY_LEFT] == KEY_REPEAT || gamepad.Controller[LEFT] == KEY_DOWN ||
			App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT)
		{
			currentTile.first--;
		}

		//--- Limits
		//need to modify, when player1 is building we must chehk its width
		if (isPlayer1 == true)
		{
			if (currentTile.first <= x_limits_player1.first) //left limit
			{
				currentTile.first = x_limits_player1.first;
			}
			else if (currentTile.first + collider.dimensions.first >= x_limits_player1.second) //right limit
			{
				currentTile.first = x_limits_player1.second - collider.dimensions.first;
			}

			if (currentTile.second < y_limits_player1.first) //up limit
			{
				currentTile.second = y_limits_player1.first;
			}
			else if (currentTile.second + collider.dimensions.second >= y_limits_player1.second) //down limit
			{
				currentTile.second = y_limits_player1.second - collider.dimensions.second;
			}
		}
		else if (isPlayer1 == false)
		{
			if (currentTile.first + collider.dimensions.first >= x_limits_player2.second) // right limit
			{
				currentTile.first = x_limits_player2.second - collider.dimensions.first;
			}
			else if (currentTile.first <= x_limits_player2.first) //left limit
			{
				currentTile.first = x_limits_player2.first;
			}

			if (currentTile.second < y_limits_player2.first) //up limit
			{
				currentTile.second = y_limits_player2.first;
			}
			else if (currentTile.second + collider.dimensions.second >= y_limits_player2.second) //down limit
			{
				currentTile.second = y_limits_player2.second - collider.dimensions.second;
			}

		}


		//--- Press A
		App->map->debug = true;
		if (CheckBuildingPos() == true) // Can build
		{
			//Preview entity to spawn
			pair<int, int> pos;
			App->input->GetMousePosition(pos.first, pos.second);
			pos = App->render->ScreenToWorld(pos.first, pos.second);
			pos.first--;

			App->render->Blit(App->entitymanager->entitiesTextures[type], pos.first, pos.second, &preview_rects[type]);


			if (gamepad.Controller[BUTTON_A] == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				//play fx (build);
				App->entitymanager->AddEntity(isPlayer1, type, { collider.tiles[0].first /*- offset.first*/, collider.tiles[0].second /*- offset.second*/ },collider);

				entityAdded = false;
				isBuilding = false;
				currentUI == CURRENT_UI::CURR_GENERAL;
			}
		}
		else
		{
			if (gamepad.Controller[BUTTON_A] == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				if (type >= Entity::entityType::TOWNHALL && type <= Entity::entityType::BARRACKS) //if building
				{
					//buildings.pop_back();

				}
				else if (type > Entity::entityType::BARRACKS) //if troops
				{
					//troops.pop_back();
				}
				entityAdded = true;
				isBuilding = false;
				currentUI == CURRENT_UI::CURR_GENERAL;

				//play fx (error);
			}
		}
	}



	return true;
}

bool Player::PostUpdate()
{
	BROFILER_CATEGORY("Player PostUpdate", Profiler::Color::Black);

	return true;
}

bool Player::CleanUp()
{
	LOG("---Player Deleted");

	//Clear buildings
	list<Building*>::iterator item = buildings.begin();
	while (item != buildings.end())
	{
		(*item)->CleanUp();
		RELEASE(*item);
		item++;
	}
	buildings.clear();

	//Clear troops
	list<Troop*>::iterator titem = troops.begin();
	while (titem != troops.end())
	{
		(*titem)->CleanUp();
		RELEASE(*titem);
		titem++;
	}
	troops.clear();

	//Clear UI elements
	list<UI_Element*>::iterator item2 = UI_elements.begin();
	while (item2 != UI_elements.end())
	{
		(*item2)->children.clear();
		RELEASE(*item2);
		item2++;
	}
	UI_elements.clear();

	return true;
}

void Player::GetCurrentTile(int& x, int& y)
{
	x = currentTile.first;
	y = currentTile.second;
}

bool Player::CheckBuildingPos() // Check collider with walkability map
{
	bool ret = true;

	pair<int, int> pos, real_pos;
	pos = currentTile;

	if (gamepad.Connected == true)
	{
		pos = currentTile;
	}
	else
	{
		App->input->GetMousePosition(pos.first, pos.second);
		pos = App->render->ScreenToWorld(pos.first, pos.second);

		pos = App->map->WorldToMap(pos.first, pos.second);
		pos.first--;
	}


	// Check what tiles is the collider occupying
	int cont;
	collider.tiles.resize(0); //reset vector
	for (int i = 0; i < collider.dimensions.first; i++)
	{
		for (int j = 0; j < collider.dimensions.second; j++)
		{
			real_pos = App->map->MapToWorld(pos.first, pos.second);
			collider.tiles.push_back(real_pos); //add tile to collider.tiles
			pos.second++;
			cont = j;
		}
		pos.first++;
		pos.second -= cont + 1;
	}

	// Compare tiles with walkability map
	for (int i = 0; i < collider.tiles.size(); i++)
	{
		pos = App->map->WorldToMap(collider.tiles[i].first, collider.tiles[i].second);
		if (App->pathfinding->IsWalkable(pos) == false)
		{
			ret = false;
			break;
		}
	}

	// Draw Collider
	SDL_Rect rect;
	for (int i = 0; i < collider.tiles.size(); i++)
	{
		if (ret == true)
			rect = { 0,0,60,29 }; //green
		else
			rect = { 60,0,60,29 }; //red

		App->render->Blit(App->map->debug_tex, collider.tiles[i].first, collider.tiles[i].second, &rect);
	}
	return ret;
}

Collider Player::GetCollider(pair<int, int> dimensions, pair<int, int> topTile_pos)
{
	int cont = 0;
	pair<int, int> real_pos;

	Collider new_collider;
	new_collider.dimensions = dimensions;

	for (int i = 0; i < new_collider.dimensions.first; i++)
	{
		for (int j = 0; j < new_collider.dimensions.second; j++)
		{
			real_pos = App->map->MapToWorld(topTile_pos.first, topTile_pos.second);
			new_collider.tiles.push_back(real_pos);
			topTile_pos.second++;
			cont = j;
		}
		topTile_pos.first++;
		topTile_pos.second -= cont + 1;
	}
	return new_collider;
}


void Player::UpdateWalkabilityMap(bool isWalkable, Collider collider) //update walkable tiles
{
	for (int i = 0; i < collider.tiles.size(); ++i)
	{
		pair <int, int> pos = App->map->WorldToMap(collider.tiles[i].first, collider.tiles[i].second);
		if (App->pathfinding->GetTileAt(pos) != isWalkable)
		{
			App->pathfinding->ChangeWalkability(pos, isWalkable);
			App->map->walkability_layer->Set(pos.first, pos.second, 1);
		}
	}
}

void Player::UpdateFocus(uint data)
{
	switch (data)
	{
	case::Player::CURRENT_UI::CURR_MAIN_MENU:
		focus = App->main_menu->menu_background->children.begin();
		last_element = App->main_menu->menu_background->children.end();
		last_element--;
		break;
	case::Player::CURRENT_UI::CURR_MAIN:
		focus = Main_UI->children.begin();
		last_element = Main_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_GENERAL:
		focus = General_UI->children.begin();
		last_element = General_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_BUILD:
		focus = Build_UI->children.begin();
		last_element = Build_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_CAST:
		focus = Cast_UI->children.begin();
		last_element = Cast_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_DEPLOY:
		focus = Deploy_UI->children.begin();
		last_element = Deploy_UI->children.end();
		last_element--;
		break;
      
	case::Player::CURRENT_UI::CURR_PAUSE:
		focus = Pause_UI->children.begin();
		last_element = Pause_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_PAUSE_SETTINGS:
		focus = Settings_UI->children.begin();
		last_element = Settings_UI->children.end();
		last_element--;
		break;

	case::Player::CURRENT_UI::CURR_PAUSE_ABORT:
		focus = Abort_UI->children.begin();
		last_element = Abort_UI->children.end();
		last_element--;
		break;
	
	}
}


void Player::GotoPrevWindows(uint data)
{
	switch (data)
	{
	case Player::CURRENT_UI::CURR_MAIN:
		currentUI = CURRENT_UI::NONE;
		Y_pressed = false;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_GENERAL:
		currentUI = CURRENT_UI::NONE;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_CREATE_TROOPS:
		currentUI = CURRENT_UI::CURR_GENERAL;

		UI_troop_type = 9; 
		number_of_troops = 0;
		Update_troop_image(number_of_troops);

		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_BUILD:
		currentUI = CURRENT_UI::CURR_MAIN;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_DEPLOY:
		currentUI = CURRENT_UI::CURR_MAIN;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_CAST:
		currentUI = CURRENT_UI::CURR_MAIN;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_PAUSE:
		currentUI = last_currentUI;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_PAUSE_SETTINGS:
		currentUI = CURR_PAUSE;
		UpdateVisibility();
		break;

	case Player::CURRENT_UI::CURR_PAUSE_ABORT:
		currentUI = CURR_PAUSE;
		UpdateVisibility();
		break;

	}
}

UI_Element* Player::GetUI_Element(uint data)
{
	switch (data)
	{
	case::Player::CURRENT_UI::CURR_MAIN_MENU:
		return App->main_menu->menu_background;
	case::Player::CURRENT_UI::CURR_MAIN:
		return Main_UI;

	case::Player::CURRENT_UI::CURR_BUILD:
		return Build_UI;

	case::Player::CURRENT_UI::CURR_CAST:
		return Cast_UI;

	case::Player::CURRENT_UI::CURR_DEPLOY:
		return Deploy_UI;

	case::Player::CURRENT_UI::CURR_PAUSE:
		return Pause_UI;

	case::Player::CURRENT_UI::CURR_PAUSE_SETTINGS:
		return Settings_UI;

	case::Player::CURRENT_UI::CURR_PAUSE_ABORT:
		return Abort_UI;

	case::Player::CURRENT_UI::CURR_GENERAL:
		return General_UI;

	}
}

void Player::UpdateVisibility() // Update GUI Visibility
{
	switch (currentUI)
	{
	case::Player::CURRENT_UI::NONE:
		Main_UI->visible = true;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Abort_UI->visible = false;
		Settings_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;
	case::Player::CURRENT_UI::CURR_MAIN:
		Main_UI->visible = true;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_BUILD:
		Main_UI->visible = false;
		Build_UI->visible = true;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_DEPLOY:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = true;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_CAST:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = true;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_GENERAL:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Abort_UI->visible = false;
		Settings_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = true;
		Create_Troops_UI->visible = false;

		break;

	case::Player::CURRENT_UI::CURR_CREATE_TROOPS:

		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Abort_UI->visible = false;
		Settings_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = true;

		break;

	case::Player::CURRENT_UI::CURR_PAUSE:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = true;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_PAUSE_SETTINGS:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = true;
		Abort_UI->visible = false;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;

	case::Player::CURRENT_UI::CURR_PAUSE_ABORT:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = true;
		win_screen->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		break;
	case::Player::CURRENT_UI::CURR_WIN_SCREEN:
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = true;
		Gold_UI->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		Y_to_Main2->visible = false;
		Y_to_Main->visible = false;
		LB_img->visible = false;
		RB_img->visible = false;
		break;
	case::Player::CURRENT_UI::ENDGAME: //Dont show the other player win screen
		Main_UI->visible = false;
		Build_UI->visible = false;
		Deploy_UI->visible = false;
		Cast_UI->visible = false;
		Pause_UI->visible = false;
		Settings_UI->visible = false;
		Abort_UI->visible = false;
		win_screen->visible = false;
		Gold_UI->visible = false;
		App->scene->ui_timer->visible = false;
		General_UI->visible = false;
		Create_Troops_UI->visible = false;
		Y_to_Main2->visible = false;
		Y_to_Main->visible = false;
		LB_img->visible = false;
		RB_img->visible = false;
		break;


	}
	App->gui->UpdateChildren();
}

void Player::DoLogic(UI_Element* data)
{
	switch (data->action)
	{

	/*case::UI_Element::Action::NEW_GAME:
		App->scenechange->ContinueGame = true;
		App->scenechange->SwitchScene(App->scene, App->main_menu);
		menu_background->visible = false;
		break;*/

	case::UI_Element::Action::ACT_GOTO_MAIN:
		currentUI = CURR_MAIN;
		UpdateVisibility();
		break;

	case::UI_Element::Action::ACT_GOTO_BUILD:
		currentUI = CURR_BUILD;
		UpdateVisibility();
		break;

	case::UI_Element::Action::ACT_GOTO_DEPLOY:
		currentUI = CURR_DEPLOY;
		UpdateVisibility();
		break;

	case::UI_Element::Action::ACT_GOTO_CAST:
		currentUI = CURR_CAST;
		UpdateVisibility();
		break;

	case::UI_Element::Action::ACT_BUILD_AOE:
		isBuilding = true;
		type = Entity::entityType::DEFENSE_AOE;
		offset = { 60,30 };
		break;

	case::UI_Element::Action::ACT_BUILD_TARGET:
		isBuilding = true;
		type = Entity::entityType::DEFENSE_TARGET;
		break;

	case::UI_Element::Action::ACT_BUILD_MINE:
		isBuilding = true;
		type = Entity::entityType::MINES;
		break;

	case::UI_Element::Action::ACT_BUILD_BARRACKS:
		isBuilding = true;
		type = Entity::entityType::BARRACKS;
		break;

	case::UI_Element::Action::ACT_DEPLOY_SOLDIER:
		//isBuilding = true;

		type = Entity::entityType::SOLDIER;
		//collider.dimensions = { 1,1 };
		//
		break;

	case::UI_Element::Action::ACT_DEPLOY_TANKMAN:
		//
		break;

	case::UI_Element::Action::ACT_DEPLOY_INFILTRATOR:
		//
		break;

	case::UI_Element::Action::ACT_DEPLOY_ENGINEER:
		//
		break;

	case::UI_Element::Action::ACT_DEPLOY_WARHOUND:
		//
		break;

	case::UI_Element::Action::ACT_CAST_MISSILES:
		//
		break;

	case::UI_Element::Action::ACT_UPGRADE:
		//
		break;

	case::UI_Element::Action::ACT_REPAIR:
		//
		break;
	case::UI_Element::Action::RESUME_PAUSE:
		
		App->scene->world_seconds.Start();
		App->render->zoom = 0.77;
		Pause_UI->visible = false;
		isPaused = false;

		App->scene->pause = !App->scene->pause;

		currentUI = last_currentUI;
		UpdateVisibility();
		UpdateFocus(currentUI);
		break;
	case::UI_Element::Action::SETTINGS_PAUSE:
		// Open settings menu
		Settings_UI->visible = true;
		currentUI = CURR_PAUSE_SETTINGS;
		UpdateFocus(currentUI);
		break;
	case::UI_Element::Action::ABORT_PAUSE:
		currentUI = CURR_PAUSE_ABORT;
		UpdateVisibility();
		UpdateFocus(currentUI);
		break;
	case::UI_Element::Action::RESTART:
		
		App->entitymanager->Disable();
		App->player2->Disable();
		App->player1->Disable();
		App->gui->Disable();
		App->map->Disable();
		App->font->Disable();

		App->scenechange->ChangeMap(-1, 2);
		App->scene->Disable();	//Here fadetoblack
		App->scene->Enable();

		App->font->Enable();
		App->map->Enable();
		App->gui->Enable();
		App->player1->Enable();
		App->player2->Enable();
		App->entitymanager->Enable();

		break;
	}
}

bool Player::DeleteEntity(Entity* entity)
{
	entity->CleanUp();

	if (entity->type >= Entity::entityType::TOWNHALL && entity->type <= Entity::entityType::BARRACKS) //if entity = building
	{
		list<Building*>::iterator item = buildings.begin();
		while (item != buildings.end())
		{
			if ((*item) == entity)
				buildings.erase(item);
		}
	}
	else if (type > Entity::entityType::BARRACKS) //if entity = troop
	{
		list<Troop*>::iterator item = troops.begin();
		while (item != troops.end())
		{
			if ((*item) == entity)
				troops.erase(item);
		}
	}
	return true;
}

void Player::Update_troop_image(int type) // Changes sprite depending on the entity type
{
	switch (type)
	{
	case Entity::entityType::SOLDIER:
		troop_icon->rect = { 662, 0, 85, 81 };
		break;

	case Entity::entityType::TANKMAN:
		troop_icon->rect = { 576, 0, 85, 81 };
		break;

	case Entity::entityType::INFILTRATOR:
		troop_icon->rect = { 832, 0, 85, 81 };
		break;

	case Entity::entityType::ENGINEER:
		troop_icon->rect = { 492, 0, 85, 81 };
		break;

	case Entity::entityType::WAR_HOUND:
		troop_icon->rect = { 746, 0, 85, 81 };
		break;
	}
}

void Player::CreateTroop(int type, int number)
{
	switch (type)
	{
	case Entity::entityType::SOLDIER:
		SoldiersCreated += number;
		break;

	case Entity::entityType::TANKMAN:
		TankmansCreated += number;
		break;

	case Entity::entityType::INFILTRATOR:
		InfiltratorsCreated += number;
		break;

	case Entity::entityType::ENGINEER:
		EngineersCreated += number;
		break;

	case Entity::entityType::WAR_HOUND:
		WarHoundsCreated += number;
		break;
	}
}