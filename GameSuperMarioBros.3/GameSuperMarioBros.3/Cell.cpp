#include "Cell.h"
#include "Utils.h"

Cell::Cell()
{
	listObject.clear();
}

Cell::~Cell()
{

}

void Cell::add(LPGAMEOBJECT obj)
{
	for (int i = 0; i < listObject.size(); i++)
	{
		if (listObject.at(i) == obj)
			return;
	}
	listObject.push_back(obj);
}

void Cell::earseObj(LPGAMEOBJECT obj)
{
	listObject.erase(std::remove(listObject.begin(), listObject.end(), obj), listObject.end());
	for (int i = 0; i < listObject.size(); i++)
		if (listObject.at(i) == obj) {
			DebugOut(L"chua xoa obj \n");
			break;
		}
	//DebugOut(L"da xoa obj x=%f,y=%f\n",obj->x,obj->y);

}

vector<LPGAMEOBJECT> Cell::GetlistObject()
{
	return listObject;
}

void Cell::Unload()
{
	listObject.clear();
}

