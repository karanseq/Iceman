#ifndef ENEMY_H_
#define ENEMY_H_

#include "GameObject.h"

class GameWorld;

class Enemy : public GameObject
{
public:
	Enemy();
	~Enemy();

	// returns and autorelease GameObject
	static Enemy* create(GameWorld* game_world);

	virtual void SetSpeed(CCPoint speed);

	void Update();
	void CheckForHoles();
	virtual void CollisionResponse(int tile_col, int tile_row, ECollisionType collision_type);

	void SetState(EEnemyState state);
	inline EEnemyState GetState() { return state_; }
	inline bool GetMustBeRemoved() { return must_be_removed_; }

	void StartWalking();
	void StartDying();
	void FinishDying();
	void StartBuilding();
	void FinishBuilding();

private:
	EEnemyState state_;
	int tile_col_to_build_;
	int tile_row_to_build_;
	bool must_be_removed_;
};

#endif // ENEMY_H_