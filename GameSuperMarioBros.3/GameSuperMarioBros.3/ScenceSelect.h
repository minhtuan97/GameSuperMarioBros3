#pragma once
#include "GameObject.h"

class ScenceSelect:public CGameObject
{
public:
	int id_scence;
	bool canleft = false;
	bool canright = false;
	bool candown = false;
	bool canup = false;

	ScenceSelect();
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

