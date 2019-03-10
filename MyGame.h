#pragma once
#include "Game.h"

class CMyGame : public CGame
{
public:
	CMyGame();
	~CMyGame();

    // ----  Declare your game variables and objects here -------------
	// Variables
	int score;
	int level;

    // Declare Sprites
    CSprite startScreen;

    // Declare Models
    CModel house;
	CModel grass;
	CModel tomb;
	CModel box;
    CModelMd2 player;     // animated model
    CModelMd2 enemy;	 // enemy md2 model
	
	//  Declare Model Lists
	CModelList shots;     // list containing shots
	CModelList enemies;   // list containing enemy models
	CModelList outerWalls; // list containing outer walls
	CModelList grassList;
	CModelList tombs;

	// game world floor
	CFloor floor;
	
	// Font
	CFont font;
	
	// health indicator
	CHealthBar hbar;

	 // -----   Add you member functions here ------
   
   void PlayerControl();
   void EnemyControl(); 
   void ShotsControl();
   
   void CameraControl(CGraphics* g);
	
   // ---------------------Event Handling --------------------------

	// Game Loop Funtions
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);
	virtual void OnRender3D(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartLevel(int level);
	virtual void OnStartGame();
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
