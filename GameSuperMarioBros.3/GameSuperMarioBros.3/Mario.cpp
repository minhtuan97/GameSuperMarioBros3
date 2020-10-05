#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Portal.h"
#include "Brick.h"

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
}

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy += MARIO_GRAVITY*dt;

	if (!ispressX)
	{
		if (isJump && vy < -0.27f)
		{
			isBonusvy = false;
			vy = -0.27f;
		}
		if (iswalking)
		{//if (state == MARIO_STATE_WALKING_RIGHT)
			if (nx > 0)
			{
				vx -= 0.2 * MARIO_GRAVITY * dt;
				if (vx < 0)
					SetState(MARIO_STATE_IDLE);
			}
			//if (state == MARIO_STATE_WALKING_LEFT)
			if (nx < 0)
			{
				vx += 0.2 * MARIO_GRAVITY * dt;
				if (vx > 0)
					SetState(MARIO_STATE_IDLE);
			}
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state!=MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

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

	// No collision occured, proceed normally
	if (coEvents.size()==0)
	{
		x += dx; 
		y += dy;
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

		//if (nx!=0) vx = 0;
		if (ny != 0) vy = 0;


		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba*>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState() != GOOMBA_STATE_DIE)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable == 0)
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
			} // if Goomba
			else if (dynamic_cast<CPortal*>(e->obj))
			{
				CPortal* p = dynamic_cast<CPortal*>(e->obj);
				CGame::GetInstance()->SwitchScene(p->GetSceneId());
			}
			if (dynamic_cast<CBrick*>(e->obj))//if brick
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
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
							state = MARIO_STATE_WALKING_RIGHT;
							//DebugOut(L"Vao ham vx>0, state=%d\n", state);
						}
						else if (vx < 0)
						{
							isJump = false;
							iswalking = true;
							state = MARIO_STATE_WALKING_LEFT;
							//DebugOut(L"Vao ham vx<0, state=%d\n", state);
						}
					}
				}
				/*else if (e->nx != 0)
				{
					if (untouchable == 0)
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
				}*/
			}
		}

	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CMario::Render()
{
	int ani = -1;
	if (state == MARIO_STATE_DIE)
		ani = MARIO_ANI_DIE;
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
	else if (level == MARIO_LEVEL_SMALL)
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
		if (isSkid)
		{
			if (nx < 0)
				ani = MARIO_ANIMATION_SMALL_SKID_RIGHT;
			else if (nx > 0)
				ani = MARIO_STATE_SKID_LEFT;
		}
	}

	int alpha = 255;
	if (untouchable) alpha = 128;
	
	if(state==13||state==14)
		DebugOut(L"state=%d, ani=%d, vx=%f,vy=%f, iscanjumpS=%d, ispressX=%d,isUpS=%d ,iswalking=%d, isJump:%d\n", state, ani,vx,vy,iscanjumpS,ispressX,isUpS ,iswalking,isJump);
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
		if (vx < 0.1f)
			vx += 0.005f;
		nx = 1;
		if (!isJump)
			CGameObject::SetState(state);
		iswalking = true;
		break;
	case MARIO_STATE_WALKING_LEFT: 
		//vx = -MARIO_WALKING_SPEED;
		if (vx > -0.1f)
			vx -= 0.005f;
		nx = -1;
		if (!isJump)
			CGameObject::SetState(state);
		iswalking = true;
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
			if (isBonusvy)
			{
				vy -= MARIO_JUMP_SPEED_Y;
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
		break;
	case MARIO_STATE_DIE:
			CGameObject::SetState(state);
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	}
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y; 

	if (level==MARIO_LEVEL_BIG)
	{
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
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
	//if (isRun)
	//{
	//	if (nx > 0)
	//		SetState(MARIO_STATE_RUN_RIGHT);
	//	else if (nx < 0)
	//		SetState(MARIO_STATE_RUN_LEFT);
	//}

}

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

