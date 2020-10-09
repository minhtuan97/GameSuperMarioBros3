#pragma once
#include "GameObject.h"

class Cell
{
	vector<LPGAMEOBJECT> listObject;

public:
	Cell();
	~Cell();
	void add(LPGAMEOBJECT obj);
	void earseObj(LPGAMEOBJECT obj);

	vector<LPGAMEOBJECT> GetlistObject();
	void Unload();
};
typedef Cell* LCell;