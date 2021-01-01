#pragma once
#include "Sprites.h"
#include "Textures.h"
#include <fstream> 
#include <string>
#include <stdio.h>
#include <Windows.h>  
#include <iostream>
#include "Animations.h"

#define ID_TEX_MAP1 50
class Map
{
	static Map* _instance;
	int ColumnTile=0;
	int RowTile=0;
	int TotalTiles=0;
	int widthTile=16, heightTile=16;
	int ColumnMap, RowMap;
	int TileMap[500][500];
	int linedata = 0;
public:
	Map();
	void LoadFile(LPCWSTR filename, int idTextureMap);
	void DrawMap();
	void Clear();
	float GetHeight();
	float GetWidth();
	static Map* GetInstance();

	void _ParseSection_SETTING(string line);
	void _ParseSection_DATA(string line);

	~Map();
};

