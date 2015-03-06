#include "GameWorld.h"
#include "Popups.h"
#include "Hero.h"
#include "Enemy.h"
#include "Platform.h"

GameWorld::GameWorld()
{
	is_popup_active_ = false;
	tiled_map_ = NULL;
	bricks_layer_ = NULL;
	columns_ = 0;
	rows_ = 0;
	hero_ = NULL;
	num_enemies_ = 0;
	enemies_.clear();
	num_platforms_ = 0;
	platforms_.clear();
	level_complete_height_ = 0.0f;
	life_sprites_ = NULL;
	left_arrow_btn_ = NULL;
	right_arrow_btn_ = NULL;
	jump_btn_ = NULL;
	swing_btn_ = NULL;
	is_left_arrow_pressed_ = false;
	is_right_arrow_pressed_ = false;
	is_jump_pressed_ = false;
	is_swing_pressed_ = false;
}

GameWorld::~GameWorld()
{
	CC_SAFE_RELEASE_NULL(life_sprites_);
}

CCScene* GameWorld::scene()
{
    CCScene *scene = CCScene::create();
    GameWorld *layer = GameWorld::create();
    scene->addChild(layer);
    return scene;
}

bool GameWorld::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }

    CreateGame();
    return true;
}

void GameWorld::CreateGame()
{
	CCLayerColor* bg = CCLayerColor::create(ccc4(25, 49, 69, 255));
	addChild(bg);

	CreateTiledMap();
	CreateControls();
	CreateHUD();
	
	setTouchEnabled(true);
#ifdef ICEMAN_DEBUG_MODE
	schedule(schedule_selector(GameWorld::update), 0.2f);
#else
	scheduleUpdate();
#endif
}

void GameWorld::CreateTiledMap()
{
	// generate level filename
	char buf[128] = {0};
	sprintf(buf, "level_%02d.tmx", GameGlobals::level_number_);
	// create & add the tiled map
	tiled_map_ = CCTMXTiledMap::create(buf);
	addChild(tiled_map_);

	// get the size of the tiled map
	columns_ = (int)tiled_map_->getMapSize().width;
	rows_ = (int)tiled_map_->getMapSize().height;

	// save a reference to the layer containing all the bricks
	bricks_layer_ = tiled_map_->layerNamed("Bricks");

	// parse the list of objects
	CCTMXObjectGroup* object_group = tiled_map_->objectGroupNamed("Objects");
	CCArray* objects = object_group->getObjects();
	int num_objects = objects->count();
	
	for(int i = 0; i < num_objects; ++i)
	{
		CCDictionary* object = (CCDictionary*)(objects->objectAtIndex(i));

		// create the Hero at this spawning point
		if(strcmp(object->valueForKey("name")->getCString(), "HeroSpawnPoint") == 0)
		{
			CreateHero(ccp(object->valueForKey("x")->floatValue(), object->valueForKey("y")->floatValue()));
		}
		// create an Enemy at this spawning point
		else if(strcmp(object->valueForKey("name")->getCString(), "EnemySpawnPoint") == 0)
		{
			CCPoint position = ccp(object->valueForKey("x")->floatValue(), object->valueForKey("y")->floatValue());
			CCPoint speed = ccp(object->valueForKey("speed_x")->floatValue(), object->valueForKey("speed_y")->floatValue());
			CreateEnemy(position, speed);
		}
		// create a Platform at this spawning point
		else if(strcmp(object->valueForKey("name")->getCString(), "PlatformSpawnPoint") == 0)
		{
			CCPoint position = ccp(object->valueForKey("x")->floatValue(), object->valueForKey("y")->floatValue());
			CCPoint speed = ccp(object->valueForKey("speed_x")->floatValue(), object->valueForKey("speed_y")->floatValue());
			CreatePlatform(position, speed);
		}
		// save the point where the level should complete
		else if(strcmp(object->valueForKey("name")->getCString(), "LevelCompletePoint") == 0)
		{
			level_complete_height_ = object->valueForKey("y")->floatValue();
		}
	}
}

void GameWorld::CreateHero(CCPoint position)
{
	// create the Hero at the given position
	hero_ = Hero::create(this);
	hero_->setPosition(ccp(position.x + hero_->getContentSize().width * 0.5f, position.y + hero_->getContentSize().height * 0.5f));
	// set the AABB for the Hero
	hero_->SetAABB(CCRectMake(position.x, position.y, HERO_AABB_WIDTH, HERO_AABB_HEIGHT));
	tiled_map_->addChild(hero_, E_LAYER_HERO);
}

void GameWorld::CreateEnemy(CCPoint position, CCPoint speed)
{
	// create the Enemy at the given position
	Enemy* enemy = Enemy::create(this);
	enemy->setPosition(ccp(position.x + enemy->getContentSize().width * 0.5f, position.y + enemy->getContentSize().height * 0.5f));
	// set the AABB & speed for this Enemy
	enemy->SetAABB(CCRectMake(position.x, position.y, ENEMY_AABB_WIDTH, ENEMY_AABB_HEIGHT));
	enemy->SetSpeed(speed);
	// push this Enemy into the vector
	enemies_.push_back(enemy);
	++ num_enemies_;
	tiled_map_->addChild(enemy, E_LAYER_ENEMIES);
}

void GameWorld::CreatePlatform(CCPoint position, CCPoint speed)
{
	// create the Platform at the given position
	Platform* platform = Platform::create(this, "moving_platform.png");
	platform->setPosition(ccp(position.x + platform->getContentSize().width * 0.5f, position.y + platform->getContentSize().height * 0.5f));
	// set the AABB & speed for this Platform
	platform->SetAABB(CCRectMake(position.x, position.y, platform->getContentSize().width, platform->getContentSize().height));
	platform->SetSpeed(speed);
	// push this Platform into the vector
	platforms_.push_back(platform);
	++ num_platforms_;
	tiled_map_->addChild(platform, E_LAYER_PLATFORMS);
}

void GameWorld::CreateControls()
{
	// create the control sprites
	left_arrow_btn_ = CCSprite::create("left_arrow.png");
	right_arrow_btn_ = CCSprite::create("right_arrow.png");
	jump_btn_ = CCSprite::create("jump.png");
	swing_btn_ = CCSprite::create("swing.png");

	// ergonomically place the controls
	left_arrow_btn_->setPosition(CCPoint(100.0f, 150.0f));
	right_arrow_btn_->setPosition(CCPoint(250.0f, 100.0f));
	jump_btn_->setPosition(CCPoint(1180.0f, 150.0f));
	swing_btn_->setPosition(CCPoint(1030.0f, 100.0f));

	// add the controls
	addChild(left_arrow_btn_);
	addChild(right_arrow_btn_);
	addChild(jump_btn_);
	addChild(swing_btn_);
}

void GameWorld::CreateHUD()
{
	// create & add the pause button's menu
	CCMenu* menu = CCMenu::create();
	menu->setAnchorPoint(CCPointZero);
	menu->setPosition(CCPointZero);
	addChild(menu);

	// create & add the pause button
	CCMenuItemSprite* pause_button = CCMenuItemSprite::create(CCSprite::create("pause_button.png"), CCSprite::create("pause_button.png"), this, menu_selector(GameWorld::OnPauseClicked));
	pause_button->setPosition(ccp(SCREEN_SIZE.width * 0.95f, SCREEN_SIZE.height * 0.9f));
	menu->addChild(pause_button);

	// save size of the life sprite
	CCSize icon_size = CCSprite::create("lives.png")->displayFrame()->getOriginalSize();
	// create an array to hold the life sprites
	life_sprites_ = CCArray::createWithCapacity(GameGlobals::hero_lives_left_);
	life_sprites_->retain();

	for(int i = 0; i < GameGlobals::hero_lives_left_; ++i)
	{
		CCSprite* icon_sprite = CCSprite::create("lives.png");
		icon_sprite->setPosition(ccp(icon_size.width * (i + 1), SCREEN_SIZE.height - icon_size.height));

		// add the life sprite to game world & array
		addChild(icon_sprite, E_LAYER_HUD);
		life_sprites_->addObject(icon_sprite);
	}
}

void GameWorld::update(float dt)
{
	// update the game's main elements
	UpdatePlatforms();
	UpdateEnemies();
	UpdateHero();

	// check for level completion
	if(hero_->GetCurrentGroundHeight() >= level_complete_height_)
	{
		hero_->SetState(E_HERO_STATE_WINNING);
	}

	// scroll the tiled map with some offset
	float curr_y = tiled_map_->getPositionY();
	float next_y = SCREEN_SIZE.height * 0.1f - hero_->GetCurrentGroundHeight();
	tiled_map_->setPositionY( curr_y + (next_y - curr_y) / 5 );

	// remove any dead enemies
	RemoveEnemies();
}

void GameWorld::UpdatePlatforms()
{
	for(int i = 0; i < num_platforms_; ++i)
	{
		platforms_[i]->Update();
	}
}

void GameWorld::UpdateEnemies()
{
	for(int i = 0; i < num_enemies_; ++i)
	{
		enemies_[i]->Update();
	}
}

void GameWorld::UpdateHero()
{
	hero_->Update(is_left_arrow_pressed_, is_right_arrow_pressed_, is_jump_pressed_, is_swing_pressed_);
}

bool GameWorld::CheckCollisions(GameObject* game_object)
{
	bool found_collision = false;
	found_collision = found_collision | CheckVerticalCollisions(game_object);
	found_collision = found_collision | CheckHorizontalCollisions(game_object);
	return found_collision;
}

bool GameWorld::CheckVerticalCollisions(GameObject* game_object)
{
	int visible_rows = (int)tiled_map_->getMapSize().height;
	int visible_cols = (int)tiled_map_->getMapSize().width;

	CCRect aabb = game_object->GetAABB();
	CCPoint speed = game_object->GetSpeed();

	// since we're checking vertically, save the row occupied by the aabb
	int aabb_row = GET_ROW_FOR_Y(aabb.origin.y, visible_rows);
	if(speed.y > 0)
	{
		// if we're going up, save the row occupied by the top edge of the aabb
		aabb_row = GET_ROW_FOR_Y(aabb.origin.y + aabb.size.height, visible_rows);
	}
	// also save the columns occupied by the left & right edges of the aabb
	int aabb_start_col = GET_COL_FOR_X(aabb.origin.x);
	int aabb_end_col = GET_COL_FOR_X(aabb.origin.x + aabb.size.width);

	// bounds checking
	if(aabb_row < 0 || aabb_row >= visible_rows ||
			aabb_start_col < 0 || aabb_start_col >= visible_cols ||
			aabb_end_col < 0 || aabb_end_col >= visible_cols)
		return false;

	// initialise flags & counters
	bool found_collidable = false;
	int current_col = aabb_start_col;
	int current_row = aabb_row;

	while(current_row >= 0 && current_row < visible_rows)
	{
		// check for every column that the aabb occupies
		for(current_col = aabb_start_col; current_col <= aabb_end_col; ++current_col)
		{
			// check if a brick exists at the given row & column
			if(bricks_layer_->tileGIDAt(ccp(current_col, current_row)))
			{
				found_collidable = true;
				break;
			}
		}

		// from current tile, keep moving in same direction till a brick is found
		if(found_collidable == false)
		{
			current_row = (speed.y < 0) ? (current_row + 1):(current_row - 1);
		}
		else
		{
#ifdef ICEMAN_DEBUG_MODE
			//bricks_layer_->tileAt(ccp(current_col, current_row))->runAction(CCSequence::createWithTwoActions(CCTintTo::create(0.1f, 255, 0, 0), CCTintTo::create(0.1f, 255, 255, 255)));
#endif
			break;
		}
	}

	if(found_collidable)
	{
		// going down
		if(speed.y < 0)
		{
			// if the bottom edge of aabb is lower than the top edge of the collidable row
			if(aabb.origin.y <= GET_Y_FOR_ROW(current_row, visible_rows))
			{
				// its a collision, do something
				game_object->CollisionResponse(current_col, current_row, E_COLLISION_BOTTOM);
			}
			else
			{
				// not a collision
				found_collidable = false;
			}
		}
		// going up
		else
		{
			// if the top edge of aabb is higher than the bottom edge of the collidable row
			if((aabb.origin.y + aabb.size.height) >= GET_Y_FOR_ROW(current_row + 1, visible_rows))
			{
				// its a collision, do something
				game_object->CollisionResponse(current_col, current_row, E_COLLISION_TOP);
			}
			else
			{
				// not a collision
				found_collidable = false;
			}
		}
	}

	return found_collidable;
}

bool GameWorld::CheckHorizontalCollisions(GameObject* game_object)
{
	int visible_rows = (int)tiled_map_->getMapSize().height;
	int visible_cols = (int)tiled_map_->getMapSize().width;

	CCRect aabb = game_object->GetAABB();
	CCPoint speed = game_object->GetSpeed();

	// since we're checking horizontally, save the column occupied by the right edge of aabb
	int aabb_col = GET_COL_FOR_X(aabb.origin.x + aabb.size.width);
	if(speed.x < 0)
	{
		// if we're going left, save the column occupied by the left edge of the aabb
		aabb_col = GET_COL_FOR_X(aabb.origin.x);
	}
	// also save the rows occupied by the top & bottom edges of the aabb
	int aabb_start_row = GET_ROW_FOR_Y(aabb.origin.y + aabb.size.height, visible_rows);
	int aabb_end_row = GET_ROW_FOR_Y(aabb.origin.y, visible_rows);

	// bounds checking
	if(aabb_col < 0 || aabb_col >= visible_cols ||
			aabb_start_row < 0 || aabb_start_row >= visible_rows ||
			aabb_end_row < 0 || aabb_end_row >= visible_rows)
		return false;

	// initialise flags & counters
	bool found_collidable = false;
	int current_col = aabb_col;
	int current_row = aabb_start_row;
	int current_tile = 0;

	while(current_col >= 0 && current_col < visible_cols)
	{
		// check for every row that the aabb occupies
		for(current_row = aabb_start_row; current_row <= aabb_end_row; ++current_row)
		{
			// check if a brick exists at the given row & column
			if(bricks_layer_->tileGIDAt(ccp(current_col, current_row)))
			{
				found_collidable = true;
				break;
			}
		}

		// from current tile, keep moving in same direction till a "collidable" tile is found
		if(found_collidable == false)
		{
			current_col = (speed.x < 0) ? (current_col - 1):(current_col + 1);
		}
		else
		{
#ifdef ICEMAN_DEBUG_MODE
			//bricks_layer_->tileAt(ccp(current_col, current_row))->runAction(CCSequence::createWithTwoActions(CCTintTo::create(0.1f, 255, 0, 0), CCTintTo::create(0.1f, 255, 255, 255)));
#endif
			break;
		}
	}

	if(found_collidable)
	{
		if(speed.x < 0)
		{
			// if the left edge of aabb is before the right edge of collidable column
			if( aabb.origin.x <= GET_X_FOR_COL(current_col + 1) )
			{
				// its a collision, do something
				game_object->CollisionResponse(current_col, current_row, E_COLLISION_LEFT);
			}
			else
			{
				// not a collision
				found_collidable = false;
			}
		}
		// going right
		else
		{
			// if the right edge of aabb is after the left edge of collidable column
			if( (aabb.origin.x + aabb.size.width) >= GET_X_FOR_COL(current_col) )
			{
				// its a collision, do something
				game_object->CollisionResponse(current_col, current_row, E_COLLISION_RIGHT);
			}
			else
			{
				// not a collision
				found_collidable = false;
			}
		}
	}

	return found_collidable;
}

void GameWorld::CheckHeroEnemyCollisions()
{
	CCRect hero_aabb = hero_->GetAABB();

	// loop through the list of enemies
	for(int i = 0; i < num_enemies_; ++i)
	{
		// check for collisions between the hero & enemy aabbs
		if(hero_aabb.intersectsRect(enemies_[i]->GetAABB()))
		{
			// if the hero is swinging
			if(hero_->GetState() == E_HERO_STATE_SWINGING)
			{
				// enemy dies
				enemies_[i]->SetState(E_ENEMY_STATE_DYING);
			}
			else
			{
				// hero dies
				ReduceHeroLives();
				hero_->SetState(E_HERO_STATE_DYING);
			}
		}
	}
}

void GameWorld::CheckHeroPlatformCollisions()
{
	CCRect hero_aabb = hero_->GetAABB();

	// loop through the list of platforms
	for(int i = 0; i < num_platforms_; ++i)
	{
		CCRect platform_aabb = platforms_[i]->GetAABB();
		// check for collisions between the hero & platform aabbs
		if(hero_aabb.intersectsRect(platform_aabb))
		{
			// is the hero below or above the platform
			if(hero_aabb.getMidY() <= platform_aabb.getMidY())
			{
				// reposition the hero one pixel below the platform
				hero_aabb.origin.y = platform_aabb.origin.y - hero_aabb.size.height - 1;
				hero_->SetAABB(hero_aabb);
				// hero should start falling down
				hero_->SetSpeed(ccp(hero_->GetSpeed().x, GRAVITY));
			}
			else
			{
				// inform the hero that he has landed on a platform
				hero_->SetPlatformBelow(platforms_[i]);
			}
			return;
		}
	}

	// inform the hero that there is no platform below him
	hero_->SetPlatformBelow(NULL);
}

void GameWorld::AddBrick(int tile_col, int tile_row)
{
	// bounds checking
	if(tile_col < 0 || tile_row < 0 || tile_col >= columns_ || tile_row >= rows_)
		return;

	// check if a brick already exists there
	if(bricks_layer_->tileAt(ccp(tile_col, tile_row)))
		return;

	// add a brick at the given column & row
	bricks_layer_->setTileGID(1, ccp(tile_col, tile_row));
}

void GameWorld::RemoveBrick(int tile_col, int tile_row)
{
	bricks_layer_->removeTileAt(ccp(tile_col, tile_row));
	SOUND_ENGINE->playEffect("brick.wav");
}

void GameWorld::RemoveEnemies()
{
	for(int i = 0; i < num_enemies_; ++i)
	{
		// remove the enemy from the list & the game world
		if(enemies_[i]->GetMustBeRemoved())
		{
#ifdef ICEMAN_DEBUG_MODE
			enemies_[i]->aabb_node_->removeFromParentAndCleanup(true);
#endif
			enemies_[i]->removeFromParentAndCleanup(true);
			enemies_.erase(enemies_.begin() + i);
			-- num_enemies_;
			break;
		}
	}
}

void GameWorld::ReduceHeroLives()
{
	// decrement lives left
	-- GameGlobals::hero_lives_left_;

	// safe checking
	if(GameGlobals::hero_lives_left_ < 0 || life_sprites_->count() <= 0)
	{
		return;
	}

	// remove the last life sprite from array
	CCSprite* life_sprite = (CCSprite*)life_sprites_->lastObject();
	life_sprites_->removeObject(life_sprite);

	// animate exit of life sprite
	CCActionInterval* scale_down = CCEaseBackIn::create(CCScaleTo::create(0.25f, 0.0f));
	CCActionInstant* remove_self = CCRemoveSelf::create(true);
	life_sprite->runAction(CCSequence::createWithTwoActions(scale_down, remove_self));
}

void GameWorld::ccTouchesBegan(CCSet* set, CCEvent* event)
{
	CCTouch* touch = (CCTouch*)(*set->begin());
	CCPoint touch_point = touch->getLocationInView();
	touch_point = CCDirector::sharedDirector()->convertToGL(touch_point);

	HandleTouch(touch_point, true);
}

void GameWorld::ccTouchesMoved(CCSet* set, CCEvent* event)
{
	CCTouch* touch = (CCTouch*)(*set->begin());
	CCPoint touch_point = touch->getLocationInView();
	touch_point = CCDirector::sharedDirector()->convertToGL(touch_point);

	HandleTouch(touch_point, true);
}

void GameWorld::ccTouchesEnded(CCSet* set, CCEvent* event)
{
	CCTouch* touch = (CCTouch*)(*set->begin());
	CCPoint touch_point = touch->getLocationInView();
	touch_point = CCDirector::sharedDirector()->convertToGL(touch_point);

	HandleTouch(touch_point, false);
}

void GameWorld::HandleTouch(CCPoint touch_point, bool is_touching)
{
	is_left_arrow_pressed_ = left_arrow_btn_->boundingBox().containsPoint(touch_point) & is_touching;
	is_right_arrow_pressed_ = right_arrow_btn_->boundingBox().containsPoint(touch_point) & is_touching;
	is_jump_pressed_ = jump_btn_->boundingBox().containsPoint(touch_point) & is_touching;
	is_swing_pressed_ = swing_btn_->boundingBox().containsPoint(touch_point) & is_touching;
}

void GameWorld::OnPauseClicked(CCObject* sender)
{
	// this prevents multiple pause popups
	if(is_popup_active_)
		return;

	// pause GameWorld update
	pauseSchedulerAndActions();
	setTouchEnabled(false);
	
	// pause game elements here
	hero_->pauseSchedulerAndActions();
	for(int i = 0; i < num_enemies_; ++i)
	{
		enemies_[i]->pauseSchedulerAndActions();
	}

	// create & add the pause popup
	PausePopup* pause_popup = PausePopup::create(this);
	addChild(pause_popup, E_LAYER_POPUP);
}

void GameWorld::ResumeGame()
{
	is_popup_active_ = false;

	// resume GameWorld update
	resumeSchedulerAndActions();
	setTouchEnabled(true);

	// resume game elements here
	hero_->resumeSchedulerAndActions();
	for(int i = 0; i < num_enemies_; ++i)
	{
		enemies_[i]->resumeSchedulerAndActions();
	}
}

void GameWorld::LevelComplete()
{
	is_popup_active_ = true;

	// stop GameWorld update
	unscheduleAllSelectors();
	setTouchEnabled(false);

	// stop game elements here
	hero_->stopAllActions();
	for(int i = 0; i < num_enemies_; ++i)
	{
		enemies_[i]->stopAllActions();
	}

	// create & add the game over popup
	LevelCompletePopup* level_complete_popup = LevelCompletePopup::create(this);
	addChild(level_complete_popup, E_LAYER_POPUP);
}

void GameWorld::GameOver()
{
	is_popup_active_ = true;

	// stop GameWorld update
	unscheduleAllSelectors();
	setTouchEnabled(false);

	// stop game elements here
	hero_->stopAllActions();
	for(int i = 0; i < num_enemies_; ++i)
	{
		enemies_[i]->stopAllActions();
	}

	// create & add the game over popup
	GameOverPopup* game_over_popup = GameOverPopup::create(this);
	addChild(game_over_popup, E_LAYER_POPUP);
	SOUND_ENGINE->playEffect("game_over.wav");
}