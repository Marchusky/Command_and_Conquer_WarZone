#ifndef __Townhall_H__
#define __Townhall_H__

#include "Building.h"

class Townhall : public Building
{
public:
	Townhall();
	Townhall(bool isPlayer1, pair<int, int> position, Collider collider);
	~Townhall();

	bool Update(float dt);
	void CleanUp();
	void LoadAnimations(bool isPlayer1, string path);
};

#endif