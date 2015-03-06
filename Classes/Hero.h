#ifndef HERO_H_
#define HERO_H_

#include "GameObject.h"

class GameWorld;
class Platform;

class Hero : public GameObject
{
public:
	Hero();
	~Hero();

	// returns and autorelease GameObject
	static Hero* create(GameWorld* game_world);

	void Update(bool must_go_left, bool must_go_right, bool must_jump, bool must_swing);
	void UpdateSpeed(bool must_go_left, bool must_go_right, bool must_jump);
	virtual void CollisionResponse(int tile_col, int tile_row, ECollisionType collision_type);
	void SetPlatformBelow(Platform* platform);

	void SetState(EHeroState state);
	inline EHeroState GetState() { return state_; }

	void SetIsOnGround(bool is_on_ground);
	inline bool IsOnGround() { return is_on_ground_; }
	inline float GetCurrentGroundHeight() { return current_ground_height_; }

	void StartIdle();
	void StartWalking();
	void StartJumping();
	void StartSwinging();
	void StartDying();
	void FinishDying();
	void StartWinning();
	void FinishWinning();

private:
	EHeroState state_;
	bool is_on_ground_;
	float current_ground_height_;
	Platform* platform_below_;
};

#endif // HERO_H_