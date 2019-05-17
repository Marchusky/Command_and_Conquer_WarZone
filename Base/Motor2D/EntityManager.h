#ifndef __EntityManager_H__
#define __EntityManager_H__

#include "Module.h"
#include "Entity.h"
#include "Walls.h"

class Entity;

struct Sorting
{
	bool operator()(Entity* const & entity1, Entity* const & entity2) //true if is printed before
	{
		pair<int, int> real_pos1, real_pos2;
		real_pos1 = App->map->WorldToMap(entity1->position.first, entity1->position.second);
		real_pos2 = App->map->WorldToMap(entity2->position.first, entity2->position.second);

		if (real_pos1.first + real_pos1.second < real_pos2.first + real_pos2.second)
			return true;

		else if (real_pos1.first + real_pos1.second > real_pos2.first + real_pos2.second)
			return false;

		else if (real_pos1.first + real_pos1.second == real_pos2.first + real_pos2.second)
		{
			if (real_pos1.second > real_pos2.second) //right to left
				return true;

			else
				return false;
		}
	}
};


class EntityManager : public Module
{
public:
	EntityManager();
	~EntityManager();

	bool Awake(pugi::xml_node & config);
	bool LoadSamples();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	bool Draw(float dt);

	Entity* AddEntity(bool player1, Entity::entityType type, pair<int, int> position, Collider collider, Animation* animation = nullptr);
	char * GetName(Entity::entityType type);
	void DeleteAllEntities();


	list<Building*> OrderBuildings(list<Building*> List, bool isPlayer1);

	void OrderEntities();
	int GetDepth(Entity* entity);



	Entity * findEntity(pair<int, int> pos, bool fromplayer1, int attackrange);

	//bool Is_inRange(pair<int, int> pos, int distance, pair<int, int> position, int range);

	bool Is_inRange(pair<int, int> pos, int & distance, pair<int, int> position, int range);

	//void Remove_fromList(Entity * entity);

	//void Erase_fromList(list<Entity*>::iterator tmp);

	bool FindInList(list<Entity*> List, pair<int, int> pos, bool fromplayer1, Entity::entityType type);
	bool FindInList2(list<Building*> List, pair<int, int> pos, bool fromplayer1, Entity::entityType type);




public:
	//Soldier			soldierSample;

	string			folder;
	string			texture_path;
	SDL_Texture*	texture = nullptr;

	list<Entity*>	entity_list;
	vector<SDL_Texture*> entitiesTextures;

	bool draw_path = false;
	bool godmode = false;
	bool pathfinding = false;
	bool pathfinding2 = false;
};

#endif //