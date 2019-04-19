#ifndef __Entity_H__
#define __Entity_H__

#include "Log.h"
#include "Animation.h"
#include "Map.h"
#include "App.h"
#include "Animation.h"
#include "Render.h"

struct Collider
{
	pair<int, int> dimensions;
	vector<pair<int, int>> tiles;

	
};

class Entity
{
public:
	enum entityType
	{
		TOWNHALL,
		MAIN_DEFENSE,
		COMMAND_CENTER,
		WALLS,
		DEFENSE_AOE,
		DEFENSE_TARGET,
		MINES,
		BARRACKS,
		SOLDIER,
		TANKMAN,
		INFILTRATOR,
		ENGINEER,
		WAR_HOUND
	};

public:
	Entity() {};
	Entity(Entity::entityType type, bool isPlayer1, pair<int, int> pos, Collider Collider) {
		this->type = type;
	
		char *s_type = (type > BARRACKS) ? "troops" : "buildings";
		name = "ERROR";
		name = GetName(type);

		for (int i = 0; i < Collider.tiles.size(); i++) {

			collider.tiles[i].first = Collider.tiles[i].first;
			collider.tiles[i].second = Collider.tiles[i].second;
		}

		LOG("Player %d: Loading %s",(isPlayer1)?1:2,name.data());

		pugi::xml_document	config_file;
		pugi::xml_node config;
		config = App->LoadConfig(config_file);
		config = config.child("entitymanager").child(s_type).child(name.data());

		// Parsing data

		// name is given by entity type
		//name = config.child("name").attribute("string").as_string("ERROR");

		health_lv.push_back(config.child("health").attribute("lvl1").as_uint(0));
		health_lv.push_back(config.child("health").attribute("lvl2").as_uint(0));
		health_lv.push_back(config.child("health").attribute("lvl3").as_uint(0));

		upgrade_cost.push_back(0);
		upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl2").as_int(0));
		upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl3").as_int(0));

		damage_lv.push_back(config.child("damage").attribute("lvl1").as_uint(0));
		damage_lv.push_back(config.child("damage").attribute("lvl2").as_uint(0));
		damage_lv.push_back(config.child("damage").attribute("lvl3").as_uint(0));

		size.first = config.child("size").attribute("width").as_int(6);
		size.second = config.child("size").attribute("height").as_int(6);
		
		collider.dimensions.first = size.first;
		collider.dimensions.second = size.second;

		rate_of_fire = config.child("attack").attribute("rate").as_float(0);
		max_targets  = config.child("attack").attribute("targets").as_int(0);
		range		 = config.child("attack").attribute("range").as_int(0);

		position	= pos;

		fromPlayer1 = isPlayer1;
		upgrade		= false;
		repair		= false;

		level		= 0;
		
		health		= health_lv[level];
		damage		= damage_lv[level];

		//ChangeAnimation();

		// DEBUG PURPOSES
		tex = App->tex->Load("maps/meta.png");
	};

	~Entity() {};
	virtual bool Awake(pugi::xml_node & config) { return true; };
	virtual bool Start() { return true; };
	virtual bool PreUpdate() { 	return true; };
	virtual bool Update(float dt) { 
		
		return true; };
	virtual bool PostUpdate(
	
	) { return true; };

	virtual void CleanUp() {
		health_lv.clear();
		health_lv.resize(0);

		upgrade_cost.clear();
		upgrade_cost.resize(0);

		damage_lv.clear();
		damage_lv.resize(0);

	};
	virtual void Save(pugi::xml_node& file) const {};
	virtual void Load(pugi::xml_node& file) {};
	virtual void Restart() {};
	virtual void LoadAnimations(bool isPlayer1, string path) {};

	// Damage animation does not care about levels(?)
	virtual void ChangeAnimation() {
		if (health <= 0)
			Current_Animation = destroyed;

		else if (health < (health_lv[level] / 2))
			Current_Animation = damaged;

		else if (level == 0)
			Current_Animation = level1;

		else if (level == 1)
			Current_Animation = level2;

		else if (level == 2)
			Current_Animation = level3;
	};

	virtual void TakeDamage(int damage) {
		if (alive) {

			health -= damage;

			if (health <= 0) {
				alive = false;
				LOG("P[%d] %s DEAD", (fromPlayer1) ? 1 : 2, &name[0]);
			}

			LOG("P[%d] %s health: %d", (fromPlayer1)?1:2, &name[0], health);
		}
		
	};

	pair<int, int> GetSize(Entity* entity) { return entity->size; };
	pair<int, int> GetPos(Entity* entity) { return entity->position; };

public:
	Animation*	Current_Animation = nullptr;
	Animation*	building = nullptr;
	Animation*	level1 = nullptr;
	Animation*	level2 = nullptr;
	Animation*	level3 = nullptr;
	Animation*	damaged = nullptr;
	Animation*	destroyed = nullptr;

	entityType	type;
	string		name;

	vector<uint>	health_lv;
	vector<uint>	damage_lv;
	vector<uint>	upgrade_cost;

	pair<int, int> position;
	pair<int,int> size;
	
	Timer timer;

	bool fromPlayer1;
	bool upgrade;
	bool repair;
	bool flip	= false;
	bool alive	= true;
	
	int health;
	int damage;
	int level;
	int range;
	int repair_cost;
	int frame = 0;
	int max_targets;

	float rate_of_fire;

	// DEBUG PURPOSES
	SDL_Rect	debug_collider;
	SDL_Texture* tex;

	int offset = 0;
	Collider collider;

public:

		char* GetName(const Entity::entityType& type) {
			switch (type)
			{
			case Entity::entityType::TOWNHALL:
				return"townhall";
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
				return"defense_aoe";
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
};

#endif