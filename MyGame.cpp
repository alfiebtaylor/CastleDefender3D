#include "MyGame.h"

using namespace std;

CMyGame::CMyGame(void) {}
CMyGame::~CMyGame(void) {}


// --------  game initialisation --------
void CMyGame::OnInitialize()
{
	// Loading graphics and sound assets
	cout << "Loading assets" << endl;

	font.LoadDefault(); 

	// enable lighting
	Light.Enable();

    // house model
    house.LoadModel("output.obj","output.bmp"); 
	house.SetScale( 40.0f);

	// player model
	player.LoadModel("Abarlith.md2","Abarlith.bmp"); 
	player.SetScale( 3.5f);
	
	// enemy model
	enemy.LoadModel("Ogro.md2","Ogro.bmp"); 
	enemy.SetScale( 3.5f); enemy.SetToFloor(0);
	enemy.PlayAnimation(1,39,7,true);
	
	// floor texture
	floor.LoadTexture("floor2.bmp");
	floor.SetTiling(true);
	
	// grass texture
	grass.LoadModel("grass.obj");
	grass.LoadTexture("grass.tga");

	// tomb texture
	tomb.LoadModel("tomb.obj");
	tomb.LoadTexture("tomb.bmp");
	tomb.SetScale(25.0f);

	// box model
	box.LoadModel("box.obj");
	box.LoadTexture("box.bmp");
	box.SetScale(30.0f);

	// start screen
	startScreen.LoadImage("startScreen.bmp");
	startScreen.SetPosition(Width/2,Height/2);
	level=1;
}


void CMyGame::OnStartLevel(int level)
{
	  // set size of the game world
	  floor.SetSize(9000,9000);
  
	  enemies.clear();
	  shots.clear();

	  // create four outer walls with a dark brown colour
	  outerWalls.clear();
	  CModel* pWall = new CModel(0,25,-1500,3000,60,50,CColor(50,30,10));
	  outerWalls.push_back(pWall);
	  pWall = new CModel(0,25, 1500,3000,60,50,CColor(50,30,10));
	  outerWalls.push_back(pWall);
	  pWall = new CModel(1500,25, 0,50,60,3000,CColor(50,30,10));
	  outerWalls.push_back(pWall);
	  pWall = new CModel(-1500,25, 0,50,60,3000,CColor(50,30,10));
	  outerWalls.push_back(pWall);


	  // set positions
	  player.SetPosition(400,100,300);
	  box.SetPosition(312, 25, 50);
  
	  // reset score
	  score = 0; 

	  // setup grass
	  for (int i = 0; i < 300; i++)
	  {
		  float randXPos = int(3000 - rand() % 6000);
		  float randZPos = int(3000 - rand() % 6000);
		  CModel* pModel = grass.clone();
		  pModel->SetScale(2.0f);
		  pModel->SetPosition(randXPos, 25, randZPos);

		  grassList.push_back(pModel);
	  }

	  // setup tombs
	  for (int i = 0; i < 200; i++)
	  {
		  float randXPos = int(-1500 - rand() % 1500);
		  float randZPos = int(-1500 - rand() % 1500);
		  CModel* pModel = tomb.clone();
		  pModel->SetScale(1.0);
		  pModel->SetPosition(randXPos, 25, randZPos);
	  }

	  // setup healthbar
	  hbar.SetSize(100,15);
	  hbar.SetPosition(830,680);
	  hbar.SetHealth(100);
}

// Game Logic in the OnUpdate function called every frame
void CMyGame::OnUpdate() 
{
	if (IsMenuMode() || IsGameOver()) return;
	
	long t =  GetTime();
	
	// --- updating models ----
	player.Update(t);
	enemies.Update(t);
	grass.Update(t);
	enemy.Update(t);
	shots.Update(t);
	box.Update(t);

	for (CModel* pShot : shots)
	{
		pShot->Update(t);
	}
	
    PlayerControl();
    EnemyControl();
    ShotsControl();
    
    if (hbar.GetHealth() <= 0) GameOver();
	
	if (player.HitTest(&box))
	{
		player.SetSpeed(0);
	}
}


void CMyGame::PlayerControl()
{
	// -----  player control --------
	if (IsKeyDown(SDLK_w))
	{ 
		// don't move player forward when colliding with the house
		if (!player.HitTestFront(&house)) player.SetSpeed(800);
		else player.SetSpeed(0);	
	}

	else player.SetSpeed(0);

	// play running sequence when UP key is pressed
	// otherwise play standing sequence
	if (IsKeyDown(SDLK_w))  player.PlayAnimation(40,45,7,true);
	else player.PlayAnimation(1,39,7,true);
	
	// prevent player from leaving the game area (3000x3000) in the x-z plane
	if (player.GetX() > 1400) player.SetX(1400);
	if (player.GetX() < -1400) player.SetX(-1400);
	if (player.GetZ() > 1400) player.SetZ(1400);
	if (player.GetZ() < -1400) player.SetZ(-1400);
	
	// collision detection between player and enemies
	for (CModel* pEnemy : enemies)
	{
	 if (player.HitTest( pEnemy))
	 {
		GameOver();
	 }
    }
}

void CMyGame::EnemyControl()
{
	// create new enemy every two seconds @ 30 fps
	if (rand()%60==0)
	{   
		// select randomly a side
		int s = rand()%4;
		// clone the current enemy as a template
		CModel* pEnemy = enemy.clone();
		if (s==0) pEnemy->SetPosition(3000,float(1000-rand()%2000));
		else if(s==1) pEnemy->SetPosition(-3000,float(1000-rand()%2000));
		else if(s==2) pEnemy->SetPosition(float(1000-rand()%2000),3000);
		else pEnemy->SetPosition(float(1000-rand()%2000),-3000);
		
		pEnemy->SetToFloor(0);
		pEnemy->PlayAnimation(40,45,5,true); // select running animation
		// direct the enemy in the direction of the house
		pEnemy->SetDirectionAndRotationToPoint( house.GetX(), house.GetZ());
	    pEnemy->SetSpeed(100);
		 	
		enemies.push_back( pEnemy);
	}
	
    // Enemy A.I.
	for (CModel* pEnemy : enemies)
	{
	  // reduce health bar once an enemy has reached the house
	  if (house.HitTest(pEnemy)) 
	  {
		hbar.SetHealth(hbar.GetHealth()-10);
		pEnemy->Delete();
	  }
	}
	enemies.remove_if(deleted);
}


void CMyGame::ShotsControl()
{
	// collision detection between shots and enemies
	for (CModel* pEnemy : enemies)
	{
	 for (CModel* pShot : shots)
	 {
		if (pShot->HitTest(pEnemy))
		{
			score++;
			pEnemy->Delete();
			pShot->Delete();
			break;
		} 
	 }
    }
    enemies.remove_if(deleted);
    shots.remove_if(deleted);
    
    for (CModel* pShot: shots)
	{
		// remove shots if outside the game area
		if (pShot->GetX() > 2000) { pShot->Delete(); break; }
	    if (pShot->GetX() < -2000) { pShot->Delete(); break; }
	    if (pShot->GetZ() > 2000) { pShot->Delete(); break; }
	    if (pShot->GetZ() < -2000) { pShot->Delete(); break; }
	    
	    // remove shots when hitting the house	    
	    if (pShot->HitTest(&house)) { { pShot->Delete(); break; } }
	 }
     shots.remove_if(deleted);
}

//-----------------  Draw 2D overlay ----------------------
void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode())
	{
	 startScreen.Draw(g);
	 return;
	}
	// draw score
	font.SetSize(32); font.SetColor( CColor::Red()); font.DrawNumber(10,Height-50, score);

	// draw coordinates of player's location
	font.SetSize(32); font.SetColor(CColor::White()); font.DrawNumber(10, Height - 575, player.GetX());
	font.SetSize(32); font.SetColor(CColor::White()); font.DrawNumber(10, Height - 550, player.GetY());
	font.SetSize(32); font.SetColor(CColor::White()); font.DrawNumber(10, Height - 525, player.GetZ());
	
	// draw GAME OVER if game over
   	if (IsGameOver())
   	{
		font.SetSize(64); font.SetColor( CColor::Red()); font.DrawText( 250,300, "GAME OVER");	
	}
	
	hbar.Draw(g);
}

// ----------------   Draw 3D world -------------------------
void CMyGame::OnRender3D(CGraphics* g)
{
	CameraControl(g);
    
	// ------- Draw your 3D Models here ----------
	
	floor.Draw(g);
	//floor.Draw(true); // draw grid
	outerWalls.Draw(g);
	tombs.Draw(g);
	enemies.Draw(g);
	player.Draw(g);
	shots.Draw(g);
	house.Draw(g);
	box.Draw(g);
	
	// draw each model in grass list
	for each (CModel* pModel in grassList)
	{
		pModel->Draw(g);
	}

	ShowCoordinateSystem();
	
}

void CMyGame::CameraControl(CGraphics* g)
{
	// game world tilt 
    float tilt=60;
    float scale=1.0f;
    float rotation=-35;
	
	// ------ Global Transformation Functions -----
	glTranslatef(0,-100,0);  // move game world down 
	
	glRotatef(tilt,1,0,0);	// tilt game world around x axis
	
	glScalef( scale,scale,scale);			// scaling the game world
	glRotatef(rotation,0,1,0);				// rotating the game world
	
	// ---- 3rd person camera setup -----
	// translate game world to player position
	glTranslatef(-player.GetX(), 0, -player.GetZ());    
	
	UpdateView();
	Light.Apply();
}


// called at the start of a new game - display menu here
void CMyGame::OnDisplayMenu()
{
		
}


// called when Game Mode entered
void CMyGame::OnStartGame()
{
     OnStartLevel(1);	
   
}


// called when Game is Over
void CMyGame::OnGameOver()
{
	
}


// one time termination code
void CMyGame::OnTerminate()
{
	
}

// -------    Keyboard Event Handling ------------

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	
	if (sym == SDLK_SPACE)
	{
		if (IsPaused()) ResumeGame();
		else PauseGame();

	}
	if (sym == SDLK_F2) NewGame();
}


void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	
}

// -----  Mouse Events Handlers -------------

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	// project screen to floor coordinates
	if (!IsGameOver())
	{
	  CVector pos=ScreenToFloorCoordinate(x,y);
	  player.SetDirectionAndRotationToPoint(pos.x,pos.z);
    }
	
}


void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{    
	if (IsMenuMode()) StartGame();
	else
	{
	  // create a new shot dynamically as a short line
	  CModel* pShot = new CLine(player.GetPositionV(), 50);
	  // rotation and direction same as the player
	  pShot->SetDirection(player.GetRotation());
	  pShot->SetRotation(player.GetRotation());
	  pShot->SetSpeed(3000);
	  pShot->Move(50); // move starting point in front of the player
	  shots.push_back(pShot);
    }
		
	

}


void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}


void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}


void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}


void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}


void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}