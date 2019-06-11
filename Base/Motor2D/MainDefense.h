#ifndef __MainDefense_H__
#define __MainDefense_H__

#include "Building.h"

class MainDefense : public Building
{
public:
	MainDefense();
	MainDefense(bool isPlayer1, pair<int, int> position, Collider collider);
	~MainDefense();

	bool Update(float dt);
	void CleanUp();
	void LoadAnimations(bool isPlayer1, string path);
	bool Is_inRange(pair<int, int> pos, int & distance);
	void ChangeAnimation(Entity * closest);
	Animation* idle;
	/*vector<Animation*> moving;*/
	vector<Animation*> shooting;
	bool isShooting = false;
};

#endif