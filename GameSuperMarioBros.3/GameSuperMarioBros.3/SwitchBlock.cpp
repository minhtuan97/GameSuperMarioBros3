#include "SwitchBlock.h"
#include "Brick.h"
#include "Coin.h"
#include "Grid.h"

SwitchBlock::SwitchBlock(float x, float y)
{
	this->x = x;
	this->y = y;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(ANIMATION_ID_SWITCHBLOCK);
	SetAnimationSet(ani_set);
}

void SwitchBlock::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + SWITCHBLOCK_WIDTH;
	bottom = y + SWITCHBLOCK_HEIGHT;
	if (isColi)
	{
		right = x + SWITCHBLOCK_WIDTH/2;
		bottom = y + SWITCHBLOCK_HEIGHT/2;
	}
}

void SwitchBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vector<LPGAMEOBJECT> listBrick;
	listBrick.clear();

	if (isColi&&!isSwitch)
	{
		//doi gach thanh vang
		for (int i = 0; i < coObjects->size(); i++)
		{
			if (dynamic_cast<CBrick*>(coObjects->at(i)))
			{
				CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
				if (brick->type == BRICK3)
				{
					listBrick.push_back(brick);
				}
			}
		}
		for (int i = 0; i < listBrick.size(); i++)
		{
			Coin* c = new Coin(listBrick.at(i)->x, listBrick.at(i)->y);
			Grid::GetInstance()->addObject(c);
		}
		for (int i = 0; i < listBrick.size(); i++)
		{
			Grid::GetInstance()->deleteObject(listBrick.at(i));
		}
	}
}

void SwitchBlock::Render()
{
	int ani = 0;
	if (isColi)
		ani = 1;
	animation_set->at(ani)->Render(x, y);
}
