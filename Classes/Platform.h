#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "GameObject.h"

class GameWorld;

class Platform : public GameObject
{
public:
	Platform();
	~Platform();

	// returns and autorelease GameObject
	static Platform* create(GameWorld* game_world, const char* frame_name);

	void Update();

private:
};

#endif // PLATFORM_H_