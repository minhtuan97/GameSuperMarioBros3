#pragma once
#include "GameObject.h"

#define PLANT_STATE_UP	0
#define PLANT_STATE_DOWN	1
#define PLANT_STATE_ATTACK	2
#define PLANT_STATE_WAIT	3

#define PLANT_SPEED_Y 0.02f;

#define PLANT_ATTACK_TIME 2000
#define PLANT_WAIT_TIME 3000

class Plant:public CGameObject
{

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	bool isColi = false;
	DWORD attack_start;
	DWORD wait_start;
	int nx = 1;
	int ny = 1;
	float y_min, y_max;
	int type;
	bool isattack = false;
	Plant(int type, float x, float y);
	virtual void SetState(int state);
};

