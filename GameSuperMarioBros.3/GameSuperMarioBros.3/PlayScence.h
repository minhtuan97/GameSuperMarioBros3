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
#include "SelectPlayer.h"

#define SCENCE_ID_INTRO	0
#define SCENCE_ID_MAP	1
#define SCENCE_ID_4_1	5

#define SPEED_ITEM_Y	0.3f

#define BROAD_X	8
#define BROAD_Y	200

#define MAN_INTRO_X_MIN	-16
#define MAN_SPRITE_ID	41001
#define MAN_SPRITE_ID2	41002
#define MAN_SPRITE_ID3	41003

#define MAN_YUPDATE	2

#define MAN_SPRITE_MAP	40050
#define	WORLDMAP_X	5
#define WORLDMAN_Y	20
#define TIME_MAX	300

#define MARIOSAMLL_SPEED_Y_PIPE	0.01
#define MARIOSUPPER_SPEED_Y_PIPE	0.02
#define MARIO_SPEED_Y_FLY	0.1f

#define MAX_BALL	2




class CPlayScene: public CScene
{
protected: 
	Map* map;
	Grid* grid;
	Camera* cam;
	static CMario* player;					// A play scene has to have player, right? 
	CMario* playermap;
	SelectPlayer* selectplayer;
	Board* board;

	vector<LPGAMEOBJECT> objects;	
	vector<LPGAMEOBJECT> objects_item;
	vector<LPGAMEOBJECT> listscenceselect;

	string s1 = "COURSE CLEAR";
	string s2 = "YOU GOT A CARD";
	Font* string1;
	Font* string2;
	Font* card;
	DWORD cardstart;
	bool iscard = false;
	int height = 188;
	int pace = 0;
	bool khoitao = false;
	float cxcount = 0;

	void _ParseSection_SETTINGSCENCE(string line);
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
	bool isCanS = false;
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	CMario* GetPlayer() { return player; }
	CMario* GetPlayerMap() { return playermap; }
	SelectPlayer * GetSelectPlayer() { return selectplayer; }

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

