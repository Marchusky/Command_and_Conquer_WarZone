#ifndef  __Troop_H__
#define  __Troop_H__

#include "Entity.h"
#include "Group.h"
#include "Render.h"


enum TroopDir {
	NORTH,
	SOUTH,
	EAST,
	WEST,
	NORTHEAST,
	NORTHWEST,
	SOUTHEAST,
	SOUTHWEST,

	MAX_DIR
};


class Troop : public Entity
{
public:

	Troop() {};
	Troop(entityType type, bool isPlayer1, pair<int, int> pos, Collider Collider) : Entity(type, isPlayer1, pos, Collider) {
		BROFILER_CATEGORY("Troop constructor", Profiler::Color::LimeGreen);
		pugi::xml_document	config_file;
		pugi::xml_node config;
		config = App->LoadConfig(config_file);
		config = config.child("entitymanager").child("troops").child(name.data());

		speed = config.child("speed").attribute("value").as_float();
		collider = Collider;
		state = TROOP_IDLE;
		init_position = pos;
		

	}
	~Troop() {};

	void CleanUp() {
		Entity::CleanUp();
	}
	bool Update(float dt) {

		//if (isSelected) 
		//{
		//	SDL_Rect r = Current_Animation->GetCurrentFrame(dt);
		//	r.x = position.first;
		//	r.y = position.second;
		//	App->render->DrawQuad(r, 255, 0, 0, 255,false);
		//	//LOG("SELECTED");
		//}
		

		return true;
	}

public:

	
	

	Animation* idle = nullptr;
	vector<Animation*> moving;
	vector<Animation*> shooting;

	Animation* idle_inv = nullptr;
	vector<Animation*> moving_inv;
	vector<Animation*> shooting_inv;

	int curr = 0;

	vector<pair<int, int>> path;
	int path_count = 1;


	
	bool isInvulnerable = false;
	bool offensive_mode = false;
	float time_to_awake = 2.0f;
	bool lead = false;
	bool pathfind = false;
	bool inmune = false;

	
	
	pair<int, int> init_position;

	TroopDir facing;



	// CHANGED TO TROOP STATE
	/*bool isMoving = false;
	bool isShooting = false;*/
};

#endif