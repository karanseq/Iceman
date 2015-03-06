#include "Enemy.h"
#include "GameWorld.h"

Enemy* Enemy::create(GameWorld* game_world)
{
	Enemy* enemy = new Enemy();
	if(enemy && enemy->initWithFile("enemy walking0001.png"))
	{
		enemy->autorelease();
		enemy->game_world_ = game_world;
		enemy->SetState(E_ENEMY_STATE_WALKING);
		return enemy;
	}
	CC_SAFE_DELETE(enemy);
	return NULL;
}

Enemy::Enemy()
{
	type_ = E_GAME_OBJECT_ENEMY;
	state_ = E_ENEMY_STATE_NONE;
	tile_col_to_build_ = -1;
	tile_row_to_build_ = -1;
	must_be_removed_ = false;
}

Enemy::~Enemy()
{}

void Enemy::SetSpeed(CCPoint speed)
{
	// update speed_
	GameObject::SetSpeed(speed);

	// flip the sprite based on direction of horizontal movement
	if(speed_.x > 0)
	{
		setFlipX(true);
	}
	else
	{
		setFlipX(false);
	}
}

void Enemy::Update()
{
	// reverse the direction of movement when this enemy reaches the left & right edge
	if(aabb_.origin.x + speed_.x <= TILE_SIZE || aabb_.origin.x + aabb_.size.width >= SCREEN_SIZE.width - TILE_SIZE)
	{
		SetSpeed(ccp(speed_.x * -1, speed_.y));
	}

	// check for holes in the ground when not already building
	if(state_ != E_ENEMY_STATE_BUILDING)
	{
		// update the AABB
		GameObject::Update();
		// check for collisions
		game_world_->CheckHorizontalCollisions(this);

		// check for any holes in the ground
		CheckForHoles();
	}

	// update position
	setPosition(ccp(aabb_.origin.x + aabb_.size.width * 0.5f, aabb_.origin.y + aabb_.size.height * 0.5f));
}

void Enemy::CheckForHoles()
{
	// get the next tile column & row in the direction of movement
	int tile_col = GET_COL_FOR_X(aabb_.origin.x) + (speed_.x > 0 ? 1 : 0);
	int tile_row = GET_ROW_FOR_Y(aabb_.origin.y, game_world_->GetRows()) + 1;

	// get the tile's GID
	int tile_GID = game_world_->GetBricksLayer()->tileGIDAt(ccp(tile_col, tile_row));
	// check if there is an empty tile
	if(tile_GID == 0)
	{
		// save the column and row of this empty tile
		tile_col_to_build_ = tile_col;
		tile_row_to_build_ = tile_row;
		// start building
		SetState(E_ENEMY_STATE_BUILDING);
	}
}

void Enemy::CollisionResponse(int tile_col, int tile_row, ECollisionType collision_type)
{
	switch(collision_type)
	{
	case E_COLLISION_TOP:
		// stop moving
		speed_.y = 0;
		// collision occured above AABB...reposition the AABB one pixel below the collided tile
		aabb_.origin.y = GET_Y_FOR_ROW(tile_row + 1, game_world_->GetRows()) - aabb_.size.height - 1;
		break;
	case E_COLLISION_BOTTOM:
		// stop moving
		speed_.y = 0;
		// collision occured below AABB...reposition the AABB one pixel above the collided tile
		aabb_.origin.y = GET_Y_FOR_ROW(tile_row, game_world_->GetRows()) + 1;
		break;
	case E_COLLISION_LEFT:
		// start moving in the opposite direction
		speed_.x *= -1;
		// collision occured to the left AABB...reposition the AABB one pixel after the collided tile
		aabb_.origin.x = GET_X_FOR_COL(tile_col + 1) + 1;
		break;
	case E_COLLISION_RIGHT:
		// start moving in the opposite direction
		speed_.x *= -1;
		// collision occured to the right AABB...reposition the AABB one pixel before the collided tile
		aabb_.origin.x = GET_X_FOR_COL(tile_col) - aabb_.size.width - 1;
		break;
	}
}

void Enemy::SetState(EEnemyState state)
{
	// only accept a change in state
	if(state_ == state)
		return;

	state_ = state;

	// call respective state based action
	switch(state_)
	{
	case E_ENEMY_STATE_WALKING:
		StartWalking();
		break;
	case E_ENEMY_STATE_DYING:
		StartDying();
		break;
	case E_ENEMY_STATE_BUILDING:
		StartBuilding();
		break;
	}
}

void Enemy::StartWalking()
{
	stopAllActions();
	CCActionInterval* walking_animation = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("EnemyWalking"));
	runAction(walking_animation);
}

void Enemy::StartDying()
{
	speed_.x *= -0.5f;

	stopAllActions();
	CCActionInterval* dying_animation = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("EnemyDying"));
	CCActionInterval* blinking = CCBlink::create(0.75f, 5);
	CCActionInstant* after_dying = CCCallFunc::create(this, callfunc_selector(Enemy::FinishDying));
	runAction(CCSequence::create(dying_animation, blinking, after_dying, NULL));

	SOUND_ENGINE->playEffect("enemy_death.wav");
}

void Enemy::FinishDying()
{
	must_be_removed_ =  true;
}

void Enemy::StartBuilding()
{
	stopAllActions();
	CCActionInterval* building_animation = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("EnemyBuilding"));
	CCActionInstant* after_building = CCCallFunc::create(this, callfunc_selector(Enemy::FinishBuilding));
	runAction(CCSequence::createWithTwoActions(building_animation, after_building));
}

void Enemy::FinishBuilding()
{
	stopAllActions();

	game_world_->AddBrick(tile_col_to_build_, tile_row_to_build_);
	tile_col_to_build_ = -1;
	tile_row_to_build_ = -1;

	SetState(E_ENEMY_STATE_WALKING);
}