#pragma once

#include "GameObject.h"

#define KOOPAS_WALKING_SPEED 0.02f;
#define KOOPAS_SPEED_Y 0.02f
#define KOOPAS_SPEED_Y_DIE 0.2f
#define KOOPAS_KICK_SPEED 0.3f;
#define KOOPAS_FLY_SPEED -0.2f

#define KOOPAS_BBOX_WIDTH 16
#define KOOPAS_BBOX_HEIGHT 26
#define KOOPAS_BBOX_HEIGHT_DIE 16

#define KOOPAS_STATE_WALKING 100
#define KOOPAS_STATE_DIE 200
#define KOOPAS_STATE_KICK 300
#define KOOPAS_STATE_HOLD 400
#define KOOPAS_STATE_FLY 500

#define KOOPAS_ANI_WALKING_LEFT 0
#define KOOPAS_ANI_WALKING_RIGHT 1
#define KOOPAS_ANI_KICK	4
#define KOOPAS_ANI_DIE 2
#define KOOPAS_ANI_HOLD 2
#define KOOPAS_ANI_FLY 14

#define	KOOPAS_GREEN_ANI_FLY_RIGHT	11
#define KOOPAS_GREEN_ANI_FLY_LEFT	12
#define	KOOPAS_GREEN_ANI_WALKING_RIGHT	5
#define	KOOPAS_GREEN_ANI_WALKING_LEFT	6
#define	KOOPAS_GREEN_ANI_KICK	7
#define	KOOPAS_GREEN_ANI_HOLD	8
#define KOOPAS_GREEN_ANI_DIE	13


class CKoopas : public CGameObject
{
public:

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();
	bool ishold = false;
	int nx=1;
	float x_min, x_max;
	int type;
	CKoopas(int type, float x,float y);
	virtual void SetState(int state);
};