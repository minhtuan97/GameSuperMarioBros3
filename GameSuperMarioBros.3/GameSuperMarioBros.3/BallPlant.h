#pragma once
#include "GameObject.h"


#define BALL_PLANT_SPEED_X 0.1f
#define BALL_PLANT_SPEED_Y 0.2f

class BallPlant :public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	bool isattack = false;
	BallPlant(float x, float y, int nx, int ny);

};

