#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scence.h"
#include "GameObject.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
#include "Koopas.h"
#include "Map.h"
#include "Camera.h"
#include "define.h"
#include "Grid.h"
#include "Board.h"

class CPlayScene: public CScene
{
protected: 
	Map* map;
	Grid* grid;
	Camera* cam;
	static CMario* player;					// A play scene has to have player, right? 
	CMario *player2;					// A play scene has to have player, right? 
	Board* board;

	vector<LPGAMEOBJECT> objects;	
	vector<LPGAMEOBJECT> objects_item;
	vector<LPGAMEOBJECT> listscenceselect;

	string s1 = "COURSE CLEAR";
	string s2 = "YOU GOT A CARD";
	Font string1;
	Font string2;
	Font card;
	bool iscard = false;
	int height = 188;
	int pace = 0;
	bool khoitao = false;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_MAP(string line);
	
public: 
	DWORD64 time_start = 0;
	DWORD64 time = 0;
	bool isjump = false;

	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	CMario * GetPlayer() { return player; } 

	//friend class CPlayScenceKeyHandler;
};

class CPlayScenceKeyHandler : public CScenceKeyHandler
{
public: 
	virtual void KeyState(BYTE *states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CPlayScenceKeyHandler(CScene *s) :CScenceKeyHandler(s) {};
};

