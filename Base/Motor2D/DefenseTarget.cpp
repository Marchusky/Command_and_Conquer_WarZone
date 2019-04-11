#include "DefenseTarget.h"
#include "Brofiler\Brofiler.h"
#include "Player.h"
#include "Render.h"
#include "Log.h"

DefenseTarget::DefenseTarget()
{
}

DefenseTarget::~DefenseTarget()
{
}

DefenseTarget::DefenseTarget(bool isPlayer1, pair<int, int> pos) : Entity(entityType::DEFENSE_TARGET, isPlayer1, pos)
{
	collider = { 0,0,64,32 };
	tex = App->tex->Load("maps/meta.png");

}

bool DefenseTarget::Start()
{
	

	return true;
}

bool DefenseTarget::PreUpdate()
{
	BROFILER_CATEGORY("DefenseTarget PreUpdate", Profiler::Color::SandyBrown);


	return true;
}

bool DefenseTarget::Update(float dt)
{
	BROFILER_CATEGORY("DefenseTarget Update", Profiler::Color::SandyBrown);

	// Moves building to mouse position 
	int x = 0;
	int y = 0;
	if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE)) {

		App->input->GetMousePosition(x, y);
		position.first = x;
		position.second = y;
	}

	App->render->Blit(tex, position.first, position.second, &collider);

	// Checks where to look for enemies
	Player* tmpMod = (fromPlayer1) ? App->player2 : App->player1;
	list<Entity*>::iterator tmp = tmpMod->troops.begin();
	
	// Finds the closest one
	Entity* closest = *tmpMod->troops.begin();
	int min_distance;
	int d = 0;
	Is_inRange(closest->position, min_distance);

	while (tmp != tmpMod->troops.end())
	{
		if (Is_inRange((*tmp)->position, d) && min_distance >= d) {
			closest = *tmp;
			min_distance = d;
		}
		tmp++;
	}

	// Shoots the closest one if in range
	if (timer.ReadSec() >= rate_of_fire && Is_inRange(closest->position,d))
	{
		closest->TakeDamage(damage_lv[level]);
		timer.Start();
		//LOG("Distance: %d", d);
	}
	return true;
}

void DefenseTarget::CleanUp()
{
	// Its needed or the super class is always called?
	this->Entity::CleanUp();
}



bool DefenseTarget::Is_inRange(pair<int, int> pos, int &distance) {

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second, 2)));
	
	return distance <= range;
}

