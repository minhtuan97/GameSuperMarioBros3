#include "SelectPlayer.h"

void SelectPlayer::Render()
{
	animation_set->at(select)->Render(x, y);
}

void SelectPlayer::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BBOX_SELECTPLAYER_WIDTH;
	b = y + BBOX_SELECTPLAYER_HEIGHT;
}

void SelectPlayer::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (select > 1)
	{
		select = select % 2;
	}
	if (select < 0)
	{
		select = -select;
	}
}
