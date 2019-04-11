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

#include "Brofiler\Brofiler.h"
#include "PugiXml/src/pugixml.hpp"

EntityManager::EntityManager()
{
	name.append("EntityManager");
}

EntityManager::~EntityManager()
{
}

bool EntityManager::Awake(pugi::xml_node &config)
{
	bool ret = true;
	folder.append(config.child("folder").child_value());
	texture_path = config.child("sprite_sheet").attribute("source").as_string();

	return ret;
}

bool EntityManager::Start()
{
	bool ret = true;
	texture = App->tex->Load(PATH(folder.data(), texture_path.data()));

	return ret;
}

bool EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager Update", Profiler::Color::Blue);

	bool ret = true;

	if (App->scene->pause == false)
	{
		// Player 1 Buildings
		list<Entity*>::const_iterator tmp = App->player1->buildings.begin();
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

		// Player 1 Troops
		tmp = App->player1->troops.begin();
		while (tmp != App->player1->troops.end())
		{
			ret = (*tmp)->Update(dt);
			tmp++;
		}

		// Player 2 Troops
		tmp = App->player2->troops.begin();
		while (tmp != App->player2->troops.end())
		{
			ret = (*tmp)->Update(dt);
			tmp++;
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
		list<Entity*>::const_iterator tmp = App->player1->buildings.begin();
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


	}
	
	return ret;
}

bool EntityManager::CleanUp()
{
	App->tex->UnLoad(texture);
	entity_list.clear();

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
	tmp = App->player1->buildings.begin();
	while (tmp != App->player1->buildings.end())
	{
		(*tmp)->CleanUp();
		RELEASE(*tmp);
		tmp++;
	}
	App->player1->buildings.clear();

	// Player 2 Buildings
	tmp = App->player2->buildings.begin();
	while (tmp != App->player2->buildings.end())
	{
		(*tmp)->CleanUp();
		RELEASE(*tmp);
		tmp++;
	}
	App->player2->buildings.clear();
}

bool EntityManager::Draw(float dt) //sprite ordering
{
	bool ret = true;

	list<Entity*>::iterator tmp = entity_list.begin();
	while (tmp != entity_list.end())
	{
		//App->render->Blit(texture, (*tmp)->position.first, (*tmp)->position.second, &((*tmp)->Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_NONE);
		tmp++;
	}
	return ret;
}

Entity* EntityManager::AddEntity(bool isPlayer1, Entity::entityType type, pair<int, int> position)
{
	Entity* tmp = nullptr;

	switch (type)
	{
	case Entity::entityType::TOWNHALL:
		tmp = new Townhall(isPlayer1, position);
		break;

	case Entity::entityType::MAIN_DEFENSE:
		tmp = new MainDefense(isPlayer1, position);
		break;

	case Entity::entityType::COMMAND_CENTER:
		tmp = new CmdCenter(isPlayer1, position);
		break;

	case Entity::entityType::WALLS:
		tmp = new Walls(isPlayer1, position);
		break;

	case Entity::entityType::DEFENSE_AOE:
		tmp = new DefenseAoe(isPlayer1, position);
		break;

	case Entity::entityType::DEFENSE_TARGET:
		tmp = new DefenseTarget(isPlayer1, position);
		break;

	case Entity::entityType::MINES:
		tmp = new Mines(isPlayer1, position);
		break;

	case Entity::entityType::BARRACKS:
		tmp = new Barracks(isPlayer1, position);
		break;
	case Entity::entityType::SOLDIER:
		tmp = new Soldier(isPlayer1, position);
		break;
	case Entity::entityType::ENGINEER:
		//tmp = new Engineer(isPlayer1, position);
		break;
	case Entity::entityType::TANKMAN:
		//tmp = new Tankman(isPlayer1, position);
		break;
	case Entity::entityType::INFILTRATOR:
		//tmp = new Infiltrator(isPlayer1, position);
		break;
	case Entity::entityType::WAR_HOUND:
		//tmp = new War_hound(isPlayer1, position);
		break;
	}

	if (tmp)
	{
		entity_list.push_back(tmp); // add to main entity list
		if (isPlayer1 == true)
		{
			if (type >= Entity::entityType::TOWNHALL && type <= Entity::entityType::BARRACKS) //if building
			{
				App->player1->buildings.push_back(tmp);
			}
			else if (type > Entity::entityType::BARRACKS) //if troops
			{
				App->player1->troops.push_back(tmp);
			}
		}
		else // Player 2 -------------------------------
		{
			if (type >= Entity::entityType::TOWNHALL && type <= Entity::entityType::BARRACKS)
			{
				App->player2->buildings.push_back(tmp);
			}
			else if (type > Entity::entityType::BARRACKS)
			{
				App->player2->troops.push_back(tmp);
			}
		}
	}
	return tmp;
}