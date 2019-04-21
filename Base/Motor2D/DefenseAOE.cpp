#include "DefenseAoe.h"
#include "Brofiler\Brofiler.h"
#include "Player.h"
#include "Render.h"
#include "Log.h"
#include <map>


DefenseAoe::DefenseAoe()
{
	// NOW IS A COPY OF DEFENSE TARGET WITH DIFERENT BEHAVIOUR (UPDATE)
}

DefenseAoe::~DefenseAoe()
{
}

DefenseAoe::DefenseAoe(bool isPlayer1, pair<int, int> pos, Collider collider) : Building(Entity::entityType::DEFENSE_AOE, isPlayer1, pos, collider)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
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
	building = building->LoadAnimation(path.data(), (isPlayer1) ? "red" : "blue");
	level1 = level1->LoadAnimation(path.data(), (isPlayer1) ? "red" : "blue");
	level1->speed = 3;
	building->speed = 3;
	building->loop = false;
	level1->loop = false;
	Current_Animation = building;
}

