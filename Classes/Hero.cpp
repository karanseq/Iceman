#include "Hero.h"
#include "GameWorld.h"
#include "Platform.h"

Hero* Hero::create(GameWorld* game_world)
{
	Hero* hero = new Hero();
	if(hero && hero->initWithFile("idle0001.png"))
	{
		hero->autorelease();
		hero->game_world_ = game_world;
		hero->SetState(E_HERO_STATE_IDLE);
		return hero;
	}
	CC_SAFE_DELETE(hero);
	return NULL;
}

Hero::Hero()
{
	type_ = E_GAME_OBJECT_HERO;
	state_ = E_HERO_STATE_NONE;
	is_on_ground_ = false;
	current_ground_height_ = 0.0f;
	platform_below_ = NULL;
}

Hero::~Hero()
{}

void Hero::Update(bool must_go_left, bool must_go_right, bool must_jump, bool must_swing)
{
	// let the hero die in peace
	if(state_ == E_HERO_STATE_DYING)
		return;

	// update speed based on user input
	UpdateSpeed(must_go_left, must_go_right, must_jump);

	// this enables the hero to leave from the left edge and reappear from the right edge
	if(aabb_.origin.x < TILE_SIZE)
	{
		aabb_.origin.x = SCREEN_SIZE.width - TILE_SIZE - aabb_.size.width;
	}
	if(aabb_.origin.x + aabb_.size.width > SCREEN_SIZE.width - TILE_SIZE)
	{
		aabb_.origin.x = TILE_SIZE;
	}

	// update the AABB
	GameObject::Update();
	// check for collisions
	game_world_->CheckCollisions(this);
	game_world_->CheckHeroEnemyCollisions();
	game_world_->CheckHeroPlatformCollisions();

	setPosition(ccp(aabb_.origin.x + aabb_.size.width * 0.5f, aabb_.origin.y + m_obContentSize.height * 0.5f));

	// check if the hero should swing
	if(must_swing)
	{
		SetState(E_HERO_STATE_SWINGING);
	}
	else if(state_ == E_HERO_STATE_SWINGING)
	{
		SetState(E_HERO_STATE_IDLE);
	}

	// check if the hero could be falling
	if(fabs(speed_.y) > 0.1f)
	{
		SetIsOnGround(false);
	}
}

void Hero::UpdateSpeed(bool must_go_left, bool must_go_right, bool must_jump)
{
	// add gravity & clamp vertical velocity
	speed_.y += GRAVITY;
	speed_.y = (speed_.y < MAX_VELOCITY_Y) ? MAX_VELOCITY_Y : speed_.y;

	// is the hero above a platform
	if(platform_below_)
	{
		// stop falling due to gravity
		speed_.y = 0;
		// move the hero along with the platform he's standing on
		aabb_.origin.x += platform_below_->GetSpeed().x;
		aabb_.origin.y = platform_below_->GetAABB().getMaxY();
	}

	// set speed accordingly if the hero must jump
	if(must_jump && is_on_ground_)
	{
		speed_.y = TILE_SIZE * 0.75f;
	}

	// increase/decrease the horizontal speed based on the button pressed
	if(must_go_left)
	{
		speed_.x -= HERO_MOVEMENT_FORCE;
		speed_.x = (speed_.x < -MAX_VELOCITY_X) ? -MAX_VELOCITY_X : speed_.x;
		setFlipX(true);
	}
	if(must_go_right)
	{
		speed_.x += HERO_MOVEMENT_FORCE;
		speed_.x = (speed_.x > MAX_VELOCITY_X) ? MAX_VELOCITY_X : speed_.x;
		setFlipX(false);
	}

	// gradually come to a halt if no button is pressed
	if(!must_go_left && !must_go_right)
	{
		speed_.x -= speed_.x / 5;
	}

	// change from idle to walking & vice versa based on horizontal velocity
	if(fabs(speed_.x) > 0.5f)
	{
		SetState(E_HERO_STATE_WALKING);
	}
	else if(state_ == E_HERO_STATE_WALKING)
	{
		SetState(E_HERO_STATE_IDLE);
	}
}

void Hero::CollisionResponse(int tile_col, int tile_row, ECollisionType collision_type)
{
	switch(collision_type)
	{
	case E_COLLISION_TOP:
		// stop moving
		speed_.y = 0;
		// collision occured above AABB...reposition the AABB one pixel below the collided tile
		aabb_.origin.y = GET_Y_FOR_ROW(tile_row + 1, game_world_->GetRows()) - aabb_.size.height - 1;
		// tell the game world to remove this brick
		game_world_->RemoveBrick(tile_col, tile_row);
		break;
	case E_COLLISION_BOTTOM:
		// stop moving
		speed_.y = 0;
		// collision occured below AABB...reposition the AABB one pixel above the collided tile
		aabb_.origin.y = GET_Y_FOR_ROW(tile_row, game_world_->GetRows()) + 1;
		// hero has landed on a brick
		SetIsOnGround(true);
		break;
	case E_COLLISION_LEFT:
		// stop moving
		speed_.x = 0;
		// collision occured to the left AABB...reposition the AABB one pixel after the collided tile
		aabb_.origin.x = GET_X_FOR_COL(tile_col + 1) + 1;
		break;
	case E_COLLISION_RIGHT:
		// stop moving
		speed_.x = 0;
		// collision occured to the right AABB...reposition the AABB one pixel before the collided tile
		aabb_.origin.x = GET_X_FOR_COL(tile_col) - aabb_.size.width - 1;
		break;
	}
}

void Hero::SetPlatformBelow(Platform* platform)
{
	// save this platform
	platform_below_ = platform;

	// even platforms count as ground
	if(platform_below_)
	{
		SetIsOnGround(true);
	}
}

void Hero::SetState(EHeroState state)
{
	// only accept a change in state
	if(state_ == state)
		return;

	state_ = state;

	// call respective state based action
	switch(state_)
	{
	case E_HERO_STATE_IDLE:
		StartIdle();
		break;
	case E_HERO_STATE_WALKING:
		StartWalking();
		break;
	case E_HERO_STATE_JUMPING:
		StartJumping();
		break;
	case E_HERO_STATE_SWINGING:
		StartSwinging();
		break;
	case E_HERO_STATE_DYING:
		StartDying();
		break;
	case E_HERO_STATE_WINNING:
		StartWinning();
		break;
	}
}

void Hero::SetIsOnGround(bool is_on_ground)
{
	/// only accept a change
	if(is_on_ground_ == is_on_ground)
		return;

	is_on_ground_ = is_on_ground;

	if(is_on_ground_)
	{
		// save the height the hero is at currently
		current_ground_height_ = aabb_.origin.y;
		SetState(E_HERO_STATE_IDLE);
	}
	else
	{
		// going up means jumping...for now
		if(speed_.y > 0)
		{
			SetState(E_HERO_STATE_JUMPING);
		}
	}
}

void Hero::StartIdle()
{
	stopAllActions();
	CCActionInterval* idle_animation = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("HeroIdle"));
	runAction(idle_animation);
}

void Hero::StartWalking()
{
	stopAllActions();
	CCActionInterval* walking_animation = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("HeroWalking"));
	runAction(walking_animation);
}

void Hero::StartJumping()
{
	stopAllActions();
	initWithFile("jumping.png");
	SOUND_ENGINE->playEffect("jump.wav");
}

void Hero::StartSwinging()
{
	stopAllActions();
	CCActionInterval* swinging_animation = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("HeroSwinging"));
	runAction(swinging_animation);
	SOUND_ENGINE->playEffect("swing.wav");
}

void Hero::StartDying()
{
	speed_.x = 0;
	speed_.y = 0;

	stopAllActions();
	CCActionInterval* dying_animation = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("HeroDying"));
	CCActionInstant* after_dying = CCCallFunc::create(this, callfunc_selector(Hero::FinishDying));
	runAction(CCSequence::createWithTwoActions(dying_animation, after_dying));

	CCActionInterval* falling_animation = CCEaseBackIn::create(CCMoveBy::create(0.75f, ccp(0, SCREEN_SIZE.height * -0.4f)));
	runAction(falling_animation);
	SOUND_ENGINE->playEffect("hero_death.wav");
}

void Hero::FinishDying()
{
	// if lives are over, so is the game
	if(GameGlobals::hero_lives_left_ <= 0)
	{
		game_world_->GameOver();
	}
	// otherwise recreate the level
	else
	{
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, GameWorld::scene()));
	}
}

void Hero::StartWinning()
{
	speed_.x = 0;
	speed_.y = 0;

	stopAllActions();
	CCActionInterval* winning_animation = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("HeroWinning"));
	CCActionInstant* after_winning = CCCallFunc::create(this, callfunc_selector(Hero::FinishWinning));
	runAction(CCSequence::createWithTwoActions(winning_animation, after_winning));
	SOUND_ENGINE->playEffect("level_complete.wav");
}

void Hero::FinishWinning()
{
	game_world_->LevelComplete();
}