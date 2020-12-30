#include "Camera.h"

Camera* Camera::__instance = NULL;

Camera::Camera()
{
	cameraPosition = D3DXVECTOR3(0, 0, 0);
	Setwidthheight(260,180);
}

Camera* Camera::GetInstance()
{
	if (__instance == NULL) __instance = new Camera();
	return __instance;
}

D3DXVECTOR3 Camera::GetPositionInCamera(D3DXVECTOR3 position)
{
	D3DXVECTOR3 pos;
	pos.x = position.x - this->cameraPosition.x;
	pos.y = position.y - this->cameraPosition.y;
	return D3DXVECTOR3(pos.x, pos.y, 0);
}

void Camera::SetCameraPosition(float x, float y)
{
	if (!block)
	{
		this->cameraPosition.x = x;
		this->cameraPosition.y = y;
	}
}

void Camera::Setwidthheight(int x, int y)
{
	this->width = x;
	this->height = y;
}

std::vector<LPGAMEOBJECT> Camera::GetlistinCamera(vector<LPGAMEOBJECT> obj)
{
	vector<LPGAMEOBJECT> list;
	list.clear();
	float x = cameraPosition.x;
	float y = cameraPosition.y;

	for (int i = 0; i < obj.size(); i++)
	{
		if (obj.at(i)->x > x && obj.at(i)->x <x + width && obj.at(i)->y >y && obj.at(i)->y < y + height)
			list.push_back(obj.at(i));
	}
	return list;
}

D3DXVECTOR3 Camera::GetCameraPosition()
{
	return this->cameraPosition;
}

void Camera::Unload()
{
	if (__instance == NULL) return;
	else
	{
		__instance = NULL;
	}
}

Camera::~Camera()
{

}




