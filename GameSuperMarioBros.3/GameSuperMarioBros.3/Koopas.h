#pragma once

#include "GameObject.h"

#define KOOPAS_WALKING_SPEED 0.02f;
#define KOOPAS_KICK_SPEED 0.3f;

#define KOOPAS_BBOX_WIDTH 16
#define KOOPAS_BBOX_HEIGHT 26
#define KOOPAS_BBOX_HEIGHT_DIE 16

#define KOOPAS_STATE_WALKING 100
#define KOOPAS_STATE_DIE 200
#define KOOPAS_STATE_KICK 300
#define KOOPAS_STATE_HOLD 400

#define KOOPAS_ANI_WALKING_LEFT 0
#define KOOPAS_ANI_WALKING_RIGHT 1
#define KOOPAS_ANI_KICK	4
#define KOOPAS_ANI_DIE 2
#define KOOPAS_ANI_HOLD 2

class CKoopas : public CGameObject
{
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();

public:
	bool ishold = false;
	int nx=1;
	float x_min, x_max;
	CKoopas();
	virtual void SetState(int state);
};