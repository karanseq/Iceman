#include "GameGlobals.h"

CCSize GameGlobals::screen_size_ = CCSizeZero;
int GameGlobals::level_number_ = 0;
int GameGlobals::hero_lives_left_ = 0;

void GameGlobals::Init()
{
	screen_size_ = CCDirector::sharedDirector()->getWinSize();
	LoadData();
}

void GameGlobals::LoadData()
{
	// add Resources folder to search path. This is necessary when releasing for win32
	CCFileUtils::sharedFileUtils()->addSearchPath("Resources");

	// load sound effects & background music
	SOUND_ENGINE->preloadEffect("brick.wav");
	SOUND_ENGINE->preloadEffect("enemy_death.wav");
	SOUND_ENGINE->preloadEffect("game_over.wav");
	SOUND_ENGINE->preloadEffect("hero_death.wav");
	SOUND_ENGINE->preloadEffect("jump.wav");
	SOUND_ENGINE->preloadEffect("level_complete.wav");
	SOUND_ENGINE->preloadEffect("swing.wav");

	// create and add animations
	LoadAnimations();
}

void GameGlobals::LoadAnimations()
{
	CCAnimation* animation = NULL;

	// Hero animations
	animation = CCAnimation::create();
	animation->setDelayPerUnit(0.25f);
	animation->setLoops(-1);
	animation->addSpriteFrame(CCSprite::create("idle0001.png")->displayFrame());
	animation->addSpriteFrame(CCSprite::create("idle0002.png")->displayFrame());
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "HeroIdle");
	
	animation = CCAnimation::create();
	animation->setDelayPerUnit(0.1f);
	animation->setLoops(-1);
	animation->addSpriteFrame(CCSprite::create("walking0001.png")->displayFrame());
	animation->addSpriteFrame(CCSprite::create("walking0002.png")->displayFrame());
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "HeroWalking");
	
	animation = CCAnimation::create();
	animation->setDelayPerUnit(0.25f);
	animation->setLoops(-1);
	animation->addSpriteFrame(CCSprite::create("swinging0002.png")->displayFrame());
	animation->addSpriteFrame(CCSprite::create("swinging0001.png")->displayFrame());
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "HeroSwinging");
	
	animation = CCAnimation::create();
	animation->setDelayPerUnit(0.25f);
	animation->setLoops(2);
	animation->addSpriteFrame(CCSprite::create("dying0001.png")->displayFrame());
	animation->addSpriteFrame(CCSprite::create("dying0002.png")->displayFrame());
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "HeroDying");
	
	animation = CCAnimation::create();
	animation->setDelayPerUnit(0.25f);
	animation->setLoops(3);
	animation->addSpriteFrame(CCSprite::create("win0001.png")->displayFrame());
	animation->addSpriteFrame(CCSprite::create("win0002.png")->displayFrame());
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "HeroWinning");

	// Enemy animations	
	animation = CCAnimation::create();
	animation->setDelayPerUnit(0.2f);
	animation->setLoops(-1);
	animation->addSpriteFrame(CCSprite::create("enemy walking0001.png")->displayFrame());
	animation->addSpriteFrame(CCSprite::create("enemy walking0002.png")->displayFrame());
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "EnemyWalking");
	
	animation = CCAnimation::create();
	animation->setDelayPerUnit(0.25f);
	animation->addSpriteFrame(CCSprite::create("enemyBuilding0001.png")->displayFrame());
	animation->addSpriteFrame(CCSprite::create("enemyBuilding0002.png")->displayFrame());
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "EnemyBuilding");
	
	animation = CCAnimation::create();
	animation->setDelayPerUnit(0.25f);
	animation->addSpriteFrame(CCSprite::create("enemy dying0001.png")->displayFrame());
	animation->addSpriteFrame(CCSprite::create("enemy dying0002.png")->displayFrame());
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "EnemyDying");
}