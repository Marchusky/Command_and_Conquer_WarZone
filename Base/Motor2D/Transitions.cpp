#include "Log.h"
#include "App.h"
#include "Transitions.h"
#include "Render.h"
#include "Scene.h"
#include "Window.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Gui.h"
#include "Brofiler\Brofiler.h"

#include <math.h>
#include "SDL\include\SDL_render.h"

Transitions::Transitions()
{
	name.assign("scenechange");
}

Transitions::~Transitions()
{}

bool Transitions::Awake(pugi::xml_node&)
{
	LOG("SceneChange Awake");
	bool ret = true;

	screen = { 0, 0, App->win->width*App->win->scale, App->win->height*App->win->scale };
	return ret;
}

bool Transitions::Start()
{
	LOG("SceneChange Start");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}


bool Transitions::Update(float dt)
{
	BROFILER_CATEGORY("SceneChange Update", Profiler::Color::Red);

	if (current_step == fade_step::none)
	{
		return true;
	}

	uint now = SDL_GetTicks() - start_time;
	float normalized = 1.0f < ((float)now / (float)total_time) ? 1.0f : ((float)now / (float)total_time);


	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (map == true)
		{
			if (now >= total_time) //screen->black & map->loaded
			{
				App->scene->currentMap = nextMap;
				LOG("%i", App->scene->currentMap);

				App->map->SwitchMaps(App->scene->map_names[nextMap]);

				total_time += total_time;
				start_time = SDL_GetTicks();
				fading = false;
				current_step = fade_step::fade_from_black;
			}
		}
		else if (scene == true)
		{
			if (switchtimer.ReadSec() >= fadetime)
			{
				to_disable->Disable();
				App->gui->CleanUp();
				App->map->CleanUp();
				App->entitymanager->DeleteEntities();
				if (to_disable == App->scene)
				{
					App->entitymanager->Disable();
				}

				App->gui->Start();
				to_enable->Enable();
				if (to_enable == App->scene)
				{
					App->entitymanager->Enable();
				}

				switchtimer.Start();

				if (App->scenechange->ContinueGame)
				{
					App->LoadGame();
				}
				current_step = fade_step::fade_from_black;
			}
		}
	}break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (map == true)
		{
			if (now >= total_time)
			{
				current_step = fade_step::none;
				map = false;
			}
		}
		else if (scene == true)
		{
			if (switchtimer.ReadSec() >= fadetime)
			{
				current_step = fade_step::none;
				scene = false;
			}
		}
	}break;

	}

	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

bool Transitions::ChangeMap(int newMap, float time)
{
	bool ret = false;

	nextMap = newMap;

	map = true;
	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time*0.5f*1000.0f);
		fading = true;
		ret = true;
	}

	return ret;
}

bool Transitions::IsChanging() const
{
	return current_step != fade_step::none;
}

bool Transitions::SwitchScene(Module* SceneIn, Module* SceneOut)
{
	bool ret = false;

	scene = true;
	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		switchtimer.Start();
		to_enable = SceneIn;
		to_disable = SceneOut;

		ret = true;
	}
	return true;
}

bool Transitions::IsSwitching() const
{
	return (current_step != fade_step::none);
}
