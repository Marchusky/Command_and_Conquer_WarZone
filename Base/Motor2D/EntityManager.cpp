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
	LoadSamples();
	return true;
}
bool EntityManager::LoadSamples() {

	string path = "animation/BasicSoldier.tmx";

	soldier_p1_sample.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	soldier_p1_sample.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	
	soldier_p1_sample.idle = soldier_p1_sample.idle->LoadAnimation(path.data(), (true) ? "red_idle" : "blue_idle");
	
	soldier_p1_sample.moving[NORTH]			= soldier_p1_sample.moving[NORTH]->LoadAnimation(path.data(), (true) ? "red_north" : "blue_north");
	soldier_p1_sample.moving[SOUTH]			= soldier_p1_sample.moving[SOUTH]->LoadAnimation(path.data(), (true) ? "red_south" : "blue_south");
	soldier_p1_sample.moving[EAST]			= soldier_p1_sample.moving[EAST]->LoadAnimation(path.data(), (true) ? "red_east" : "blue_east");
	soldier_p1_sample.moving[WEST]			= soldier_p1_sample.moving[WEST]->LoadAnimation(path.data(), (true) ? "red_west" : "blue_west");
	soldier_p1_sample.moving[NORTHEAST]		= soldier_p1_sample.moving[NORTHEAST]->LoadAnimation(path.data(), (true) ? "red_northeast" : "blue_northeast");
	soldier_p1_sample.moving[NORTHWEST]		= soldier_p1_sample.moving[NORTHWEST]->LoadAnimation(path.data(), (true) ? "red_northwest" : "blue_northwest");
	soldier_p1_sample.moving[SOUTHEAST]		= soldier_p1_sample.moving[SOUTHEAST]->LoadAnimation(path.data(), (true) ? "red_southeast" : "blue_southeast");
	soldier_p1_sample.moving[SOUTHWEST]		= soldier_p1_sample.moving[SOUTHWEST]->LoadAnimation(path.data(), (true) ? "red_southwest" : "blue_southwest");
	
	soldier_p1_sample.shooting[NORTH]		= soldier_p1_sample.shooting[NORTH]->LoadAnimation(path.data(), (true) ? "red_shoot_N" : "blue_shoot_N");
	soldier_p1_sample.shooting[SOUTH]		= soldier_p1_sample.shooting[SOUTH]->LoadAnimation(path.data(), (true) ? "red_shoot_S" : "blue_shoot_S");
	soldier_p1_sample.shooting[EAST]		= soldier_p1_sample.shooting[EAST]->LoadAnimation(path.data(), (true) ? "red_shoot_E" : "blue_shoot_E");
	soldier_p1_sample.shooting[WEST]		= soldier_p1_sample.shooting[WEST]->LoadAnimation(path.data(), (true) ? "red_shoot_W" : "blue_shoot_W");
	soldier_p1_sample.shooting[NORTHEAST]	= soldier_p1_sample.shooting[NORTHEAST]->LoadAnimation(path.data(), (true) ? "red_shoot_NE" : "blue_shoot_NE");
	soldier_p1_sample.shooting[NORTHWEST]	= soldier_p1_sample.shooting[NORTHWEST]->LoadAnimation(path.data(), (true) ? "red_shoot_NW" : "blue_shoot_NW");
	soldier_p1_sample.shooting[SOUTHEAST]	= soldier_p1_sample.shooting[SOUTHEAST]->LoadAnimation(path.data(), (true) ? "red_shoot_SE" : "blue_shoot_SE");
	soldier_p1_sample.shooting[SOUTHWEST]	= soldier_p1_sample.shooting[SOUTHWEST]->LoadAnimation(path.data(), (true) ? "red_shoot_SW" : "blue_shoot_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		soldier_p1_sample.moving[i]->speed = 10;
		soldier_p1_sample.shooting[i]->speed = 6;
	}

	soldier_p1_sample.idle->speed = 0;
	soldier_p1_sample.idle->SetCurrentFrame(2);
	soldier_p1_sample.Current_Animation = soldier_p1_sample.moving[NORTH];


	//Player 2
	soldier_p2_sample.moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	soldier_p2_sample.shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	soldier_p2_sample.idle = soldier_p2_sample.idle->LoadAnimation(path.data(), (false) ? "red_idle" : "blue_idle");

	soldier_p2_sample.moving[NORTH] = soldier_p2_sample.moving[NORTH]->LoadAnimation(path.data(), (false) ? "red_north" : "blue_north");
	soldier_p2_sample.moving[SOUTH] = soldier_p2_sample.moving[SOUTH]->LoadAnimation(path.data(), (false) ? "red_south" : "blue_south");
	soldier_p2_sample.moving[EAST] = soldier_p2_sample.moving[EAST]->LoadAnimation(path.data(), (false) ? "red_east" : "blue_east");
	soldier_p2_sample.moving[WEST] = soldier_p2_sample.moving[WEST]->LoadAnimation(path.data(), (false) ? "red_west" : "blue_west");
	soldier_p2_sample.moving[NORTHEAST] = soldier_p2_sample.moving[NORTHEAST]->LoadAnimation(path.data(), (false) ? "red_northeast" : "blue_northeast");
	soldier_p2_sample.moving[NORTHWEST] = soldier_p2_sample.moving[NORTHWEST]->LoadAnimation(path.data(), (false) ? "red_northwest" : "blue_northwest");
	soldier_p2_sample.moving[SOUTHEAST] = soldier_p2_sample.moving[SOUTHEAST]->LoadAnimation(path.data(), (false) ? "red_southeast" : "blue_southeast");
	soldier_p2_sample.moving[SOUTHWEST] = soldier_p2_sample.moving[SOUTHWEST]->LoadAnimation(path.data(), (false) ? "red_southwest" : "blue_southwest");

	soldier_p2_sample.shooting[NORTH] = soldier_p2_sample.shooting[NORTH]->LoadAnimation(path.data(), (false) ? "red_shoot_N" : "blue_shoot_N");
	soldier_p2_sample.shooting[SOUTH] = soldier_p2_sample.shooting[SOUTH]->LoadAnimation(path.data(), (false) ? "red_shoot_S" : "blue_shoot_S");
	soldier_p2_sample.shooting[EAST] = soldier_p2_sample.shooting[EAST]->LoadAnimation(path.data(), (false) ? "red_shoot_E" : "blue_shoot_E");
	soldier_p2_sample.shooting[WEST] = soldier_p2_sample.shooting[WEST]->LoadAnimation(path.data(), (false) ? "red_shoot_W" : "blue_shoot_W");
	soldier_p2_sample.shooting[NORTHEAST] = soldier_p2_sample.shooting[NORTHEAST]->LoadAnimation(path.data(), (false) ? "red_shoot_NE" : "blue_shoot_NE");
	soldier_p2_sample.shooting[NORTHWEST] = soldier_p2_sample.shooting[NORTHWEST]->LoadAnimation(path.data(), (false) ? "red_shoot_NW" : "blue_shoot_NW");
	soldier_p2_sample.shooting[SOUTHEAST] = soldier_p2_sample.shooting[SOUTHEAST]->LoadAnimation(path.data(), (false) ? "red_shoot_SE" : "blue_shoot_SE");
	soldier_p2_sample.shooting[SOUTHWEST] = soldier_p2_sample.shooting[SOUTHWEST]->LoadAnimation(path.data(), (false) ? "red_shoot_SW" : "blue_shoot_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		soldier_p2_sample.moving[i]->speed = 10;
		soldier_p2_sample.shooting[i]->speed = 6;
	}

	soldier_p2_sample.idle->speed = 0;
	soldier_p2_sample.idle->SetCurrentFrame(2);
	soldier_p2_sample.Current_Animation = soldier_p2_sample.moving[NORTH];
	return true;
}
bool EntityManager::Start()
{
	LoadSamples();
	bool ret = true;
	for (int i = Entity::entityType::TOWNHALL; i < Entity::entityType::WAR_HOUND; i++) {

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
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first-((*tmp)->collider.dimensions.first*20), (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20), &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

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
			App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first-((*tmp)->collider.dimensions.first*8), (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 40), &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

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
		else if ((*tmp)->type == Entity::entityType::BARRACKS || (*tmp)->type == Entity::entityType::MINES)
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
		else
		{
			// Soldier testing with per frame deploying
			if ((*tmp)->type == Entity::entityType::SOLDIER && (*tmp)->info.current_group == nullptr) {
				//App->render->Blit(entitiesTextures[(*tmp)->type], (*tmp)->position.first/*-((*tmp)->collider.dimensions.first*29)*/, (*tmp)->position.second - (*tmp)->Current_Animation->frames->h + ((*tmp)->collider.dimensions.second * 20), &((*tmp)->Current_Animation->GetCurrentFrame(dt)));

			}
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

void EntityManager::LoadAnimationSoldier(Troop* troop) {
	troop->moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	troop->shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	Soldier* sample = (troop->fromPlayer1)? &soldier_p1_sample: &soldier_p2_sample;

	if (troop->fromPlayer1) {
		troop->idle = sample->idle;
		troop->idle->speed = 0;
		troop->idle->SetCurrentFrame(2);
		for (int i = NORTH; i < MAX_DIR; i++) {
			troop->moving[i] = sample->moving[i];
			troop->shooting[i] = sample->shooting[i];
			troop->moving[i]->speed = 0.75;
			troop->shooting[i]->speed = 0.75;
		}
	}
	troop->Current_Animation = troop->moving[NORTH];
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
			LoadAnimationSoldier((Troop*)tmp);
			break;

		case Entity::entityType::ENGINEER:
			//tmp = new Engineer(isPlayer1, position, collider);
			break;

		case Entity::entityType::TANKMAN:
			//tmp = new Tankman(isPlayer1, position, collider);
			break;

		case Entity::entityType::INFILTRATOR:
			//tmp = new Infiltrator(isPlayer1, position, collider);
			break;

		case Entity::entityType::WAR_HOUND:
			//tmp = new War_hound(isPlayer1, position, collider);
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

		case Entity::entityType::WALLS:
			tmp = new Walls(isPlayer1, position, collider, animation);
			break;

		case Entity::entityType::BARRACKS:
			App->audio->PlayFx(BARRACKS_BUILD);
			tmp = new Barracks(isPlayer1, position, collider);
			break;
		}
		if (tmp) {
			if (isPlayer1 == true)
			{
				App->player1->gold -= App->player1->CheckCost(tmp);
				App->player1->buildings.push_back((Building*)tmp);
				App->player1->entities.push_back((Building*)tmp);
				App->player1->UpdateWalkabilityMap(P1_BUILDING, collider);

			}
			else // Player 2 -------------------------------
			{

				App->player2->buildings.push_back((Building*)tmp);
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

