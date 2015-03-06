#include "Platform.h"
#include "GameWorld.h"

Platform* Platform::create(GameWorld* game_world, const char* frame_name)
{
	Platform* platform = new Platform();
	if(platform && platform->initWithFile(frame_name))//initWithSpriteFrameName(frame_name))
	{
		platform->autorelease();
		platform->game_world_ = game_world;
		return platform;
	}
	CC_SAFE_DELETE(platform);
	return NULL;
}

Platform::Platform()
{
	type_ = E_GAME_OBJECT_PLATFORM;
}

Platform::~Platform()
{}

void Platform::Update()
{
	// update the AABB
	GameObject::Update();

	// reverse the direction of movement when this platform reaches the left & right edge
	if(aabb_.origin.x < TILE_SIZE || aabb_.origin.x + aabb_.size.width > SCREEN_SIZE.width - TILE_SIZE)
	{
		speed_.x *= -1;
	}

	setPosition(ccp(aabb_.origin.x + aabb_.size.width * 0.5f, aabb_.origin.y + aabb_.size.height * 0.5f));
}