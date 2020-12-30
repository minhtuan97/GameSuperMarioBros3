#pragma once
#include <d3dx9.h>
#include <vector>
#include "GameObject.h"

class Camera
{
private:
	D3DXVECTOR3 cameraPosition;
	int width, height;
	static Camera* __instance;


public:
	bool block = false;
	Camera();
	static Camera* GetInstance();
	D3DXVECTOR3 GetPositionInCamera(D3DXVECTOR3 position);
	void SetCameraPosition(float x, float y);
	void Setwidthheight(int x, int y);
	std::vector<LPGAMEOBJECT> GetlistinCamera(vector<LPGAMEOBJECT> obj);
	D3DXVECTOR3 GetCameraPosition();
	void Unload();
	~Camera();

};

