#include "Defs.h"
#include "Log.h"

#include "EntityManager.h"
#include "App.h"
#include "Audio.h"
#include "Render.h"
#include "Pathfinding.h"
#include "Scene.h"
#include "Transitions.h"
#include "Entity.h"
#include "Player.h"
#include "Textures.h"
#include "Barracks.h"
#include "CommandCenter.h"
#include "DefenseAOE.h"
#include "DefenseTarget.h"
#include "MainDefense.h"
#include "Mines.h"
#include "Townhall.h"
#include "Walls.h"
#include "Soldier.h"
#include "Engineer.h"
#include "Tankman.h"
#include "Infiltrator.h"
#include "Hound.h"
#include "Building.h"
#include "Troop.h"

#include "Brofiler\Brofiler.h"
#include "PugiXml/src/pugixml.hpp"
#include <cmath>


EntityManager::EntityManager()
{
	name.append("entitymanager");
}

EntityManager::~EntityManager()
{
}

bool EntityManager::Awake(pugi::xml_node &config)
{
	folder.append(config.child("folder").child_value());
	texture_path = config.child("sprite_sheet").attribute("source").as_string();

	entitiesTextures = vector<SDL_Texture*> (Entity::entityType::WAR_HOUND, nullptr);
	
	return true;
}
bool EntityManager::LoadSamples() {
	//for(int i=Entity::entityType::TOWNHALL;i<Entity::EntityType::WARHOUND;i++){
	//}
	//char *s_type = (type > BARRACKS) ? "troops" : "buildings";
	//name = "ERROR";
	//name = GetName(type);
	//collider = Collider;

	//LOG("Player %d: Loading %s", (isPlayer1) ? 1 : 2, name.data());

	//pugi::xml_document	config_file;
	//pugi::xml_node config;
	//config = App->LoadConfig(config_file);
	//config = config.child("entitymanager").child(s_type).child(name.data());

	//// Parsing data

	//health_lv.push_back(config.child("health").attribute("lvl1").as_uint(0));
	//health_lv.push_back(config.child("health").attribute("lvl2").as_uint(0));
	//health_lv.push_back(config.child("health").attribute("lvl3").as_uint(0));

	//upgrade_cost.push_back(0);
	//upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl2").as_int(0));
	//upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl3").as_int(0));

	//damage_lv.push_back(config.child("damage").attribute("lvl1").as_uint(0));
	//damage_lv.push_back(config.child("damage").attribute("lvl2").as_uint(0));
	//damage_lv.push_back(config.child("damage").attribute("lvl3").as_uint(0));

	//size.first = config.child("size").attribute("width").as_int(6);
	//size.second = config.child("size").attribute("height").as_int(6);

	//rate_of_fire = config.child("attack").attribute("rate").as_float(0);
	//max_targets = config.child("attack").attribute("targets").as_int(0);
	//range = config.child("attack").attribute("range").as_int(0);
	return true;
}
bool EntityManager::Start()
{
	LoadSamples();
	bool ret = true;
	for (int i = Entity::entityType::TOWNHALL; i <= Entity::entityType::WAR_HOUND; i++) {

		string n = GetName(Entity::entityType(i));
		n += "_anim.png";

		entitiesTextures[i] = App->tex->Load(PATH(folder.data(), n.data()));
	}

	return ret;
}
bool EntityManager::PreUpdate()
{
	BROFILER_CATEGORY("EntityManager PostUpdate", Profiler::Color::Blue);

	bool ret = true;

	
	if (App->scene->pause == false)
	{
		// Player 1 Buildings
		list<Building*>::const_iterator tmp = App->player1->buildings.begin();
		while (tmp != App->player1->buildings.end())
		{
			ret = (*tmp)->PreUpdate();
			tmp++;
		}

		// Player 2 Buildings
		tmp = App->player2->buildings.begin();
		while (tmp != App->player2->buildings.end())
		{
			ret = (*tmp)->PreUpdate();
			tmp++;
		}
		// Player 1 Troops
		list<Troop*>::const_iterator ttmp = App->player1->troops.begin();
		while (ttmp != App->player1->troops.end())
		{
			ret = (*ttmp)->PreUpdate();
			ttmp++;
		}

		// Player 2 Troops
		ttmp = App->player2->troops.begin();
		while (ttmp != App->player2->troops.end())
		{
			ret = (*ttmp)->PreUpdate();
			ttmp++;
		}


	}

	return ret;
}


bool EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager Update", Profiler::Color::Blue);

	bool ret = true;

	if (App->scene->pause == false)
	{
		

		// Player 1 Buildings
		list<Building*>::const_iterator tmp = App->player1->buildings.begin();
		while (tmp != App->player1->buildings.end())
		{
			ret = (*tmp)->Update(dt);
			tmp++;
		}

		// Player 2 Buildings
		tmp = App->player2->buildings.begin();
		while (tmp != App->player2->buildings.end())
		{
			ret = (*tmp)->Update(dt);
			tmp++;
		}

		pair<int, int> mouse_pos;
		App->input->GetMousePosition(mouse_pos.first, mouse_pos.second);
		mouse_pos = App->render->ScreenToWorld(mouse_pos.first, mouse_pos.second);

		// Player 1 Troops

		list<Troop*>::const_iterator ttmp = App->player1->troops.begin();

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && ttmp != App->player1->troops.end()) {
			pathfinding = true;
		}
		
		/*if (pathfinding && ttmp != App->player1->troops.end() && (*ttmp)->info.current_group->IsGroupLead(*ttmp)) {
			pathfinding = !App->move_manager->Move((*ttmp)->info.current_group, dt,mouse_pos);
		}	*/	

		
		while (ttmp != App->player1->troops.end())
		{
			ret = (*ttmp)->Update(dt);
			ttmp++;
		}

		// Player 2 Troops
		ttmp = App->player2->troops.begin();

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && ttmp != App->player2->troops.end()) {
			pathfinding2 = true;
		}

		/*if (pathfinding2 && ttmp != App->player2->troops.end()) {
			pathfinding2 = !App->move_manager->Move((*ttmp)->info.current_group, dt, mouse_pos);
		}*/

		while (ttmp != App->player2->troops.end())
		{
			ret = (*ttmp)->Update(dt);
			ttmp++;
		}

	}

	return ret;
}

bool EntityManager::PostUpdate()
{
	BROFILER_CATEGORY("EntityManager PostUpdate", Profiler::Color::Blue);

	bool ret = true;

	if (App->scene->pause == false)
	{
		// Player 1 Buildings
		list<Building*>::const_iterator tmp = App->player1->buildings.begin();
		while (tmp != App->player1->buildings.end())
		{
			ret = (*tmp)->PostUpdate();
			tmp++;
		}

		// Player 2 Buildings
		tmp = App->player2->buildings.begin();
		while (tmp != App->player2->buildings.end())
		{
			ret = (*tmp)->PostUpdate();
			tmp++;
		}
		// Player 1 Troops
		list<Troop*>::const_iterator ttmp = App->player1->troops.begin();
		while (ttmp != App->player1->troops.end())
		{
			ret = (*ttmp)->PostUpdate();
			ttmp++;
		}

		// Player 2 Troops
		ttmp = App->player2->troops.begin();
		while (ttmp != App->player2->troops.end())
		{
			ret = (*ttmp)->PostUpdate();
			ttmp++;
		}


	}

	return ret;
}

bool EntityManager::CleanUp()
{
	App->tex->UnLoad(texture);
	entity_list.clear();

	for (int i = Entity::entityType::TOWNHALL; i < Entity::entityType::WAR_HOUND; i++) {

			if (entitiesTextures[i] = nullptr) {
				App->tex->UnLoad(entitiesTextures[i]);
			}
	}

	//Building::CleanUp();
	return true;
}

void EntityManager::DeleteAllEntities()
{
	// Main entity list
	list<Entity*>::iterator	tmp = entity_list.begin();
	while (tmp != entity_list.end())
	{
		(*tmp)->CleanUp();
		RELEASE(*tmp);
		tmp++;
	}
	entity_list.clear();


	// Player 1 Buildings
	list<Building*>::iterator btmp = App->player1->buildings.begin();
	while (btmp != App->player1->buildings.end())
	{
		(*btmp)->CleanUp();
		RELEASE(*btmp);
		btmp++;
	}
	App->player1->buildings.clear();

	// Player 2 Buildings
	btmp = App->player2->buildings.begin();
	while (btmp != App->player2->buildings.end())
	{
		(*btmp)->CleanUp();
		RELEASE(*btmp);
		btmp++;
	}
	App->player2->buildings.clear();

	// Player 1 Troops
	list<Troop*>::iterator ttmp = App->player1->troops.begin();
	while (ttmp != App->player1->troops.end())
	{
		(*ttmp)->CleanUp();
		RELEASE(*ttmp);
		ttmp++;
	}
	App->player1->troops.clear();

	// Player 2 Troops
	ttmp = App->player2->troops.begin();
	while (ttmp != App->player2->troops.end())
	{
		(*ttmp)->CleanUp();
		RELEASE(*ttmp);
		ttmp++;
	}
	App->player2->troops.clear();

}

bool EntityManager::Draw(float dt) //sprite ordering
{
	bool ret = true;

	list<Entity*>::iterator tmp = entity_list.begin();

	while (tmp != entity_list.end())
	{

		//if (entitiesTextures[(*tmp)->type] != nullptr) {

		//	int posy = (*tmp)->position.second - (*tmp)->Current_Animation->GetCurrentFrame(dt).h;// - ((*tmp)->Current_Animation->GetCurrentFrame(dt).h - (*tmp)->position.second);
		//	App->render->Blit(entitiesTextures[(*tmp)->type],  (*tmp)->position.first ,posy, &((*tmp)->Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_NONE);
		//	
		//	pair<int,int> pos = App->map->WorldToMap((*tmp)->position.first /*- (*tmp)->size.first * App->map->data.tile_width*0.5f*/, (*tmp)->position.second - (*tmp)->size.second*App->map->data.tile_height*0.5f);
		//	pos = App->map->MapToWorld(pos.first, pos.second);

		//	pair<int, int> pos = { (*tmp)->position.first,(*tmp)->position.second - (*tmp)->offset };
		//	App->render->Blit(entitiesTextures[(*tmp)->type], pos.first, pos.second, &((*tmp)->Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_NONE);

		//}
		//if (entitiesTextures[(*tmp)->type] != nullptr)


		if ((*tmp)->type == Entity::entityType::TOWNHALL && (*tmp)->fromPlayer1 == true)
		{
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 8), 
				(*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 40), 
				&((*tmp)->Current_Animation->GetCurrentFrame(dt)));


			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}

		}
		else if (((*tmp)->type == Entity::entityType::TOWNHALL && (*tmp)->fromPlayer1 == false))
		{
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20), (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20), &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 8) + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 40) - 10;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}
		}
		else if ((*tmp)->type == Entity::entityType::BARRACKS)
		{
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + 30, (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10, &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}
		}
		else if ((*tmp)->type == Entity::entityType::MINES)
		{
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20), (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20), &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}
		}
		else if ((*tmp)->type == Entity::entityType::DEFENSE_AOE)
		{
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + 40, (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10, &((*tmp)->Current_Animation->GetCurrentFrame(dt)));


			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}
		}
		else if ((*tmp)->type >= Entity::entityType::SOLDIER)
		{
		App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first - (*tmp)->Current_Animation->frames->w*0.5, (*tmp)->position.second - (*tmp)->Current_Animation->frames->h*0.5, &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

		//--- Draw Life Bar
		if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
		{
			SDL_Rect rect, rect_bg;

			rect_bg.w = 30;
			rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

			rect_bg.h = rect.h = 5;
			rect_bg.x = rect.x = (*tmp)->position.first - ((*tmp)->collider.dimensions.first * 20) + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
			rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10;

			App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
			App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
		}
}
		else
		{
			// Soldier testing with per frame deploying
			if ((*tmp)->type == Entity::entityType::SOLDIER && (*tmp)->info.current_group == nullptr) {
				//App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first/*-((*tmp)->collider.dimensions.first*29)*/, (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20), &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

			}//change print for soldier cases
			else 
 				App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first/*-((*tmp)->collider.dimensions.first*29)*/, (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second*20), &((*tmp)->Current_Animation->GetCurrentFrame(dt)));
		
			//--- Draw Life Bar
			if ((*tmp)->health < (*tmp)->health_lv[(*tmp)->level] && (*tmp)->health > 0)
			{
				SDL_Rect rect, rect_bg;

				rect_bg.w = 30;
				rect.w = rect_bg.w * (*tmp)->health / (*tmp)->health_lv[(*tmp)->level];

				rect_bg.h = rect.h = 5;
				rect_bg.x = rect.x = (*tmp)->position.first + ((*tmp)->Current_Animation->GetCurrentFrame(dt).w / 2) - (rect_bg.w / 1.5);
				rect_bg.y = rect.y = (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20) - 10;

				App->render->DrawQuad(rect_bg, 255, 0, 0, 255); //background (red)
				App->render->DrawQuad(rect, 0, 255, 0, 255); //life (green)
			}
		}
			

		

		tmp++;
	}


	return ret;
}
Entity* EntityManager::AddEntity(bool isPlayer1, Entity::entityType type, pair<int, int> position, Collider collider, Animation* animation)
{
	BROFILER_CATEGORY("EntityManager AddEntity", Profiler::Color::AliceBlue);

	Entity* tmp = nullptr;

	if (type > Entity::entityType::BARRACKS) //if troops
	{
		switch (type)
		{
		case Entity::entityType::SOLDIER:
			tmp = new Soldier(isPlayer1, position, collider);
			break;

		case Entity::entityType::ENGINEER:
			tmp = new Engineer(isPlayer1, position, collider);
			break;

		case Entity::entityType::TANKMAN:
			tmp = new Tankman(isPlayer1, position, collider);
			break;

		case Entity::entityType::INFILTRATOR:
			tmp = new Infiltrator(isPlayer1, position, collider);
			break;

		case Entity::entityType::WAR_HOUND:
			tmp = new Hound(isPlayer1, position, collider);
			break;

		default:
			break;
		}
		if (tmp)
		{
			if (isPlayer1 == true)
			{
				App->player1->troops.push_back((Troop*)tmp);
				App->player1->entities.push_back((Troop*)tmp);
			}
			else {
				App->player2->troops.push_back((Troop*)tmp);
				App->player2->entities.push_back((Troop*)tmp);
			}
		
			entity_list.push_back(tmp); // add to main entity list
			OrderEntities();

		}
	}
	// if building
	else {
		switch (type)
		{
		case Entity::entityType::TOWNHALL:
			tmp = new Townhall(isPlayer1, position, collider);
			break;

		case Entity::entityType::MAIN_DEFENSE: // this is the actual sentrygun
			tmp = new MainDefense(isPlayer1, position, collider);
			App->audio->PlayFx(SENTRYGUN_BUILD);
			break;

		case Entity::entityType::COMMAND_CENTER:
			tmp = new CmdCenter(isPlayer1, position, collider);
			break;

		case Entity::entityType::DEFENSE_AOE:
			//tesla
			App->audio->PlayFx(TESLA_BUILD);
			tmp = new DefenseAoe(isPlayer1, position, collider);
			break;

		case Entity::entityType::DEFENSE_TARGET:
			App->audio->PlayFx(SENTRYGUN_BUILD);
			tmp = new DefenseTarget(isPlayer1, position, collider);
			break;

		case Entity::entityType::MINES:
			App->audio->PlayFx(MINE_BUILD);
			tmp = new Mines(isPlayer1, position, collider);
			break;

		case Entity::entityType::BARRACKS:
			App->audio->PlayFx(BARRACKS_BUILD);
			tmp = new Barracks(isPlayer1, position, collider);
			break;

		case Entity::entityType::WALLS:
			tmp = new Walls(isPlayer1, position, collider, animation);
			break;

		}
		if (tmp) {
			if (isPlayer1 == true)
			{
				App->player1->gold -= App->player1->CheckCost(tmp->type);
				App->player1->buildings.push_back((Building*)tmp);
				App->player1->buildings = OrderBuildings(App->player1->buildings, true);
				App->player1->entities.push_back((Building*)tmp);
				App->player1->UpdateWalkabilityMap(P1_BUILDING, collider);

			}
			else // Player 2 -------------------------------
			{
				App->player2->gold -= App->player2->CheckCost(tmp->type);
				App->player2->buildings.push_back((Building*)tmp);
				App->player2->buildings = OrderBuildings(App->player2->buildings, true);
				App->player2->entities.push_back((Building*)tmp);
				App->player2->UpdateWalkabilityMap(P2_BUILDING, collider);

			}
			entity_list.push_back(tmp); // add to main entity list
			OrderEntities();
		}
	}

	return tmp;
}

char* EntityManager::GetName(Entity::entityType type) {
	switch (type)
	{
	case Entity::entityType::TOWNHALL:
		return"Townhall";
		break;
	case Entity::entityType::MAIN_DEFENSE:
		return"sentrygun";
		break;
	case Entity::entityType::COMMAND_CENTER:
		return"CommandCenter";
		break;
	case Entity::entityType::WALLS:
		return"Walls";
		break;
	case Entity::entityType::DEFENSE_AOE:
		return"aoe";
		break;
	case Entity::entityType::DEFENSE_TARGET:
		return"Tesla";
		break;
	case Entity::entityType::MINES:
		return"GoldMine";
		break;
	case Entity::entityType::BARRACKS:
		return"Barracks";
		break;
	case Entity::entityType::SOLDIER:
		return"BasicSoldier";
		break;
	case Entity::entityType::TANKMAN:
		return"tankman";
		break;
	case Entity::entityType::INFILTRATOR:
		return"infiltrator";
		break;
	case Entity::entityType::ENGINEER:
		return"engineer";
		break;
	case Entity::entityType::WAR_HOUND:
		return"war_hound";
		break;
	default:
		break;
	}
}

void EntityManager::OrderEntities()
{
	BROFILER_CATEGORY("EntityManager OrderEntities", Profiler::Color::Blue);
	priority_queue<Entity*, vector<Entity*>, Sorting> ListOrder;

	for (list<Entity*>::iterator tmp = entity_list.begin(); tmp != entity_list.end(); tmp++) //push entity_list items to Ordered List
	{
		ListOrder.push(*tmp);
	}

	entity_list.clear(); //clear unordered list

	while (ListOrder.empty() == false) //push ordered list to entity_list
	{
		entity_list.push_front(ListOrder.top());
		ListOrder.pop();
	}
}

list<Building*> EntityManager::OrderBuildings(list<Building*> List, bool isPlayer1)
{
	list<Building*> ListOrder;
	ListOrder.push_back(List.front()); // push first element of List to OrderList

	bool found = false;

	for (list<Building*>::iterator tmp = List.begin(); tmp != List.end(); tmp++) // traverse entity list (unordered)
	{
		for (list<Building*>::iterator tmp2 = ListOrder.begin(); tmp2 != ListOrder.end(); tmp2++) // traverse Ordered List
		{
			if (isPlayer1)
			{
				if (GetDepth(*tmp) < GetDepth(*tmp2)) // if tmp is further than tmp2
				{
					if (FindInList2(ListOrder, (*tmp)->position, (*tmp)->fromPlayer1, (*tmp)->type) == false)
					{
						ListOrder.insert(tmp2, *tmp); // add tmp in front of tmp2
						found = true;
					}

					break;
				}
			}
			else
			{
				if (GetDepth(*tmp) > GetDepth(*tmp2)) // if tmp is further than tmp2
				{
					if (FindInList2(ListOrder, (*tmp)->position, (*tmp)->fromPlayer1, (*tmp)->type) == false)
					{
						ListOrder.insert(tmp2, *tmp); // add tmp in front of tmp2
						found = true;
					}

					break;
				}
			}
			
		}
		if (found == false) // if tmp is the closest
		{
			if (FindInList2(ListOrder, (*tmp)->position, (*tmp)->fromPlayer1, (*tmp)->type) == false)
			{
				ListOrder.push_back(*tmp); // push to last place	
			}
		}
		found = false;
	}

	return ListOrder;
}

int EntityManager::GetDepth(Entity* entity)
{
	BROFILER_CATEGORY("Get Depth", Profiler::Color::Blue);
	pair<int,int> postemp = App->map->WorldToMap(entity->position.first, entity->position.second); // get map coords

	return (postemp.first + postemp.second); // return depth
}


Entity* EntityManager::findEntity(pair <int,int> pos,bool fromplayer1, int attackrange)
{
	Player* enemy = (!fromplayer1)?App->player1:App->player2;

	Entity* found = *enemy->entities.begin();
	int distance = 0;
	pair<int, int> map_pos = App->map->WorldToMap(found->position.first, found->position.second);
	Is_inRange(pos,distance,map_pos,attackrange);
	int min_dist = distance;

	for (list<Entity*>::iterator tmp = enemy->entities.begin(); tmp != enemy->entities.end(); tmp++) // traverse entity list (unordered)
	{
		map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);
		
		if (Is_inRange(pos,distance,map_pos,attackrange))
		{

			if (min_dist >= distance )
			{
				found = (*tmp);
				min_dist = distance;
			}
		}
		
	}
	if (min_dist <= attackrange)
	{
		return found;
	}

	return nullptr;
	
}

bool EntityManager::Is_inRange(pair<int, int> pos, int &distance, pair <int,int> position, int range) {

	//posicion entre dos entidades cualquiera
	//determina si esta en el rango

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second/2, 2)));

	return distance <= range;
}

//void EntityManager:: Remove_fromList(Entity* entity)
//{
//	RELEASE(entity);
//	entity_list.remove(entity);
//}
//
//void EntityManager::Erase_fromList(list<Entity*>::iterator tmp)
//{
//	
//	entity_list.erase(tmp);
//}

bool EntityManager::FindInList(list<Entity*> List, pair <int,int> pos, bool fromplayer1, Entity::entityType type)
{
	BROFILER_CATEGORY("FindInList", Profiler::Color::Blue);
	bool ret = false;
	for (list<Entity*>::iterator tmp = List.begin(); tmp != List.end(); tmp++) // traverse entity list (unordered)
	{
		if ((*tmp)->fromPlayer1 == fromplayer1 && (*tmp)->position == pos && (*tmp)->type == type)
		{
			ret = true;
			break;
		}
	}
	return ret;
}

bool EntityManager::FindInList2(list<Building*> List, pair <int, int> pos, bool fromplayer1, Entity::entityType type)
{
	bool ret = false;
	for (list<Building*>::iterator tmp = List.begin(); tmp != List.end(); tmp++) // traverse entity list (unordered)
	{
		if ((*tmp)->fromPlayer1 == fromplayer1 && (*tmp)->position == pos && (*tmp)->type == type)
		{
			ret = true;
			break;
		}
	}
	return ret;
}
