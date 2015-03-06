#include "MainMenu.h"
#include "GameWorld.h"

CCScene* MainMenu::scene()
{
    CCScene *scene = CCScene::create();
    MainMenu *layer = MainMenu::create();
    scene->addChild(layer);
    return scene;
}

bool MainMenu::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
	
	CCLayerColor* bg = CCLayerColor::create(ccc4(25, 49, 69, 255));
	addChild(bg);

	CCTMXTiledMap* tiled_map = CCTMXTiledMap::create("level_01.tmx");
	addChild(tiled_map);

	CCSprite* title = CCSprite::create("title.png");
	title->setPosition(ccp(SCREEN_SIZE.width * 0.5f, SCREEN_SIZE.height * 0.65f));
	addChild(title, E_LAYER_HUD);

	CCActionInterval* scale_up = CCEaseSineOut::create(CCScaleTo::create(1.0f, 1.05f));
	CCActionInterval* scale_down = CCEaseSineIn::create(CCScaleTo::create(1.0f, 1.0f));
	title->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(scale_up, scale_down)));

	// create & add the play button's menu
	CCMenu* menu = CCMenu::create();
	menu->setAnchorPoint(CCPointZero);
	menu->setPosition(CCPointZero);
	addChild(menu);

	// create & add the play button
	CCMenuItemSprite* play_button = CCMenuItemSprite::create(CCSprite::create("play_button.png"), CCSprite::create("play_button.png"), this, menu_selector(MainMenu::OnPlayClicked));
	play_button->setPosition(ccp(SCREEN_SIZE.width * 0.5f, SCREEN_SIZE.height * 0.35f));
	play_button->setScale(1.5f);
	menu->addChild(play_button);

	return true;
}

void MainMenu::OnPlayClicked(CCObject* sender)
{
	// handler function for the play button
	// start the player off with 3 lives
	GameGlobals::level_number_ = 1;
	GameGlobals::hero_lives_left_ = 3;
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, GameWorld::scene()));
}