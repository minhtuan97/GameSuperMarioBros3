#pragma once
#include "GameObject.h"

#define GOOMBA_WALKING_SPEED 0.05f
#define GOOMBA_FLY_SPEED -0.3f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 15
#define GOOMBA_FLY_WALKING_BBOX_HEIGHT 19
#define GOOMBA_FLY_BBOX_HEIGHT 24
#define GOOMBA_BBOX_HEIGHT_DIE 9

#define GOOMBA_TYPE_FLY 1


#define GOOMBA_STATE_WALKING 100
#define GOOMBA_STATE_DIE 200
#define GOOMBA_STATE_FLY 300
#define GOOMBA_STATE_WALKING2 400


#define GOOMBA_ANI_WALKING 0
#define GOOMBA_ANI_DIE 1
#define GOOMBA_ANI_FLY 3
#define GOOMBA_ANI_FLY_WALK 2
#define GOOMBA_ANI_FLY_WALK2 4
#define GOOMBA_ANI_DIE2	5



#define GOOMBA__GRAVITY	0.001f
#define TIME_DIE	500
#define TIME_FLY	1000
#define TIME_WALK	1000



class CGoomba : public CGameObject
{
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();

public: 
	int type;
	bool iskilltop = false;
	CGoomba(int nx, int type);
	DWORD startdie;
	DWORD startfly;
	DWORD startwalk;
	virtual void SetState(int state);
};