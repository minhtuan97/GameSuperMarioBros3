#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Koopas.h"
#include "Portal.h"
#include "Brick.h"
#include "QuestionBrick.h"
#include "Box.h"
#include "WaterPipe.h"
#include "Item.h"
#include "Grid.h"
#include "Goomba.h"
#include "Coin.h"
#include "Plant.h"
#include "BallPlant.h"
#include "ItemSelect.h"
#include "ScenceSelect.h"
#include "SwitchBlock.h"
#include "PlayScence.h"
#include "BrickMove.h"
#include "BrokenBrick.h"
#include "Hammer.h"
#include "HammerBros.h"

#define TYPE_NAM	1
#define TYPE_LA	2

CMario* CMario::__instance = NULL;

CMario::CMario(float x, float y) : CGameObject()
{
	level = MARIO_LEVEL_SMALL;
	untouchable = 0;
	SetState(MARIO_STATE_IDLE);

	start_x = x; 
	start_y = y; 
	this->x = x; 
	this->y = y; 
	power = 0;
	listweapon.clear();
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	//if (scence == 0)
	//{
	//	CGameObject::Update(dt);
	//	x += dx;
	//	y += dy;

	//	return;
	//}
	if (scence == 1)
	{
		ScenceSelect* s = nullptr;
		for (int i = 0; i < coObjects->size(); i++)
		{
			if (AABB(coObjects->at(i)))
			{
				s = dynamic_cast<ScenceSelect*> (coObjects->at(i));
				this->canleft = s->canleft;
				this->canright = s->canright;
				this->candown = s->candown;
				this->canup = s->canup;
				this->scenceselect = s->id_scence;
				break;
			}
			else
			{
				this->scenceselect = -1;
			}
		}
		if (autoleft)
		{
			x -= 1;
			if (x_star-x >= MARIO_distanmove)
			{
				autoleft = false;
				x_star = x;
			}
		}
		if (autoright)
		{
			x += 1;
			if (x- x_star >= MARIO_distanmove)
			{
				autoright = false;
				x_star = x;
			}
		}
		if (autoup)
		{
			y -= 1;
			if (y_start - y >= MARIO_distanmove)
			{
				autoup = false;
				y_start = y;
			}
		}
		if (autodown)
		{
			y += 1;
			if (y - y_start >= MARIO_distanmove)
			{
				autodown = false;
				y_start = y;
			}
		}
		return;
	}
	// Calculate dx, dy 
	CGameObject::Update(dt);

	//check vi tri
	if (x < 0) x = 0;
	if (y < 0) y = 0;

	if (state == MARIO_STATE_PIPE)
	{
		y += dy;
		
		if (GetTickCount64() - switch_scence_start > MARIO_SWITCH_SCENCE_TIME && isSwitchtwoScence)
		{			
			SetState(MARIO_STATE_IDLE);
			isSwitchtwoScence = false;
			return;
		}
		if (GetTickCount64() - pipe_start > MARIO_PIPE_TIME && isSwitch)
		{
			//pipe_start = 0;
			float xnew = portal->xmario;
			float ynew = portal->ymario;
			int nynew = portal->nynewscence;
			CGame::GetInstance()->SwitchScene(portal->GetSceneId());

			SetPosition(xnew, ynew);
			if(nynew<0)
				if (GetLevel() == MARIO_LEVEL_SMALL)
					SetSpeed(0, -0.01);
				else
					SetSpeed(0, -0.02);
			isSwitch = false;
			return;
		}
		return;
	}
	// Simple fall down
	vy += MARIO_GRAVITY*dt;

	if (!ispressX&&state!=MARIO_STATE_DIE)
	{
		if (isJump && vy < -0.3f)
		{
			isBonusvy = false;
			vy = -0.28f;
		}
		if (iswalking)
		{//if (state == MARIO_STATE_WALKING_RIGHT)
			if (nx > 0)
			{
				vx -= 0.15 * MARIO_GRAVITY * dt;
				if (vx < 0)
					SetState(MARIO_STATE_IDLE);
			}
			//if (state == MARIO_STATE_WALKING_LEFT)
			if (nx < 0)
			{
				vx += 0.15 * MARIO_GRAVITY * dt;
				if (vx > 0)
					SetState(MARIO_STATE_IDLE);
			}
		}
	}

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}
	if (GetTickCount64() - skid_start > MARIO_SKID_TIME)
	{
		skid_start = 0;
		isSkid = false;
	}
	if (GetTickCount64() - kick_start > MARIO_KICK_TIME)
	{
		kick_start = 0;
		isKick = false;
	}
	if (GetTickCount64() - hold_start > MARIO_HOLD_TIME)
	{
		//DebugOut(L"het tg hold\n");
		//hold_start = 0;
		/*if (rua)
		{
			rua->ishold = false;
			rua->SetState(KOOPAS_STATE_KICK);
			rua == NULL;
			isHold = false;
			iscanHold = false;
			isColiWithKoopas = false;
		}*/
	}
	if (GetTickCount64() - throw_start > MARIO_THROW_TIME)
	{
		throw_start = 0;
		isthrow = false;
	}
	if (GetTickCount64() - wag_start > MARIO_WAG_TIME&&iswag)
	{
		wag_start = 0;
		if (iswag)
			if (nx > 0)
				SetPosition(x - 8, y);
			else if (nx < 0)
				SetPosition(x + 8, y);
		iswag = false;

	}
	if (GetTickCount64() - card_start >MARIO_SWITCH_FINISH_SCENCE_TIME&&iscard)
	{
		isfinishscence = true;
		card_start = NULL;
	}
	if (state == MARIO_STATE_DIE)
	{
		x += dx;
		y += dy;
		return;
	}
	vector<LPGAMEOBJECT> listPort;
	listPort.clear();
	for (int i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CPortal*>(coObjects->at(i)))
			listPort.push_back(coObjects->at(i));
	}
	for (int i = 0; i < listPort.size(); i++)
	{
		if (AABB(listPort.at(i)))
		{
			//if (!portal)
			//{
				portal = dynamic_cast<CPortal*>(listPort.at(i));
				if (portal->yCPortal < 0)
					isCanSwitchScenceDown = true;
				if (portal->yCPortal > 0)
					isCanSwitchScenceUP = true;
			//}
		}
		else
		{
			isCanSwitchScenceDown = false;
			isCanSwitchScenceUP = false;
			//delete portal;
			//portal = NULL;
		}
	}

	if (!ColitionWithObjectStatic(coObjects))
	{
		x += dx;
		y += dy;
	}
	ColitionWithItem(coObjects);
	ColitionWithEnemy(coObjects);
	for (int i = 0; i < listball.size(); i++)
	{
		listball.at(i)->Update(dt, coObjects);
	}
	vector<LPGAMEOBJECT> listb;

	for (int i = 0; i < listball.size(); i++)
	{
		Ball* b = dynamic_cast<Ball*>(listball.at(i));
		if (!b->isthrow)
			listb.push_back(b);
	}
	for (int i = 0; i < listb.size(); i++)
		listball.erase(std::remove(listball.begin(), listball.end(), listb.at(i)), listball.end());
}

void CMario::Render()
{
	if (this == NULL)
		return;
	int ani = -1;
	if (state == MARIO_STATE_DIE)
		ani = MARIO_ANIMATION_SMALL_DIE;
	else
	if (level == MARIO_LEVEL_BIG)
	{
		if (vx == 0)
		{
			if (nx>0) ani = MARIO_ANIMATION_SMALL_IDLE_RIGHT;
			else ani = MARIO_ANIMATION_SMALL_IDLE_LEFT;
		}
		else if (vx > 0) 
			ani = MARIO_ANI_BIG_WALKING_RIGHT; 
		else ani = MARIO_ANI_BIG_WALKING_LEFT;
	}
	if (level == MARIO_LEVEL_SMALL)
	{
		if (isJump)
		{
			if (isRun)
			{
				if (nx > 0)
					ani = MARIO_ANIMATION_SMALL_RUNJUMP_LEFT;
				else if (nx < 0)
					ani = MARIO_ANIMATION_SMALL_RUNJUMP_RIGHT;
			}
			else
			{
				if (nx > 0)
					ani = MARIO_ANIMATION_SMALL_JUMP_RIGHT;
				else if (nx < 0)
					ani = MARIO_ANIMATION_SMALL_JUMP_LEFT;
			}
		}
		else if (iswalking)
		{
			//if (nx > 0)
			//	ani = MARIO_ANIMATION_SMALL_WALKING_RIGHT;
			//else if (nx < 0)
			//	ani = MARIO_ANIMATION_SMALL_WALKING_LEFT;
			;
		}
		else if (vx == 0)
		{
			if (nx > 0) ani = MARIO_ANIMATION_SMALL_IDLE_RIGHT;
			else ani = MARIO_ANIMATION_SMALL_IDLE_LEFT;
		}
		if (state == MARIO_STATE_JUMP_LEFT)
			ani = MARIO_ANIMATION_SMALL_JUMP_LEFT;
		else if(state == MARIO_STATE_JUMP_RIGHT)
			ani = MARIO_ANIMATION_SMALL_JUMP_RIGHT;
		if (state == MARIO_STATE_WALKING_LEFT)
			ani = MARIO_ANIMATION_SMALL_WALKING_LEFT;
		else if (state == MARIO_STATE_WALKING_RIGHT)
			ani = MARIO_ANIMATION_SMALL_WALKING_RIGHT;
		if (state == MARIO_STATE_RUN_LEFT)
			ani = MARIO_ANIMATION_SMALL_RUN_LEFT;
		else if (state == MARIO_STATE_RUN_RIGHT)
			ani = MARIO_ANIMATION_SMALL_RUN_RIGHT;
		if (state == MARIO_STATE_RUNJUMP_LEFT)
			ani = MARIO_ANIMATION_SMALL_RUNJUMP_LEFT;
		else if (state == MARIO_STATE_RUNJUMP_RIGHT)
			ani = MARIO_ANIMATION_SMALL_RUNJUMP_RIGHT;
		if (state == MARIO_STATE_HOLD_LEFT)
			ani = MARIO_ANIMATION_SMALL_HOLD_LEFT;
		else if (state == MARIO_STATE_HOLD_RIGHT)
			ani = MARIO_ANIMATION_SMALL_HOLD_RIGHT;
		else if (state == MARIO_STATE_PIPE)
			ani = MARIO_ANIMATION_SMALL_PIPE;


		if (isSkid)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_SMALL_SKID_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_SMALL_SKID_RIGHT;
		}
		if (isKick)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_SMALL_KICK_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_SMALL_KICK_RIGHT;
		}
		if (isHold)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_SMALL_HOLD_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_SMALL_HOLD_RIGHT;
		}
	}
	if (level == MARIO_LEVEL_BIG)
	{
		if (isJump)
		{
			if (isRun)
			{
				if (nx > 0)
					ani = MARIO_ANIMATION_SUPER_RUNJUMP_LEFT;
				else if (nx < 0)
					ani = MARIO_ANIMATION_SUPER_RUNJUMP_RIGHT;
			}
			else
			{
				if (nx > 0)
					ani = MARIO_ANIMATION_SUPER_JUMP_RIGHT;
				else if (nx < 0)
					ani = MARIO_ANIMATION_SUPER_JUMP_LEFT;
			}
		}
		else if (iswalking)
		{
			//if (nx > 0)
			//	ani = MARIO_ANIMATION_SUPER_WALKING_RIGHT;
			//else if (nx < 0)
			//	ani = MARIO_ANIMATION_SUPER_WALKING_LEFT;
			;
		}
		else if (vx == 0)
		{
			if (nx > 0) ani = MARIO_ANIMATION_SUPER_IDLE_RIGHT;
			else ani = MARIO_ANIMATION_SUPER_IDLE_LEFT;
		}
		if (state == MARIO_STATE_JUMP_LEFT)
			ani = MARIO_ANIMATION_SUPER_JUMP_LEFT;
		else if (state == MARIO_STATE_JUMP_RIGHT)
			ani = MARIO_ANIMATION_SUPER_JUMP_RIGHT;
		if (state == MARIO_STATE_WALKING_LEFT)
			ani = MARIO_ANIMATION_SUPER_WALKING_LEFT;
		else if (state == MARIO_STATE_WALKING_RIGHT)
			ani = MARIO_ANIMATION_SUPER_WALKING_RIGHT;
		if (state == MARIO_STATE_RUN_LEFT)
			ani = MARIO_ANIMATION_SUPER_RUN_LEFT;
		else if (state == MARIO_STATE_RUN_RIGHT)
			ani = MARIO_ANIMATION_SUPER_RUN_RIGHT;
		if (state == MARIO_STATE_RUNJUMP_LEFT)
			ani = MARIO_ANIMATION_SUPER_RUNJUMP_LEFT;
		else if (state == MARIO_STATE_RUNJUMP_RIGHT)
			ani = MARIO_ANIMATION_SUPER_RUNJUMP_RIGHT;
		if (state == MARIO_STATE_HOLD_LEFT)
			ani = MARIO_ANIMATION_SUPER_HOLD_LEFT;
		else if (state == MARIO_STATE_HOLD_RIGHT)
			ani = MARIO_ANIMATION_SUPER_HOLD_RIGHT;
		else if (state == MARIO_STATE_PIPE)
			ani = MARIO_ANIMATION_SUPER_PIPE;

		if (isSkid)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_SUPER_SKID_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_SUPER_SKID_RIGHT;
		}
		if (isKick)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_SUPER_KICK_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_SUPER_KICK_RIGHT;
		}
		if (isHold)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_SUPER_HOLD_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_SUPER_HOLD_RIGHT;
		}
	}
	if (level == MARIO_LEVEL_FIRE)
	{
		if (isJump)
		{
			if (isRun)
			{
				if (nx > 0)
					ani = MARIO_ANIMATION_FIRE_RUNJUMP_LEFT;
				else if (nx < 0)
					ani = MARIO_ANIMATION_FIRE_RUNJUMP_RIGHT;
			}
			else
			{
				if (nx > 0)
					ani = MARIO_ANIMATION_FIRE_JUMP_RIGHT;
				else if (nx < 0)
					ani = MARIO_ANIMATION_FIRE_JUMP_LEFT;
			}
		}
		else if (iswalking)
		{
			//if (nx > 0)
			//	ani = MARIO_ANIMATION_FIRE_WALKING_RIGHT;
			//else if (nx < 0)
			//	ani = MARIO_ANIMATION_FIRE_WALKING_LEFT;
			;
		}
		else if (vx == 0)
		{
			if (nx > 0) ani = MARIO_ANIMATION_FIRE_IDLE_RIGHT;
			else ani = MARIO_ANIMATION_FIRE_IDLE_LEFT;
		}
		if (state == MARIO_STATE_JUMP_LEFT)
			ani = MARIO_ANIMATION_FIRE_JUMP_LEFT;
		else if (state == MARIO_STATE_JUMP_RIGHT)
			ani = MARIO_ANIMATION_FIRE_JUMP_RIGHT;
		if (state == MARIO_STATE_WALKING_LEFT)
			ani = MARIO_ANIMATION_FIRE_WALKING_LEFT;
		else if (state == MARIO_STATE_WALKING_RIGHT)
			ani = MARIO_ANIMATION_FIRE_WALKING_RIGHT;
		if (state == MARIO_STATE_RUN_LEFT)
			ani = MARIO_ANIMATION_FIRE_RUN_LEFT;
		else if (state == MARIO_STATE_RUN_RIGHT)
			ani = MARIO_ANIMATION_FIRE_RUN_RIGHT;
		if (state == MARIO_STATE_RUNJUMP_LEFT)
			ani = MARIO_ANIMATION_FIRE_RUNJUMP_LEFT;
		else if (state == MARIO_STATE_RUNJUMP_RIGHT)
			ani = MARIO_ANIMATION_FIRE_RUNJUMP_RIGHT;
		if (state == MARIO_STATE_HOLD_LEFT)
			ani = MARIO_ANIMATION_FIRE_HOLD_LEFT;
		else if (state == MARIO_STATE_HOLD_RIGHT)
			ani = MARIO_ANIMATION_FIRE_HOLD_RIGHT;
		else if (state == MARIO_STATE_PIPE)
			ani = MARIO_ANIMATION_FIRE_PIPE;


		if (isSkid)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_FIRE_SKID_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_FIRE_SKID_RIGHT;
		}
		if (isKick)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_FIRE_KICK_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_FIRE_KICK_RIGHT;
		}
		if (isHold)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_FIRE_HOLD_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_FIRE_HOLD_RIGHT;
		}
		if (isthrow)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_FIRE_THROW_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_FIRE_THROW_RIGHT;
		}
	}
	if (level == MARIO_LEVEL_RACCOON)
	{
		if (isJump)
		{
			if (isRun)
			{
				if (nx > 0)
					ani = MARIO_ANIMATION_REACCOON_RUNJUMP_LEFT;
				else if (nx < 0)
					ani = MARIO_ANIMATION_REACCOON_RUNJUMP_RIGHT;
			}
			else
			{
				if (isBonusvy)
				{
					if (nx > 0)
						ani = MARIO_ANIMATION_REACCOON_JUMP_RIGHT;
					else if (nx < 0)
						ani = MARIO_ANIMATION_REACCOON_JUMP_LEFT;
				}
				else
				{
					if (nx > 0) 
						ani = MARIO_ANIMATION_REACCOON_IDLE_RIGHT;
					else if(nx<0)
						ani = MARIO_ANIMATION_REACCOON_IDLE_LEFT;
				}
			}
		}
		else if (iswalking)
		{
			//if (nx > 0)
			//	ani = MARIO_ANIMATION_REACCOON_WALKING_RIGHT;
			//else if (nx < 0)
			//	ani = MARIO_ANIMATION_REACCOON_WALKING_LEFT;
			;
		}
		else if (vx == 0)
		{
			if (nx > 0) ani = MARIO_ANIMATION_REACCOON_IDLE_RIGHT;
			else ani = MARIO_ANIMATION_REACCOON_IDLE_LEFT;
		}
		if (state == MARIO_STATE_JUMP_LEFT)
			ani = MARIO_ANIMATION_REACCOON_JUMP_LEFT;
		else if (state == MARIO_STATE_JUMP_RIGHT)
			ani = MARIO_ANIMATION_REACCOON_JUMP_RIGHT;
		if (state == MARIO_STATE_WALKING_LEFT)
			ani = MARIO_ANIMATION_REACCOON_WALKING_LEFT;
		else if (state == MARIO_STATE_WALKING_RIGHT)
			ani = MARIO_ANIMATION_REACCOON_WALKING_RIGHT;
		if (state == MARIO_STATE_RUN_LEFT)
			ani = MARIO_ANIMATION_REACCOON_RUN_LEFT;
		else if (state == MARIO_STATE_RUN_RIGHT)
			ani = MARIO_ANIMATION_REACCOON_RUN_RIGHT;
		if (state == MARIO_STATE_RUNJUMP_LEFT)
			ani = MARIO_ANIMATION_REACCOON_RUNJUMP_LEFT;
		else if (state == MARIO_STATE_RUNJUMP_RIGHT)
			ani = MARIO_ANIMATION_REACCOON_RUNJUMP_RIGHT;
		if (state == MARIO_STATE_HOLD_LEFT)
			ani = MARIO_ANIMATION_REACCOON_HOLD_LEFT;
		else if (state == MARIO_STATE_HOLD_RIGHT)
			ani = MARIO_ANIMATION_REACCOON_HOLD_RIGHT;
		else if (state == MARIO_STATE_PIPE)
			ani = MARIO_ANIMATION_REACCOON_PIPE;

		if (isSkid)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_REACCOON_SKID_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_REACCOON_SKID_RIGHT;
		}
		if (isKick)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_REACCOON_KICK_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_REACCOON_KICK_RIGHT;
		}
		if (isHold)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_REACCOON_HOLD_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_REACCOON_HOLD_RIGHT;
		}
		if (iswag)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_REACCOON_WAGTAIL_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_REACCOON_WAGTAIL_RIGHT;
		}
		if (isfly)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_REACCOON_WAG_LEFT;
			else if (nx > 0)
				ani = MARIO_ANIMATION_REACCOON_WAG_RIGHT;
		}
	}
	int alpha = 255;
	if (untouchable) alpha = 128;
	if (scence == 1)
	{
		ani = 136;
	}
	animation_set->at(ani)->Render(x, y, alpha);
	RenderBoundingBox();
}

void CMario::SetState(int state)
{
	//if (this->state == state)
	//	return;

	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		//vx = MARIO_WALKING_SPEED;
		if (scence == 0)
		{	
			vx = 0.05f;
			vy = 0;
			nx = 1;
			if (!isJump)
				CGameObject::SetState(state);
			iswalking = true;
		}
		else
		{
			if (vx < 0.1f)
				vx += 0.005f;
			nx = 1;
			if (!isJump)
				CGameObject::SetState(state);
			iswalking = true;
		}
		
		break;
	case MARIO_STATE_WALKING_LEFT: 
		//vx = -MARIO_WALKING_SPEED;
		if (scence == 0)
		{
				vx = -0.05f;
				vy = 0;
			nx = -1;
			if (!isJump)
				CGameObject::SetState(state);
			iswalking = true;
		}
		else
		{
			if (vx > -0.1f)
				vx -= 0.005f;
			nx = -1;
			if (!isJump)
				CGameObject::SetState(state);
			iswalking = true;
		}
		break;
	case MARIO_STATE_RUN_LEFT:
		//vx = -MARIO_WALKING_SPEED;
		if (vx > -0.2f)
			vx -= 0.01f;
		nx = -1;
		if (!isJump)
			CGameObject::SetState(state);
		iswalking = true;
		break;
	case MARIO_STATE_RUN_RIGHT:
		//vx = MARIO_WALKING_SPEED;
		if (vx < 0.2f)
			vx += 0.01f;
		nx = 1;
		if (!isJump)
			CGameObject::SetState(state);
		iswalking = true;
		break;
	case MARIO_STATE_JUMP:
		// TODO: need to check if Mario is *current* on a platform before allowing to jump again
		vy = -MARIO_JUMP_SPEED_Y;
		break;
	case MARIO_STATE_JUMP_LEFT:
	case MARIO_STATE_JUMP_RIGHT:
			CGameObject::SetState(state);
		isJump = true;		
		if (ispressX == true)
			vy = -0.23f;

		else
		{
			if (isBonusvy)
			{
				vy -= MARIO_JUMP_SPEED_Y;
			}

			
		}
		iswalking = false;
		break;
	case MARIO_STATE_RUNJUMP_LEFT:
	case MARIO_STATE_RUNJUMP_RIGHT:
		CGameObject::SetState(state);
		isJump = true;
		if (ispressX == true)
			vy = -0.23f;
		else
			if (isBonusvy)
			{
				vy -= MARIO_JUMP_SPEED_Y;
			}
		iswalking = false;
		break;
	case MARIO_STATE_IDLE: 
			CGameObject::SetState(state);
		vx = 0;
		isJump = false;
		isjumpX = false;
		iswalking = false;
		ispressX = false;
		isHold = false;
		//isColiWithKoopas = false;
		break;
	case MARIO_STATE_DIE:
			CGameObject::SetState(state);
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_STATE_HOLD_LEFT:
		CGameObject::SetState(state);
		if (vx > -0.1f)
			vx -= 0.005f;
		nx = -1;
		iswalking = true;
		break;

	case MARIO_STATE_HOLD_RIGHT:
		CGameObject::SetState(state);
		if (vx < 0.1f)
			vx += 0.005f;
		nx = 1;
		iswalking = true;
		break;
	case MARIO_STATE_PIPE:
		CGameObject::SetState(state);
		vx = vy = 0;
		pipe_start = GetTickCount64();
		isSwitch = true;
		switch_scence_start = GetTickCount64();
		isSwitchtwoScence = true;
		break;
	}
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y; 

	if (level==MARIO_LEVEL_BIG|| level == MARIO_LEVEL_FIRE)
	{
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_RACCOON)
	{
		right = x + MARIO_RACCOON_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
		if (iswag)
		{
			left = x;
			top = y + MARIO_BIG_BBOX_HEIGHT / 2;
			right = x + MARIO_RACCOON_BBOX_WIDTH;
			bottom = y + MARIO_BIG_BBOX_HEIGHT;
		}
	}
	else
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
	
}

void CMario::DecreasePower()
{
	if (power > 0)
		power -= 0.5f;
	else
		power = 0.0f;
	if (power < 70)
		isRun = false;
}

void CMario::IncreasePower()
{
	if (power < 90)
		power += 1.0f;
	if (power >= 70)
		isRun = true;
}

bool CMario::ColitionWithObjectStatic(vector<LPGAMEOBJECT>* listObject)
{

	vector<LPGAMEOBJECT> listbox;
	vector<LPGAMEOBJECT> listbrick;
	vector<LPGAMEOBJECT> listquestionbrick;
	vector<LPGAMEOBJECT> listwaterpipe;
	vector<LPGAMEOBJECT> listItemSelect;
	vector<LPGAMEOBJECT> listP;
	vector<LPGAMEOBJECT> listbrickmove;

	listbox.clear();
	listbrick.clear();
	listquestionbrick.clear();
	listItemSelect.clear();
	listP.clear();
	listbrickmove.clear();

	for (int i = 0; i < listObject->size(); i++)
	{
		if (dynamic_cast<Box*>(listObject->at(i)))
			listbox.push_back(listObject->at(i));
		if (dynamic_cast<CBrick*>(listObject->at(i)))
			listbrick.push_back(listObject->at(i));
		if (dynamic_cast<QuestionBrick*>(listObject->at(i)))
			listquestionbrick.push_back(listObject->at(i));
		if (dynamic_cast<WaterPipe*>(listObject->at(i)))
			listwaterpipe.push_back(listObject->at(i));
		if (dynamic_cast<ItemSelect*>(listObject->at(i)))
			listItemSelect.push_back(listObject->at(i));
		if (dynamic_cast<SwitchBlock*>(listObject->at(i)))
			listP.push_back(listObject->at(i));
		if (dynamic_cast<BrickMove*>(listObject->at(i)))
			listbrickmove.push_back(listObject->at(i));
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != MARIO_STATE_DIE)
	{
		if (!(isJump && vy < 0))
			CalcPotentialCollisions(&listbox, coEvents);
		CalcPotentialCollisions(&listbrick, coEvents);
		CalcPotentialCollisions(&listquestionbrick, coEvents);
		CalcPotentialCollisions(&listwaterpipe, coEvents);
		CalcPotentialCollisions(&listItemSelect, coEvents);
		CalcPotentialCollisions(&listP, coEvents);
		CalcPotentialCollisions(&listbrickmove, coEvents);
	}

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		return false;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx); 

		// block every object first!
		x += min_tx * dx + nx * 0.2f;
		y += min_ty * dy + ny * 0.2f;

		if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;


		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			
			if (e->ny < 0)
			{
				if (ispressX)
				{
					if (this->nx > 0)
						SetState(MARIO_STATE_JUMP_RIGHT);
					else
						SetState(MARIO_STATE_JUMP_LEFT);
				}
				else
				{
					isjumpX = false;
					if (vx == 0)
						SetState(MARIO_STATE_IDLE);
				}

				if (isUpS)
				{
					iscanjumpS = true;
					isBonusvy = true;
				}
				else
				{
					iscanjumpS = false;
					isBonusvy = false;
				}
				if (!isjumpX)
				{
					if (vx > 0)
					{
						isJump = false;
						iswalking = true;
						//state = MARIO_STATE_WALKING_RIGHT;
						//DebugOut(L"Vao ham vx>0, state=%d\n", state);
					}
					else if (vx < 0)
					{
						isJump = false;
						iswalking = true;
						//state = MARIO_STATE_WALKING_LEFT;
						//DebugOut(L"Vao ham vx<0, state=%d\n", state);
					}
				}
			}
			//if brickmove
			if (dynamic_cast<BrickMove*>(e->obj))
			{
				if (e->ny < 0)
				{
					BrickMove* b = dynamic_cast<BrickMove*>(e->obj);
					b->isColi = true;
				}
			}
			//if ItemSelect
			if (dynamic_cast<ItemSelect*>(e->obj))
			{
				ItemSelect* p = dynamic_cast<ItemSelect*>(e->obj);
				p->isColi = true;
				listcard.push_back(p);
				card_start = GetTickCount64();
			}
			if (dynamic_cast<SwitchBlock*>(e->obj))
			{
				SwitchBlock* p = dynamic_cast<SwitchBlock*>(e->obj);
				if (!p->isColi)
				{
					p->isColi = true;
					p->SetPosition(p->x, p->y + 8);
					power = 100;
				}

			}
			if (dynamic_cast<CBrick*>(e->obj))//if brick
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);
				 if (e->ny > 0)
				{
					if (brick->type == BRICK5 &&!brick->isP)
					{
						brick->isP = true;
						brick->SetSpeed(0, -0.15);

					}
					if (brick->type == BRICK3)
					{
						Grid::GetInstance()->deleteObject(brick);
						float l, t, r, b;
						brick->GetBoundingBox(l, t, r, b);
						float xnew = l + (r - l) / 2;
						float ynew = t + (b - t) / 2;
						BrokenBrick* broken1 = new BrokenBrick(xnew, ynew);
						broken1->SetSpeed(-SPEED_BRICK_X, -SPEED_BRICK_YY);
						BrokenBrick* broken2 = new BrokenBrick(xnew, ynew);
						broken2->SetSpeed(SPEED_BRICK_X, -SPEED_BRICK_YY);
						BrokenBrick* broken3 = new BrokenBrick(xnew, ynew);
						broken3->SetSpeed(-SPEED_BRICK_X, -SPEED_BRICK_Y);
						BrokenBrick* broken4 = new BrokenBrick(xnew, ynew);
						broken4->SetSpeed(SPEED_BRICK_X, -SPEED_BRICK_Y);
						Grid::GetInstance()->addObject(broken1);
						Grid::GetInstance()->addObject(broken2);
						Grid::GetInstance()->addObject(broken3);
						Grid::GetInstance()->addObject(broken4);
					}
				}
			}
			if (dynamic_cast<QuestionBrick*>(e->obj))//if question brick
			{
				QuestionBrick* brick = dynamic_cast<QuestionBrick*>(e->obj);
				if (e->ny > 0)
				{
					if (!brick->isColi)
					{
						brick->isColi = true;
						money++;
						brick->SetSpeed(0, -0.15);
					}
					isBonusvy = false;
				}
			}
			if (dynamic_cast<Box*>(e->obj))//if Box
			{
				Box* brick = dynamic_cast<Box*>(e->obj);

				if (e->nx != 0)
				{
					x += dx;
					y += dy;
				}
			}
		}
	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	for (int i = 0; i < listbrick.size(); i++)
	{
		CBrick* brick = dynamic_cast<CBrick*>(listbrick.at(i));
		if (brick->type == BRICK3&&iswag)
			if (AABB(brick))
			{
				Grid::GetInstance()->deleteObject(brick);
				float l, t, r, b;
				brick->GetBoundingBox(l, t, r, b);
				float xnew = l + (r - l) / 2;
				float ynew = t + (b - t) / 2;
				BrokenBrick* broken1 = new BrokenBrick(xnew, ynew);
				broken1->SetSpeed(-SPEED_BRICK_X,-SPEED_BRICK_YY);
				BrokenBrick* broken2 = new BrokenBrick(xnew, ynew);
				broken2->SetSpeed(SPEED_BRICK_X, -SPEED_BRICK_YY);
				BrokenBrick* broken3 = new BrokenBrick(xnew, ynew);
				broken3->SetSpeed(-SPEED_BRICK_X, -SPEED_BRICK_Y);
				BrokenBrick* broken4 = new BrokenBrick(xnew, ynew);
				broken4->SetSpeed(SPEED_BRICK_X, -SPEED_BRICK_Y);
				Grid::GetInstance()->addObject(broken1);
				Grid::GetInstance()->addObject(broken2);
				Grid::GetInstance()->addObject(broken3);
				Grid::GetInstance()->addObject(broken4);
			}
	}

		for (int i = 0; i < listquestionbrick.size(); i++)
		{
			QuestionBrick* brick = dynamic_cast<QuestionBrick*>(listquestionbrick.at(i));
			if (iswag)
				if (AABB(brick))
				{
					if (!brick->isColi)
					{
						brick->isColi = true;
						money++;
						brick->SetSpeed(0, -0.15);
					}
				}
		}
	return true;
}

void CMario::ColitionWithItem(vector<LPGAMEOBJECT>* listObject)
{
	vector<LPGAMEOBJECT> listitem;
	listitem.clear();

	for (int i = 0; i < listObject->size(); i++)
	{
		if (dynamic_cast<Item*>(listObject->at(i)))
		{
			Item* item = dynamic_cast<Item*>(listObject->at(i));
			if(item->type== TYPE_NAM|| item->type == TYPE_LA)
				listitem.push_back(listObject->at(i));
		}
	}
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;
	coEvents.clear();
	CalcPotentialCollisions(&listitem, coEvents);
	if (coEvents.size() != 0)
	{
		if (level == MARIO_LEVEL_SMALL)
		{
			SetPosition(this->x, this->y - 16);
			SetLevel(MARIO_LEVEL_BIG);
		}
		else if (level == MARIO_LEVEL_BIG)
		{
			SetLevel(MARIO_LEVEL_RACCOON);

		}

		for(int i=0;i<listitem.size();i++)
			Grid::GetInstance()->deleteObject(listitem.at(i));
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	vector<LPGAMEOBJECT> listcoin;
	listcoin.clear();

	//va cham voi coin
	for (int i = 0; i < listObject->size(); i++)
	{
		if (dynamic_cast<Coin*>(listObject->at(i)))
		{
			Coin* item = dynamic_cast<Coin*>(listObject->at(i));
			listcoin.push_back(listObject->at(i));
		}
	}
	
	vector<LPCOLLISIONEVENT> coEvents2;
	vector<LPCOLLISIONEVENT> coEventsResult2;
	coEvents2.clear();
	CalcPotentialCollisions(&listcoin, coEvents2);
	if (coEvents2.size() != 0)
	{
		DebugOut(L"size%d\n", coEvents2.size());

		for (UINT i = 0; i < coEvents2.size(); i++)
		{
			LPCOLLISIONEVENT e = coEvents2[i];

			if (dynamic_cast<Coin*>(e->obj))
			{
				this->money++;
				Coin* coin = dynamic_cast<Coin*>(e->obj);
				coin->iscoli = true;
			}
		}
	}
	//for (int i = 0; i < listcoin.size(); i++)
	//{
	//	Coin* coin = dynamic_cast<Coin*>(listcoin.at(i));
	//	if (coin->iscoli)
	//		Grid::GetInstance()->deleteObject(coin);
	//}
	for (UINT i = 0; i < coEvents2.size(); i++) delete coEvents2[i];

}

void CMario::ColitionWithEnemy(vector<LPGAMEOBJECT>* listObject)
{
	//isColiWithKoopas = false;

	vector<LPGAMEOBJECT> listenemy;
	vector<LPGAMEOBJECT> listkoopas;
	vector<LPGAMEOBJECT> listgoomba;
	vector<LPGAMEOBJECT> listPlant;
	vector<LPGAMEOBJECT> listPlantball;
	vector<LPGAMEOBJECT> listHammerBros;
	vector<LPGAMEOBJECT> listHammer;
	listenemy.clear();
	listkoopas.clear();
	listgoomba.clear();
	listPlant.clear();
	listPlantball.clear();
	listHammerBros.clear();
	listHammer.clear();

	for (int i = 0; i < listObject->size(); i++)
	{
		if (dynamic_cast<CKoopas*>(listObject->at(i)))
		{
			CKoopas* k = dynamic_cast<CKoopas*>(listObject->at(i));
			if (!k->ishold)
			{
				listkoopas.push_back(listObject->at(i));
				listenemy.push_back(listObject->at(i));
			}

		}
		if (dynamic_cast<Hammer*>(listObject->at(i)))
		{
				listHammer.push_back(listObject->at(i));
				listenemy.push_back(listObject->at(i));

		}
		if (dynamic_cast<CGoomba*>(listObject->at(i)))
		{
			CGoomba* k = dynamic_cast<CGoomba*>(listObject->at(i));
			listenemy.push_back(listObject->at(i));
			listgoomba.push_back(listObject->at(i));
		}
		if (dynamic_cast<Plant*>(listObject->at(i)))
		{
			Plant* k = dynamic_cast<Plant*>(listObject->at(i));
			listenemy.push_back(listObject->at(i));
			listPlant.push_back(listObject->at(i));
		}
		if (dynamic_cast<BallPlant*>(listObject->at(i)))
		{
			BallPlant* k = dynamic_cast<BallPlant*>(listObject->at(i));
			listenemy.push_back(listObject->at(i));
			listPlantball.push_back(listObject->at(i));
		}
		if (dynamic_cast<HammerBros*>(listObject->at(i)))
		{
			HammerBros* k = dynamic_cast<HammerBros*>(listObject->at(i));
			listenemy.push_back(listObject->at(i));
			listHammerBros.push_back(listObject->at(i));
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	
	// turn off collision when die 
	if (state != MARIO_STATE_DIE&&!untouchable)
	{
		CalcPotentialCollisions(&listkoopas, coEvents);
		CalcPotentialCollisions(&listgoomba, coEvents);
		//CalcPotentialCollisions(&listPlant, coEvents);
		CalcPotentialCollisions(&listPlantball, coEvents);
		CalcPotentialCollisions(&listHammerBros, coEvents);
		CalcPotentialCollisions(&listHammer, coEvents);
	}

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		//isColiWithKoopas = false;
		//check AABB va cham luc dung yen
		/*for (int i = 0; i < listenemy.size(); i++)
		{
			if (AABB(listenemy.at(i)))
			{
				if (untouchable == 0)
				{
					if (level > MARIO_LEVEL_SMALL)
					{
						level = MARIO_LEVEL_SMALL;
						StartUntouchable();
					}
					else
						SetState(MARIO_STATE_DIE);
				}
				return;
			}
		}*/
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		 //TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);
		
		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.3f;
		
		if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		//coEventsResult = coEvents;
		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			
			if (dynamic_cast<CKoopas*>(e->obj))
			{
				if (state == MARIO_STATE_HOLD_LEFT || state == MARIO_STATE_HOLD_RIGHT)
					continue;
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				{
					if (e->ny < 0)
					{
						if (koopas->GetState() == KOOPAS_STATE_WALKING)
						{
							koopas->SetState(KOOPAS_STATE_DIE);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
						else  if (koopas->GetState() == KOOPAS_STATE_DIE)
						{
							koopas->SetState(KOOPAS_STATE_KICK);
							StartKick();							
						}
						else if (koopas->GetState() == KOOPAS_STATE_FLY)
						{
							koopas->SetState(KOOPAS_STATE_WALKING);
							vy = -MARIO_JUMP_DEFLECT_SPEED;
						}
						enymy += 100;
					}
					else if (e->ny > 0)
					{
						if (untouchable == 0)
						{
							if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}
					}
					else if (e->nx != 0)
					{
						if (level == MARIO_LEVEL_RACCOON)
						{
							if (iswag)
							{
								if (this->nx * e->nx < 0)
								{
									koopas->SetState(KOOPAS_STATE_DIE);
									koopas->SetSpeed(this->nx * 0.1, -0.2);
								}
							}
						}
						if (koopas->GetState() == KOOPAS_STATE_DIE)
						{
							if (iscanHold&&!iswag)
							{
								isColiWithKoopas = true;
							}
							else
							{
								koopas->SetState(KOOPAS_STATE_KICK);
								StartKick();
							}
							rua = koopas;
						}
						else if (koopas->GetState() == KOOPAS_STATE_WALKING|| koopas->GetState() == KOOPAS_STATE_FLY)
						{
							if (untouchable == 0)
							{
							if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
							}
						}

					}
				}
			}
			if (dynamic_cast<HammerBros*>(e->obj))
			{

				HammerBros* h = dynamic_cast<HammerBros*>(e->obj);
				{
					if (e->ny < 0)
					{
						if (h->GetState() != HammerBros_STATE_DIE)
						{
							h->SetState(HammerBros_STATE_DIE);
							//h->iskilltop = true;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							enymy += 100;
						}
					}
					else if (e->ny > 0)
					{
						if (untouchable == 0)
						{
							if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}
					}
					if (e->nx != 0)
					{
						if (level == MARIO_LEVEL_RACCOON)
						{
							if (iswag)
							{
								/*if (this->nx * e->nx < 0)
								{
									koopas->SetState(KOOPAS_STATE_DIE);
									koopas->SetSpeed(this->nx * 0.1, -0.2);
								}*/
							}
							else
							{
								if (untouchable == 0)
								{
									if (level > MARIO_LEVEL_SMALL)
									{
										level = MARIO_LEVEL_SMALL;
										StartUntouchable();
									}
									else
										SetState(MARIO_STATE_DIE);
								}
							}
						}
						else
						{
							if (untouchable == 0)
							{
								if (level > MARIO_LEVEL_SMALL)
								{
									level = MARIO_LEVEL_SMALL;
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
						}

					}
				}
			}
			if (dynamic_cast<Hammer*>(e->obj))
			{

				Hammer* h = dynamic_cast<Hammer*>(e->obj);
				if(e->ny!=0||e->nx!=0)
					if (untouchable == 0)
					{
						if (level > MARIO_LEVEL_SMALL)
						{
							level = MARIO_LEVEL_SMALL;
							StartUntouchable();
						}
						else
							SetState(MARIO_STATE_DIE);
					}
			}
			if (dynamic_cast<CGoomba*>(e->obj))
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					enymy += 100;
					if (goomba->type == 1)
					{
						//goomba->type == 0;
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							if (goomba->GetState() == GOOMBA_STATE_FLY|| goomba->GetState() == GOOMBA_STATE_WALKING)
								goomba->SetState(GOOMBA_STATE_WALKING2);
							else if (goomba->GetState() == GOOMBA_STATE_WALKING2)
							{
								goomba->SetState(GOOMBA_STATE_DIE);
								goomba->iskilltop = true;
								vy = -MARIO_JUMP_DEFLECT_SPEED;
							}

						}
					}
					else {
						if (goomba->type == 0)
							if (goomba->GetState() != GOOMBA_STATE_DIE)
							{
								goomba->SetState(GOOMBA_STATE_DIE);
								goomba->iskilltop = true;
								vy = -MARIO_JUMP_DEFLECT_SPEED;
							}
					}

				}
				else if (e->nx != 0)
				{
					if (level == MARIO_LEVEL_RACCOON)
					{
						if (iswag)
						{
							if (this->nx * e->nx < 0)
							{
								goomba->SetState(GOOMBA_STATE_DIE);
								goomba->SetSpeed(this->nx * 0.1, -0.2);
								vy -= 0.01;
							}
						}
						else
						{
							if (goomba->GetState() != GOOMBA_STATE_DIE)
							{
								if (level > MARIO_LEVEL_SMALL)
								{
									level = MARIO_LEVEL_SMALL;
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
						}

					}
					else if (untouchable == 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}
					}
				}
			}
			if (dynamic_cast<Plant*>(e->obj))
			{
				Plant* plant = dynamic_cast<Plant*>(e->obj);
				{
					//DebugOut(L"e.nx=%d\n", e->nx);
					//DebugOut(L"e.ny=%d\n", e->ny);
					if (e->nx != 0)
					{
						if (level == MARIO_LEVEL_RACCOON && iswag)
						{
							{
								DebugOut(L"va cham quay duoi voi plant\n");
								plant->isColi = true;
							}
							enymy += 100;
						}
						else
						{
							if (untouchable == 0)
							{
								if (level > MARIO_LEVEL_SMALL)
								{
									level = MARIO_LEVEL_SMALL;
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
						}
					}
					if (e->ny != 0)
					{
						if (untouchable == 0)
						{
							if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}
					}
				}
			}
			if (dynamic_cast<BallPlant*>(e->obj))
			{
				BallPlant* b = dynamic_cast<BallPlant*>(e->obj);

				if (untouchable == 0)
				{
					if (level > MARIO_LEVEL_SMALL)
					{
						level = MARIO_LEVEL_SMALL;
						StartUntouchable();
					}
					else
						SetState(MARIO_STATE_DIE);
				}
				b->isattack = true;
			}
		}
	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) 
		delete coEvents[i];
	
	//xet va chma voi plant
	for (int i = 0; i < listPlant.size(); i++)
	{
		if (AABB(listPlant.at(i)))
		{
				if (level == MARIO_LEVEL_RACCOON && iswag)
				{
					{
						dynamic_cast<Plant*>(listPlant.at(i))->isColi = true;
					}
					enymy += 100;
				}
				else
				{
					if (untouchable == 0)
					{
						if (level > MARIO_LEVEL_SMALL)
						{
							level = MARIO_LEVEL_SMALL;
							StartUntouchable();
						}
						else
							SetState(MARIO_STATE_DIE);
					}
				}
		}
	}
	//xet va cham quay duoi voi hammerbros
	for (int i = 0; i < listHammerBros.size(); i++)
	{
		if (AABB(listHammerBros.at(i)))
		{
			if (level == MARIO_LEVEL_RACCOON && iswag)
			{
				HammerBros* h = dynamic_cast<HammerBros*>(listHammerBros.at(i));

				if (h->GetState() != HammerBros_STATE_DIE)
				{
					h->SetState(HammerBros_STATE_DIE);
					//h->iskilltop = true;
					vy = -MARIO_JUMP_DEFLECT_SPEED;
					enymy += 100;
				}
				enymy += 100;
			}
		}
	}
}

CMario* CMario::GetInstance()
{
	if (__instance == NULL) __instance = new CMario();
	return __instance;
}

bool CMario::AABB(LPGAMEOBJECT objects)
{
	float left, top, right, bot;
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	objects->GetBoundingBox(left, top, right, bot);
	if (l < right &&
		r > left &&
		t < bot &&
		b > top) {
		return true;
	}
	return false;
}

void CMario::Unload()
{
	__instance = NULL;
}

//bool CMario::AABB(vector<LPGAMEOBJECT>* colliable_objects)
//{
//
//	return false;
//}

/*
	Reset Mario status to the beginning state of a scene
*/
void CMario::Reset()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_SMALL);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
}

