#ifndef POPUPS_H_
#define POPUPS_H_

#include "GameGlobals.h"

class GameWorld;

class Popup : public CCLayerColor
{
public:
	Popup();
	~Popup();

	// create & add an empty CCMenu
	virtual CCMenu* AddMenu();
	// adds a CCMenuItem into the CCMenu
	virtual CCMenuItem* AddButton(CCMenuItem* button, CCPoint position);

	// Creates & adds a CCMenuItemLabel using a CCLabelTTF
	virtual CCMenuItemLabel* AddLabelButton(const char* text, CCPoint position, SEL_MenuHandler handler);
	// Creates & adds a CCMenuItemLabel using a CCLabelBMFont of given font name
	virtual CCMenuItemLabel* AddLabelButton(const char* text, const char* font, CCPoint position, SEL_MenuHandler handler);
	// Creates & adds a CCMenuItemSprite with specified frame_name
	virtual CCMenuItemSprite* AddSpriteButton(const char* frame_name, CCPoint position, SEL_MenuHandler handler);

	// button handler functions
	void ResumeGame(CCObject* sender);
	void RestartGame(CCObject* sender);
	void NextLevel(CCObject* sender);
	void QuitToMainMenu(CCObject* sender);

protected:
	GameWorld* game_world_;
	CCMenu* menu_;
};

class PausePopup : public Popup
{
public:
	PausePopup(GameWorld* game_world);
	~PausePopup();

	static PausePopup* create(GameWorld* game_world);
};

class LevelCompletePopup : public Popup
{
public:
	LevelCompletePopup(GameWorld* game_world);
	~LevelCompletePopup();

	static LevelCompletePopup* create(GameWorld* game_world);
};

class GameOverPopup : public Popup
{
public:
	GameOverPopup(GameWorld* game_world);
	~GameOverPopup();

	static GameOverPopup* create(GameWorld* game_world);
};

#endif // POPUPS_H_