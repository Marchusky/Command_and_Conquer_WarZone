#include "DefenseAoe.h"
#include "Brofiler\Brofiler.h"
#include "Audio.h"
#include "Player.h"
#include "Render.h"
#include "Log.h"
#include <map>


DefenseAoe::DefenseAoe()
{
	// NOW IS A COPY OF DEFENSE TARGET WITH DIFERENT BEHAVIOUR (UPDATE)
	// NEED TO CHANGE THE NAME INSIDE GetName() TO "defense_aoe" ONCE ANIMATIONS PARSED
}

DefenseAoe::~DefenseAoe()
{
}

DefenseAoe::DefenseAoe(bool isPlayer1, pair<int, int> pos, Collider collider) : Building(Entity::entityType::DEFENSE_AOE, isPlayer1, pos, collider)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
	colider = collider;
}

bool DefenseAoe::Start()
{


	return true;
}

bool DefenseAoe::PreUpdate()
{
	BROFILER_CATEGORY("DefenseAoe PreUpdate", Profiler::Color::SandyBrown);


	return true;
}

bool DefenseAoe::Update(float dt)
{
	BROFILER_CATEGORY("DefenseAoe Update", Profiler::Color::SandyBrown);

	//Checks where to look for enemies
	Player* tmpMod = (fromPlayer1) ? App->player2 : App->player1;
	list<Troop*>::iterator tmp = tmpMod->troops.begin();

	// Finds the closest one
	Troop* closest = *tmpMod->troops.begin();
	if (tmp != tmpMod->troops.end()) {
		int min_distance;
		int d = 0;

		// Gets first distance
		Is_inRange(closest->position, min_distance);

		while (tmp != tmpMod->troops.end())
		{
			if ((*tmp)->alive && Is_inRange((*tmp)->position, d) && min_distance >= d) {
				closest = *tmp;
				min_distance = d;
			}
			tmp++;
		}

		// Shoots the closest one if in range
		if (timer.ReadSec() >= rate_of_fire && Is_inRange(closest->position, d))
		{
			closest->TakeDamage(damage_lv[level]);
			timer.Start();
			App->audio->PlayFx(TESLA_ATTACK);
			//LOG("Distance: %d", d);
		}
	}

	if (fromPlayer1)  // --- Player 1 --------------------------------
	{

		if (level == 1)
		{
			SDL_Rect upgrade;
			upgrade.x = 0;
			upgrade.y = 9;
			upgrade.w = 8;
			upgrade.h = 5;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (level == 2)
		{
			SDL_Rect upgrade;
			upgrade.x = 12;
			upgrade.y = 6;
			upgrade.w = 8;
			upgrade.h = 9;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (level == 3)
		{
			SDL_Rect upgrade;
			upgrade.x = 22;
			upgrade.y = 2;
			upgrade.w = 8;
			upgrade.h = 13;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (upgrade == true && level <= 3) //upgrade
		{
			App->player1->gold -= upgrade_cost[level]; //pay costs
			level++;
			damage = damage_lv[level];
			upgrade = false;
			//play fx (upgrade);
		}

		if (health <= 0) //destroyed
		{
			App->player1->UpdateWalkabilityMap(true, colider);
			App->player1->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
		}
	}
	else if (!fromPlayer1) // --- Player 2 ---------------------------
	{

		if (level == 1)
		{
			SDL_Rect upgrade;
			upgrade.x = 0;
			upgrade.y = 9;
			upgrade.w = 8;
			upgrade.h = 5;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (level == 2)
		{
			SDL_Rect upgrade;
			upgrade.x = 12;
			upgrade.y = 6;
			upgrade.w = 8;
			upgrade.h = 9;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (level == 3)
		{
			SDL_Rect upgrade;
			upgrade.x = 22;
			upgrade.y = 2;
			upgrade.w = 8;
			upgrade.h = 13;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (upgrade == true && level <= 3) //upgrade
		{
			App->player2->gold -= upgrade_cost[level]; //pay costs
			level++;
			damage = damage_lv[level];
			upgrade = false;
			//play fx (upgrade);
		}

		if (health <= 0) //destroyed
		{
			App->player2->UpdateWalkabilityMap(true, colider);
			App->player2->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
		}
	}

	if (Current_Animation->Finished() == true)
		Current_Animation = level1;

	Building::Update(dt);
	
	return true;
}

void DefenseAoe::CleanUp()
{
	// Its needed or the super class is always called?
	Entity::CleanUp();
}



bool DefenseAoe::Is_inRange(pair<int, int> pos, int &distance) {

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second, 2)));

	return distance <= range;
}

void DefenseAoe::LoadAnimations(bool isPlayer1, string path)
{
	building = building->LoadAnimation(path.data(), (isPlayer1) ? "soviet_aoe_build" : "allied_aoe_build");
	level1 = level1->LoadAnimation(path.data(), (isPlayer1) ? "soviet_aoe_idle" : "allied_aoe_idle");
	level1->speed = 1;
	building->speed = 9;
	building->loop = false;
	level1->loop = true;
	Current_Animation = building;
}

