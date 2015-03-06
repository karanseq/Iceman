#ifndef GAME_WORLD_H_
#define GAME_WORLD_H_

#include "GameGlobals.h"

class GameObject;
class Hero;
class Enemy;
class Platform;

class GameWorld : public CCLayer
{
public:
	GameWorld();
	virtual ~GameWorld();

    static CCScene* scene();    
    CREATE_FUNC(GameWorld);

    virtual bool init();

	void CreateGame();
	void CreateTiledMap();
	void CreateHero(CCPoint position);
	void CreateEnemy(CCPoint position, CCPoint speed);
	void CreatePlatform(CCPoint position, CCPoint speed);
	void CreateControls();
	void CreateHUD();

	// update functions
	virtual void update(float dt);
	void UpdatePlatforms();
	void UpdateEnemies();
	void UpdateHero();

	bool CheckCollisions(GameObject* game_object);
	bool CheckVerticalCollisions(GameObject* game_object);
	bool CheckHorizontalCollisions(GameObject* game_object);
	void CheckHeroEnemyCollisions();
	void CheckHeroPlatformCollisions();

	void AddBrick(int tile_col, int tile_row);
	void RemoveBrick(int tile_col, int tile_row);
	void RemoveEnemies();
	void ReduceHeroLives();

	virtual void ccTouchesBegan(CCSet* set, CCEvent* event);
	virtual void ccTouchesMoved(CCSet* set, CCEvent* event);
	virtual void ccTouchesEnded(CCSet* set, CCEvent* event);
	void HandleTouch(CCPoint touch_point, bool is_touching);

	void OnPauseClicked(CCObject* sender);
	void ResumeGame();
	void LevelComplete();
	void GameOver();

	inline CCTMXTiledMap* GetTiledMap() { return tiled_map_; }
	inline CCTMXLayer* GetBricksLayer() { return bricks_layer_; }
	inline int GetColumns() { return columns_; }
	inline int GetRows() { return rows_; }

private:
	CCTMXTiledMap* tiled_map_;
	CCTMXLayer* bricks_layer_;
	int columns_;
	int rows_;

	Hero* hero_;

	int num_enemies_;
	vector<Enemy*> enemies_;

	int num_platforms_;
	vector<Platform*> platforms_;
	
	float level_complete_height_;

	CCArray* life_sprites_;
	CCSprite* left_arrow_btn_;
	CCSprite* right_arrow_btn_;
	CCSprite* jump_btn_;
	CCSprite* swing_btn_;
	bool is_left_arrow_pressed_;
	bool is_right_arrow_pressed_;
	bool is_jump_pressed_;
	bool is_swing_pressed_;

public:
	// variables accessed by class Popup
	bool is_popup_active_;
};

#endif // GAME_WORLD_H_