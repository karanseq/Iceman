#ifndef GAME_GLOBALS_H_
#define GAME_GLOBALS_H_

// Global includes go here
#include "cocos2d.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace std;
// Global includes end here

// Helper macros
#define SCREEN_SIZE GameGlobals::screen_size_
#define SOUND_ENGINE CocosDenshion::SimpleAudioEngine::sharedEngine()

#define TILE_SIZE 32
#define GRAVITY -1
#define HERO_MOVEMENT_FORCE 0.5f
#define HERO_AABB_WIDTH 64
#define HERO_AABB_HEIGHT 70
#define ENEMY_MOVEMENT_SPEED 1
#define ENEMY_AABB_WIDTH 96
#define ENEMY_AABB_HEIGHT 84
#define MAX_VELOCITY_Y -32
#define MAX_VELOCITY_X 12

#define GET_X_FOR_COL(col) ( (col) * TILE_SIZE )
#define GET_Y_FOR_ROW(row, h) ( ( (h) - (row) ) * TILE_SIZE )
#define GET_COL_FOR_X(x) ( floor( (x) / TILE_SIZE ) )
#define GET_ROW_FOR_Y(y, h) ( (h) - ceil( (y) / TILE_SIZE ) )

//#define ICEMAN_DEBUG_MODE

// enum used for proper z-ordering
enum EZorder
{
	E_LAYER_BACKGROUND = 0,
	E_LAYER_ENEMIES = 2,
	E_LAYER_PLATFORMS = 3,
	E_LAYER_HERO = 4,
	E_LAYER_HUD = 6,
	E_LAYER_POPUP = 8,
};

enum EGameObjectType
{
	E_GAME_OBJECT_NONE = 0,
	E_GAME_OBJECT_HERO,
	E_GAME_OBJECT_ENEMY,
	E_GAME_OBJECT_PLATFORM,
};

enum ECollisionType
{
	E_COLLISION_NONE = 0,
	E_COLLISION_TOP,		// collision occurs above AABB
	E_COLLISION_BOTTOM,		// collision occurs below AABB
	E_COLLISION_LEFT,		// collision occurs to left of AABB
	E_COLLISION_RIGHT,		// collision occurs to right of AABB
};

enum EHeroState
{
	E_HERO_STATE_NONE = 0,
	E_HERO_STATE_IDLE,
	E_HERO_STATE_WALKING,
	E_HERO_STATE_JUMPING,
	E_HERO_STATE_SWINGING,
	E_HERO_STATE_DYING,
	E_HERO_STATE_WINNING,
};

enum EEnemyState
{
	E_ENEMY_STATE_NONE = 0,
	E_ENEMY_STATE_WALKING,
	E_ENEMY_STATE_DYING,
	E_ENEMY_STATE_BUILDING,
};

// Helper class containing only static members
class GameGlobals
{
private:
	GameGlobals();
	~GameGlobals();

public:
	// initialise common global data here
	// called when application finishes launching
	static void Init();
	// load initial/all game data here
	static void LoadData();
	static void LoadAnimations();

	static CCSize screen_size_;
	static int level_number_;
	static int hero_lives_left_;
};

#endif // GAME_GLOBALS_H_