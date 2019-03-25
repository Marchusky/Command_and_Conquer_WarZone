#include "App.h"
#include "Textures.h"
#include "Input.h"
#include "Render.h"
#include "CursorModule.h"


CursorModule::CursorModule()
{
}

CursorModule::~CursorModule()
{
}

bool CursorModule::Awake()
{


	return true;
}

bool CursorModule::Start()
{
	//Cursor player 1 ------------
	cP1.position.first = 300;
	cP1.position.second = 300;

	cP1.area.x = cP1.area.y = 0;
	cP1.area.w = cP1.area.h = 25;

	//Cursor player 2 ------------
	cP2.position.first = 300;
	cP2.position.second = 300;

	cP2.area.x = 27;
	cP2.area.y = 0;
	cP2.area.w = cP2.area.h = 25;

	if (graphics == nullptr)
	{
		graphics = App->tex->Load("textures/Cursors.png");
	}

	return true;
}

bool CursorModule::Update(float dt)
{
	if (App->input->P1.Controller[JOY_UP] == KEY_REPEAT || App->input->P1.Controller[UP] == KEY_REPEAT)
		cP1.position.second -= 6;

	if (App->input->P1.Controller[JOY_DOWN] == KEY_REPEAT || App->input->P1.Controller[DOWN] == KEY_REPEAT)
		cP1.position.second += 6;

	if (App->input->P1.Controller[JOY_RIGHT] == KEY_REPEAT || App->input->P1.Controller[RIGHT] == KEY_REPEAT)
		cP1.position.first += 6;

	if (App->input->P1.Controller[JOY_LEFT] == KEY_REPEAT || App->input->P1.Controller[LEFT] == KEY_REPEAT)
		cP1.position.first -= 6;

	

	if (App->input->P2.Controller[JOY_UP] == KEY_REPEAT || App->input->P2.Controller[UP] == KEY_REPEAT)
		cP2.position.second -= 6;

	if (App->input->P2.Controller[JOY_DOWN] == KEY_REPEAT || App->input->P2.Controller[DOWN] == KEY_REPEAT)
		cP2.position.second += 6;

	if (App->input->P2.Controller[JOY_RIGHT] == KEY_REPEAT || App->input->P2.Controller[RIGHT] == KEY_REPEAT)
		cP2.position.first += 6;

	if (App->input->P2.Controller[JOY_LEFT] == KEY_REPEAT || App->input->P2.Controller[LEFT] == KEY_REPEAT)
		cP2.position.first -= 6;

	return true;
}

bool CursorModule::PostUpdate()
{
	App->render->Blit(graphics, cP1.position.first, cP1.position.second, &cP1.area);
	App->render->Blit(graphics, cP2.position.first, cP2.position.second, &cP2.area);

	return true;
}

bool CursorModule::CleanUp()
{
	App->tex->UnLoad(graphics);
	graphics = nullptr;

	return true;
}

void CursorModule::GetCursor1_Position(int& x, int& y)
{
	x = cP1.position.first;
	y = cP1.position.second;
}

void CursorModule::GetCursor2_Position(int& x, int& y)
{
	x = cP2.position.first;
	y = cP2.position.second;
}
