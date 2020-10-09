#include "Grid.h"
#include "define.h"
#include "Map.h"
#include "Utils.h"
#include "PlayScence.h"

Grid* Grid::_instance = NULL;

bool Grid::checkObj(vector<LPGAMEOBJECT>* listobj, LPGAMEOBJECT e)
{
	LPGAMEOBJECT obj;
	for (int i = 0; i < listobj->size(); i++)
	{
		obj = listobj->at(i);
		if (obj == e) {
			return true;
		}
	}
	return false;
}

Grid* Grid::GetInstance()
{
	if (_instance == NULL)
		_instance = new Grid();
	return _instance;
}

Grid::Grid()
{
	Map* map = Map::GetInstance();
	cellwidth = (int)SCREEN_WIDTH / 2;
	cellheight = (int)SCREEN_HEIGHT / 2;
	cell_cloumn = ceil((float)map->GetWidth() / cellwidth);
	cell_row = ceil((float)map->GetHeight() / cellheight);
	cells = new LCell[cell_cloumn];
	for (int i = 0; i < cell_cloumn; i++)
		cells[i] = new Cell[cell_row];
	DebugOut(L"cellwidth: %d, cellheight: %d, cell_cloumn: %d, cell_row: %d \n", cellwidth, cellheight, cell_cloumn, cell_row);
}

Grid::~Grid()
{
}

void Grid::addObject(LPGAMEOBJECT object)
{
	int cell_x = floor((float)object->x / cellwidth);
	int cell_y = floor((float)object->y / cellheight);

	cells[cell_x][cell_y].add(object);

	//DebugOut(L"cell_x: %d, cell_y: %d\n", cell_x, cell_y);
	//if (dynamic_cast<Torch*>(object))
		//DebugOut(L"object duoc add x:%f, y:%f \n", object->x, object->y);



}

void Grid::GetListOfObjects(vector<LPGAMEOBJECT>* list_object)
{
	Camera* cam = Camera::GetInstance();
	list_object->clear();
	int left, top, right, bottom;
	int i, j, k;

	left = (int)cam->GetCameraPosition().x / cellwidth;
	top = (int)cam->GetCameraPosition().y / cellheight;

	right = ceil((float)(cam->GetCameraPosition().x + SCREEN_WIDTH) / cellwidth);

	bottom = ceil((float)(cam->GetCameraPosition().y + SCREEN_HEIGHT) / cellheight);

	LCell cell;
	for (i = left; i < right; i++)
	{
		for (j = top; j < bottom; j++)
		{
			LCell cell = &cells[i][j];
			if (cell)
			{
				if (cells[i][j].GetlistObject().size() != 0)
				{
					for (k = 0; k < cells[i][j].GetlistObject().size(); k++)
					{
						//DebugOut(L"i:%d, j:%d, k:%d, size:%d \n", i, j, k, cells[i][j].GetlistObject().size());
						LPGAMEOBJECT e;
						e = cells[i][j].GetlistObject().at(k);
						list_object->push_back(e);
					}
				}
			}
		}
	}
}


void Grid::ClearObject()
{
	if (_instance == NULL)
		return;
	/*for (int i = 0; i < cell_cloumn; i++)
		for (int j = 0; j < cell_row; j++)
			cells[i][j].Unload();
	_instance == NULL;
	delete _instance;*/


	if (NULL != _instance)
	{
		delete _instance;
		_instance = NULL;
	}
	return;
}

void Grid::deleteObject(LPGAMEOBJECT object)
{
	int cell_x = 0, cell_y = 0;
	//if (dynamic_cast<Item*>(object))
	//{
	//	Item* item = dynamic_cast<Item*>(object);
	//	cell_x = floor(item->x_de / cellwidth);
	//	cell_y = floor(item->y_de / cellheight);
	//}
	//else
	//{
	//	cell_x = floor((float)object->x / cellwidth);
	//	cell_y = floor((float)object->y / cellheight);
	//}
	cells[cell_x][cell_y].earseObj(object);

}

void Grid::Update(LPGAMEOBJECT object)
{
	for (int i = 0; i < cell_cloumn; i++)
		for (int j = 0; j < cell_row; j++)
		{
			for (int k = 0; k < cells[i][j].GetlistObject().size(); k++)
			{
				LPGAMEOBJECT e;
				e = cells[i][j].GetlistObject().at(k);
				int cell_x = floor((float)object->x / cellwidth);
				int cell_y = floor((float)object->y / cellheight);
				if (e == object && (cell_x != i || cell_y != j))
				{
					cells[i][j].earseObj(object);
					addObject(object);
					DebugOut(L"da update grid cho object\n");
					return;
				}
			}
		}
}
