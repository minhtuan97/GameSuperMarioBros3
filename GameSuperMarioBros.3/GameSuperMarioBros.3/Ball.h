#pragma once
#include "GameObject.h"

#define BALL_BBOX_WIDTH 8
#define BALL_BBOX_HEIGHT 16

#define BALL_WALKING_SPEED_X 0.1f
#define BALL_WALKING_SPEED_Y 0.2f
#define BALL_GRAVITY	0.001f

#define BALL_ANIMATION_LEFT	0
#define BALL_ANIMATION_RIGHT	1

class Ball:public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	bool isthrow;
	Ball();
	//virtual void SetState(int state);
	void ColitionWithEnemy(vector<LPGAMEOBJECT>* listObject);
};

