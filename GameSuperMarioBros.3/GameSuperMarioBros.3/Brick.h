#pragma once
#include "GameObject.h"

#define BRICK_BBOX_WIDTH  16
#define BRICK_BBOX_HEIGHT 16
#define CLOUD	1
#define BRICK2	2
#define BRICK3	3
#define BRICK4	4
#define BRICK5	5

#define BRICK_GRAVITY	0.001f

class CBrick : public CGameObject
{

public:
	CBrick(float x,float y);
	float x_de, y_de;
	bool isColi = false;
	int type;
	bool isP = false;
	int right, bot;
	bool isAddItem = false;

	virtual void Render();
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
};