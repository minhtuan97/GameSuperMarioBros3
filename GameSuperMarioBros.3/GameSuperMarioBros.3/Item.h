#pragma once
#include "GameObject.h"

class Item : public CGameObject
{
public:
	int id;
	int type;
	bool isEnable;
	bool isfinish;
	int xde, yde;
	Item();
	//Item(int typeItem);
	~Item();

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	//void TurnOnTimeStartEnable();

};

