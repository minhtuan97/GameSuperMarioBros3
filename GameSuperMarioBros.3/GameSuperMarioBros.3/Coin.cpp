#include "Coin.h"
#define ANIMATIONSET_ID_COIN 26
Coin::Coin(float x, float y)
{
	this->x = x;
	this->y = y;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(ANIMATIONSET_ID_COIN);
	SetAnimationSet(ani_set);
}

void Coin::Render()
{
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox();
}

void Coin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x+COIN_BBOX_WIDTH;
	b = y+COIN_BBOX_HEIGHT;
}
