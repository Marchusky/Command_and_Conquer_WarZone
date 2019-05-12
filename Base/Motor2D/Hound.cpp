#include "Hound.h"
#include "Pathfinding.h"
#include "Player.h"
#include "Audio.h"


Hound::Hound()
{

}

Hound::Hound(bool isPlayer1, pair<int, int> pos, Collider collider) :Troop(Entity::entityType::WAR_HOUND, isPlayer1, pos, collider)
{
	BROFILER_CATEGORY("Hound constructor", Profiler::Color::Red);
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
	offset = 50;
	destination = pos;
	original_range = range;
	//Managed in entity.h constructor
	//rate_of_fire = 1;
	//fromPlayer1 = isPlayer1;
	//type = Entity::entityType::SOLDIER;

	state = MOVING;
	timer.Start();
	lead = true;
}


Hound::~Hound()
{
}

bool Hound::Update(float dt)
{
	if (alive) {

		if (lead)
		{
			
			pair <int, int > map_pos = App->map->WorldToMap(position.first, position.second);	

			switch (state)
			{
			case TROOP_IDLE:
				break;

			case MOVING:
				if (pathfind)
				{
					if (info.closest != nullptr)
					{
						Movement(info.closest);
					}
					else
					{
						pathfind = false;
					}
				}
				else if (timer.ReadSec() > 1 && info.closest == nullptr)
				{
					state = SEARCH;
					timer.Start();
				}
				else
				{
					if (offensive)
					{
						position.first += -2;
						position.second += 1;
					}
					else
					{
						position.first += 2;
						position.second += -1;

					}

					
				}
				break;

			case SHOOTING:

				// Shoots the closest one if in range
				if (timer.ReadSec() >= rate_of_fire)
				{
					info.closest->TakeDamage(damage_lv[level]);
					timer.Start();
					App->audio->PlayFx(SOLDIER_ATTACK);
				}
				if (info.closest->health <= 0)
				{
					info.closest->alive = false;
					state = MOVING;
					timer.Start();
					info.closest = nullptr;
				}
				break;
			
			case SEARCH:
				//ally zone
				if (map_pos.first > App->map->allyzone.up_limit.first && map_pos.first < App->map->allyzone.down_limit.first)
				{
					info.closest = FindEntityInAttackRange(map_pos,fromPlayer1,original_range,entityType::SOLDIER);
					
					if (info.closest != nullptr)
					{
						state = SHOOTING;
						timer.Start();
					}
					else if (info.closest == nullptr)
					{
						info.closest = FindNearestEntity(map_pos, fromPlayer1, entityType::SOLDIER);
						SetDestination();
						destination = App->map->MapToWorld(destination.first, destination.second);
						state = MOVING;
						pathfind = true;
					}
					else
					{
						state = MOVING;
						//pathfind = true;
					}
					
					
				}
				// enemy zone
				else if (map_pos.first > App->map->sovietzone.up_limit.first && map_pos.first < App->map->sovietzone.down_limit.first)
				{
					//find entity in range
					//if exist, state= shoot;
					//else, state= move
				} 
				// war zone
				else if (map_pos.first > App->map->warzone.up_limit.first && map_pos.first < App->map->warzone.down_limit.first)
				{
					//find entity en shoot range,
					//if closes exits, state=shoot
					//else state move
				}
				else
				{
					state = MOVING;
					timer.Start();
				}
				break;
		
			default:
				break;
			}

		}
		
		
		
	}
	else {
		//Current_Animation = Die;

	}

	ActOnDestroyed();
	ChangeAnimation();

	//DOES NOT CHANGE ANYTHING BY ITSELF - ONLY INPUT INSIDE -�
	//ForceAnimations();
	//            _|

	Troop::Update(dt);
	return true;
}

void Hound::SetDestination()
{
	destination = App->map->WorldToMap(info.closest->position.first, info.closest->position.second);

	if (!App->pathfinding->IsWalkable(destination)) {
		destination.second -= 1;
		if (!App->pathfinding->IsWalkable(destination)) {
			destination.second += 2;
			if (!App->pathfinding->IsWalkable(destination)) {

				destination.first -= 1;
				destination.second -= 1;
				if (!App->pathfinding->IsWalkable(destination)) {
					destination.first += 2;

				}
			}

		}

	}
}
bool Hound::Is_inRange(pair<int, int> pos, int &distance, pair <int, int> position, int range) {

	//posicion entre dos entidades cualquiera
	//determina si esta en el rango

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second / 2, 2)));

	return distance <= range;
}

void Hound::PrintState() {
	/*switch (state)
	{
	case NOT_DEPLOYED:
		LOG("STATE = NOT_DEPLOYED");

		break;
	case TROOP_IDLE:
		LOG("STATE = IDLE");
		break;
	case MOVING:
		LOG("STATE = MOVING");
		break;
	case SHOOTING:
		LOG("STATE = SHOOTING");
		break;
	case REST:
		LOG("STATE = REST");
		break;
	case MAX_STATE:
		LOG("MOVING");
		break;
	default:
		break;
	}*/
}

void Hound::ForceAnimations() {

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) {
		Current_Animation = moving[(curr++) % SOUTHWEST];

	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) {
		Current_Animation = shooting[(curr++) % SOUTHWEST];

	}
	else if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		Current_Animation = idle;
		idle->SetCurrentFrame((curr++) % SOUTHWEST);
	}
}

void Hound::ActOnDestroyed() {

	if (fromPlayer1)  // --- Player 1 --------------------------------
	{
		if (health <= 0) //destroyed
		{
			
			info.current_group->removeUnit(this);
			App->player1->DeleteEntity(this);
		}
	}
	else if (!fromPlayer1) // --- Player 2 ---------------------------
	{
		if (health <= 0) //destroyed
		{
			info.current_group->removeUnit(this);
			App->player2->DeleteEntity(this);
		}
	}
}

void Hound::CleanUp() {
	Troop::CleanUp();

}

void Hound::Movement(Entity* target)
{
	if (position.first <= target->position.first - offset)
	{
		position.first += 2;
	}
	else if (position.first >= target->position.first + offset)
	{
		position.first -= 2;
	}
	if (position.second <= target->position.second - offset)
	{
		position.second += 1;
	}
	else if (position.second >= target->position.second + offset)
	{
		position.second -= 1;
	}
	if (position.first >= target->position.first - offset && 
		position.first <= target->position.first + offset && 
		position.second >= target->position.second - offset &&
		position.second <= target->position.second + offset)
	{
		state = SHOOTING;
	}
}

void Hound::ChangeAnimation() {
	Current_Animation = idle;
	if (state == MOVING)
	{
		//isShooting = false;
		if (Speed.first == 0 && Speed.second < 0)
		{
			//north
			Current_Animation = moving[NORTH];
		}
		else if (Speed.first == 0 && Speed.second > 0)
		{
			//south
			Current_Animation = moving[SOUTH];
		}
		else if (Speed.first < 0 && Speed.second == 0)
		{
			//west
			Current_Animation = moving[WEST];
		}
		else if (Speed.first > 0 && Speed.second == 0)
		{
			//east
			Current_Animation = moving[EAST];
		}
		else if (Speed.first > 0 && Speed.second < 0)
		{
			//north east
			Current_Animation = moving[NORTHEAST];
		}
		else if (Speed.first > 0 && Speed.second > 0)
		{
			//south east
			Current_Animation = moving[SOUTHEAST];
		}
		else if (Speed.first < 0 && Speed.second < 0)
		{
			//north west
			Current_Animation = moving[NORTHWEST];
		}
		else if (Speed.first < 0 && Speed.second > 0)
		{
			//south wst
			Current_Animation = moving[SOUTHWEST];
		}
	}
	else if (state == SHOOTING)
	{

		if (fromPlayer1)
		{
			Current_Animation = shooting[SOUTH];
		}
		else
		{
			Current_Animation = shooting[NORTH];
		}

		if (info.closest->position == position)
		{
			if (fromPlayer1)
			{
				Current_Animation = shooting[SOUTH];
			}
			else
			{
				Current_Animation = shooting[NORTH];
			}
		}
		else if (info.closest->position.second <= position.second && info.closest->position.first >= position.first)
		{
			//noth
			Current_Animation = shooting[NORTH];
			if (info.closest->position.second == position.second)
			{
				//northwest
				Current_Animation = shooting[NORTHWEST];
			}
			//else if (info.closest->position.second > position.second)
			//{
			//	//north
			//	Current_Animation = shooting[NORTH];
			//}
			else if (info.closest->position.first == position.first)
			{
				//northeast
				Current_Animation = shooting[NORTHEAST];
			}
		}
		else if (info.closest->position.first >= position.first && info.closest->position.second >= position.second)
		{
			//south
			Current_Animation = shooting[SOUTH];
			if (info.closest->position.second == position.second)
			{
				//southwest
				Current_Animation = shooting[SOUTHWEST];
			}
			//else if (info.closest->position.second > position.second)
			//{
			//	//north
			//	Current_Animation = shooting[NORTH];
			//}
			else if (info.closest->position.first == position.first)
			{
				//southeast
				Current_Animation = shooting[SOUTHEAST];
			}
		}
		else if (info.closest->position.second > position.second && info.closest->position.first > position.first)
		{
			//east
			Current_Animation = shooting[EAST];
		}
		else if (info.closest->position.second < position.second && info.closest->position.first < position.first)
		{
			//west
			Current_Animation = shooting[WEST];

		}
		else
		{
			if (fromPlayer1)
			{
				Current_Animation = shooting[SOUTH];
			}
			else
			{
				Current_Animation = shooting[NORTH];
			}
		}

	}

}


void Hound::LoadAnimations(bool isPlayer1, string path)
{
	BROFILER_CATEGORY("Hound Load Animations", Profiler::Color::Blue);
	moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);


	idle = idle->LoadAnimation(path.data(), (isPlayer1) ? "red_idle" : "blue_idle");

	moving[NORTH] = moving[NORTH]->LoadAnimation(path.data(), (isPlayer1) ? "red_north" : "blue_north");
	moving[SOUTH] = moving[SOUTH]->LoadAnimation(path.data(), (isPlayer1) ? "red_south" : "blue_south");
	moving[EAST] = moving[EAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_east" : "blue_east");
	moving[WEST] = moving[WEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_west" : "blue_west");
	moving[NORTHEAST] = moving[NORTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_northeast" : "blue_northeast");
	moving[NORTHWEST] = moving[NORTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_northwest" : "blue_northwest");
	moving[SOUTHEAST] = moving[SOUTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_southeast" : "blue_southeast");
	moving[SOUTHWEST] = moving[SOUTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_southwest" : "blue_southwest");

	shooting[NORTH] = shooting[NORTH]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_N" : "blue_shoot_N");
	shooting[SOUTH] = shooting[SOUTH]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_S" : "blue_shoot_S");
	shooting[EAST] = shooting[EAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_E" : "blue_shoot_E");
	shooting[WEST] = shooting[WEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_W" : "blue_shoot_W");
	shooting[NORTHEAST] = shooting[NORTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_NE" : "blue_shoot_NE");
	shooting[NORTHWEST] = shooting[NORTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_NW" : "blue_shoot_NW");
	shooting[SOUTHEAST] = shooting[SOUTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_SE" : "blue_shoot_SE");
	shooting[SOUTHWEST] = shooting[SOUTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_SW" : "blue_shoot_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		moving[i]->speed = 10;
		shooting[i]->speed = 6;
	}

	idle->speed = 0;
	if (fromPlayer1)
	{
		idle->SetCurrentFrame(2);
	}
	else
	{
		idle->SetCurrentFrame(6);
	}
	Current_Animation = moving[NORTH];
}

Entity* Hound::FindEntityInAttackRange(pair <int, int> pos, bool fromplayer1, int attackrange, entityType desiredtype )
{
	Player* enemy = (!fromplayer1) ? App->player1 : App->player2;

	Entity* found = *enemy->entities.begin();
	int distance = 0;
	pair<int, int> map_pos;
	int min_dist;

	if (found != nullptr)
	{
		//take the first of de desired group
		
			map_pos = App->map->WorldToMap(found->position.first, found->position.second);
			Is_inRange(pos, distance, map_pos, attackrange);
			min_dist = distance;
		
		

		for (list<Entity*>::iterator tmp = enemy->entities.begin(); tmp != enemy->entities.end(); tmp++) // traverse entity list (unordered)
		{
			if ((*tmp)->type >= desiredtype)
			{
				map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);

				if (Is_inRange(pos, distance, map_pos, attackrange))
				{

					if (min_dist >= distance)
					{
						found = (*tmp);
						min_dist = distance;
					}
				}

			}

		}
		if (min_dist <= attackrange)
		{
			return found;
		}

	}

	return nullptr;

}

Entity* Hound::FindNearestEntity(pair <int, int> pos, bool fromplayer1, entityType desiredtype)
{
	Player* enemy = (!fromplayer1) ? App->player1 : App->player2;

	Entity* found = *enemy->entities.begin();
	int distance = 0;
	pair<int, int> map_pos;
	int min_dist;

	if (found != nullptr)
	{
		//take the first of de desired group
		map_pos = App->map->WorldToMap(found->position.first, found->position.second);
		Is_inRange(pos, distance, map_pos, 0);
		min_dist = distance;

		for (list<Entity*>::iterator tmp = enemy->entities.begin(); tmp != enemy->entities.end(); tmp++) // traverse entity list (unordered)
		{
			//if ((*tmp)->type >= desiredtype)
			{
				map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);


				Is_inRange(pos, distance, map_pos, 0);
				

					if (min_dist > distance)
					{
						found = (*tmp);
						min_dist = distance;
					}
				
			}

		}
		
	}

	return found;

}