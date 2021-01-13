#pragma once
#include "GameObject.h"

#define QUESTIONBRICK_BBOX_WIDTH  16
#define QUESTIONBRICK_BBOX_HEIGHT 16

class QuestionBrick:public CGameObject
{
public:
	bool isColiWithKoopas = false;
	bool isColiWithMario = false;
	int countColi = 0;
	int iditem;
	int xde, yde;
	bool isColi = false;
	bool isAddItem = false;
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

