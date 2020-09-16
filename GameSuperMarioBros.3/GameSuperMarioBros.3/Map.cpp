#include "Map.h"
#include "Utils.h"
#include "Camera.h"
#include "define.h"


#define SCENE_SECTION_UNKNOWN 0
#define SCENE_SECTION_SETTING 1
#define SCENE_SECTION_DATA 2

#define MAX_SCENE_LINE 1024


Map* Map::_instance = NULL;

Map::Map()
{
}

void Map::_ParseSection_SETTING(string line)
{
	//TotalTiles >> RowMap >> ColumnMap >> RowTile >> ColumnTile >> widthTile >> heightTile;
	vector<string> tokens = split(line," ");

	if (tokens.size() < 7) return; // skip invalid lines

	TotalTiles = atoi(tokens[0].c_str());
	RowMap = atoi(tokens[1].c_str());
	ColumnMap = atoi(tokens[2].c_str()); 
	RowTile = atoi(tokens[3].c_str());
	ColumnTile = atoi(tokens[4].c_str());
	widthTile = atoi(tokens[5].c_str());
	heightTile = atoi(tokens[6].c_str());
}

void Map::_ParseSection_DATA(string line)
{
	vector<string> tokens = split(line," ");

	if (tokens.size() < RowMap) return; // skip invalid lines

	for (int j = 0; j < ColumnMap; j++)
	{
		TileMap[linedata][j] = atoi(tokens[j].c_str());
		DebugOut(L"xuat : %d", TileMap[linedata][j]);
	}
	linedata++;
}

void Map::LoadFile(LPCWSTR filename, int idTextureMap)
{
	DebugOut(L"[INFO] Start loading scene resources fileMap\n");

	ifstream f;
	f.open(filename);
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[SETTING]") { section = SCENE_SECTION_SETTING; continue; }
		if (line == "[DATA]") { section = SCENE_SECTION_DATA; linedata = 0; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_SETTING: _ParseSection_SETTING(line); break;
		case SCENE_SECTION_DATA: _ParseSection_DATA(line); break;

		}
	}

	f.close();

	CTextures* textures = CTextures::GetInstance();

	CSprites* sprites = CSprites::GetInstance();

	LPDIRECT3DTEXTURE9 texMap1 = textures->Get(idTextureMap);

	for (int i = 0; i < RowTile; i++)
		for (int j = 0; j < ColumnTile; j++)
		{
			sprites->Add(50000 + i * ColumnTile + j, j * widthTile, i * heightTile, (j + 1) * widthTile, (i + 1) * heightTile, texMap1);
		}

}


void Map::DrawMap()
{
	//Camera* cam = Camera::GetInstance();
	//int tile_column = (int) SCREEN_WIDTH / widthTile+2;
	//int tile_row = RowTile;
	//float mx, my;
	//mx = cam->GetCameraPosition().x;
	////my = cam->GetCameraPosition().y;
	//int tile_start_column = (int)mx / widthTile;
	////int tile_start_row = (int)my / widthTile;
	//CSprites* sprites = CSprites::GetInstance();
	//int a = -1;
	////DebugOut(L"bat dau\n");
	//for (int i = 0; i < RowMap; i++)
	//	for (int j = tile_start_column; j < tile_column+ tile_start_column; j++)
	//	//for (int j = 0; j < ColumnMap; j++)
	//	{
	//		
	//		a = TileMap[i][j];
	//		if (a >= 0)
	//			sprites->Get(50000 + a)->Draw(j * widthTile, i * heightTile);
	//		//if (a == 55)DebugOut(L"%d	%d\n", j * widthTile, i * heightTile);
	//	}
	////DebugOut(L"bhet\n");


	CSprites* sprites = CSprites::GetInstance();
	int a = -1;
	//DebugOut(L"bat dau\n");
	for (int i = 0; i < RowMap; i++)
		for (int j = 0; j < ColumnMap; j++)
		{
			a = TileMap[i][j];
			if (a >= 0)
				sprites->Get(50000 + a)->Draw(j * widthTile, i * heightTile);
		}
	DebugOut(L"bhet\n");
}

void Map::Clear()
{
	if (_instance == NULL)
		return;
	else
	{
		/*for (int i = 0; i < RowMap; i++)
			delete[] TileMap[i];
		delete[] TileMap;*/
		_instance = NULL;
	}
}

float Map::GetHeight()
{
	return heightTile* (RowMap);
}

float Map::GetWidth()
{
	return widthTile * (ColumnMap);
}

Map* Map::GetInstance()
{
	if (_instance == NULL) _instance = new Map();
	return _instance;
}





Map::~Map()
{
}
