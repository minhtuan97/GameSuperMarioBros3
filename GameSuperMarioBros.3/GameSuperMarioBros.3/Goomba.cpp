#include "Goomba.h"
#include "Grid.h"
#include "Brick.h"
#include "QuestionBrick.h"
#include "WaterPipe.h"
#include "Camera.h"
#include "define.h"
#include "Utils.h"

CGoomba::CGoomba(int nx, int type)
{
	SetState(GOOMBA_STATE_WALKING);
	this->type = type;
	
	if(nx==-1)
		vx = -GOOMBA_WALKING_SPEED;
	
	if (type == 1)
		SetState(GOOMBA_STATE_FLY);

}

void CGoomba::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y;
	right = x + GOOMBA_BBOX_WIDTH;
	
	if (state == GOOMBA_STATE_DIE)
		bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
	else 	
		bottom = y + GOOMBA_BBOX_HEIGHT;
	if (type == 1)
	{
		if (state == GOOMBA_STATE_FLY)
			bottom = y + 24;
		else if(state==GOOMBA_STATE_WALKING)
			bottom = y + 19;
	}
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	CGameObject::Update(dt, coObjects);
	if (GetTickCount64() - startdie>TIME_DIE&& state == GOOMBA_STATE_DIE)
	{
		Grid::GetInstance()->deleteObject(this);
		return;
	}
	if (type == 1)
	{
		if (GetTickCount64() - startfly > TIME_FLY && state == GOOMBA_STATE_FLY)
		{
			startfly = 0;
			SetState(GOOMBA_STATE_WALKING);
		}
		if (GetTickCount64() - startwalk > TIME_WALK && state == GOOMBA_STATE_WALKING)
		{
			startwalk = 0;
			SetState(GOOMBA_STATE_FLY);
		}

	}
	vy += GOOMBA__GRAVITY * dt;
	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 

	vector<LPGAMEOBJECT> listbrick;
	vector<LPGAMEOBJECT> listquestionbrick;
	vector<LPGAMEOBJECT> listwaterpipe;
	listbrick.clear();
	listquestionbrick.clear();

	for (int i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i)))
			listbrick.push_back(coObjects->at(i));
		if (dynamic_cast<QuestionBrick*>(coObjects->at(i)))
			listquestionbrick.push_back(coObjects->at(i));
		if (dynamic_cast<WaterPipe*>(coObjects->at(i)))
			listwaterpipe.push_back(coObjects->at(i));

	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != GOOMBA_STATE_DIE)
	{
		CalcPotentialCollisions(&listbrick, coEvents);
		CalcPotentialCollisions(&listquestionbrick, coEvents);
		CalcPotentialCollisions(&listwaterpipe, coEvents);
	}
	if (iskilltop) return;

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
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

			if (dynamic_cast<CBrick*>(e->obj))
			{
				if (e->nx > 0)
					vx = GOOMBA_WALKING_SPEED;
				if (e->nx < 0)
					vx = -GOOMBA_WALKING_SPEED;
			}
			if (dynamic_cast<WaterPipe*>(e->obj))
			{
				if (e->nx > 0)
					vx = GOOMBA_WALKING_SPEED;
				if (e->nx < 0)
					vx = -GOOMBA_WALKING_SPEED;
			}
			if (dynamic_cast<QuestionBrick*>(e->obj))
			{
				if (e->nx > 0)
					vx = GOOMBA_WALKING_SPEED;
				if (e->nx < 0)
					vx = -GOOMBA_WALKING_SPEED;
			}
		}
	}

	DebugOut(L"gomba x=%f, y=%f\n", x, y);
	//goomba ra khoi camera
	float xcam = Camera::GetInstance()->GetCameraPosition().x;
	float ycam = Camera::GetInstance()->GetCameraPosition().y;
	
	if (x>xcam && x<xcam + SCREEN_WIDTH - BRICK_BBOX_WIDTH && y>ycam && y<ycam + SCREEN_HEIGHT- GOOMBA_BBOX_HEIGHT)
		Grid::GetInstance()->Update(this);
	
	//xoa khỏi khi ra khoi map theo trục y
	else if (this->y <= ycam || this->y >= ycam + SCREEN_HEIGHT - GOOMBA_BBOX_HEIGHT)
		Grid::GetInstance()->deleteObject(this);

		
}

void CGoomba::Render()
{
	int ani = GOOMBA_ANI_WALKING;
	if (state == GOOMBA_STATE_DIE) 
	{
		ani = GOOMBA_ANI_DIE;
	}
	if (type == 1)
	{
		if (state == GOOMBA_STATE_FLY)
			ani = GOOMBA_ANI_FLY;
		else if (state == GOOMBA_STATE_WALKING)
			ani = GOOMBA_ANI_FLY_WALK;
		else if (state == GOOMBA_STATE_WALKING2)
			ani = GOOMBA_ANI_FLY_WALK2;
		else if (state == GOOMBA_STATE_DIE)
			ani = GOOMBA_ANI_DIE2;

	}
	animation_set->at(ani)->Render(x,y);

	RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
		case GOOMBA_STATE_DIE:
			y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
			vx = 0;
			vy = 0;
			startdie = GetTickCount64();
			break;
		case GOOMBA_STATE_WALKING:
			vx = -GOOMBA_WALKING_SPEED;
			startwalk = GetTickCount64();
			break;
		case GOOMBA_STATE_FLY:
			vx = -GOOMBA_WALKING_SPEED;
			vy = GOOMBA_FLY_SPEED;
			startfly = GetTickCount64();
			break;
		case GOOMBA_STATE_WALKING2:
			vx = -GOOMBA_WALKING_SPEED;
			break;
	}
}
