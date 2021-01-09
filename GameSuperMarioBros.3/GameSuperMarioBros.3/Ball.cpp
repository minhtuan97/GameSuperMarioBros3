#include "Ball.h"
#include "WaterPipe.h"
#include "Brick.h"
#include "Goomba.h"
#include "Koopas.h"
#include "Camera.h"
#include "Utils.h"
#include "define.h"
#include "Grid.h"

void Ball::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + BALL_BBOX_WIDTH;
	bottom = y + BALL_BBOX_HEIGHT;
}

void Ball::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);
	vy += BALL_GRAVITY * dt;

	vector<LPGAMEOBJECT> listbrick;
	vector<LPGAMEOBJECT> listwaterpipe;

	listbrick.clear();
	listwaterpipe.clear();

	for (int i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i)))
			listbrick.push_back(coObjects->at(i));
		if (dynamic_cast<WaterPipe*>(coObjects->at(i)))
			listwaterpipe.push_back(coObjects->at(i));

	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

		CalcPotentialCollisions(&listbrick, coEvents);
		CalcPotentialCollisions(&listwaterpipe, coEvents);

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += nx*dx;
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
				if (e->ny != 0)
					vy = -BALL_WALKING_SPEED_Y;
				else if (e->nx != 0)
					isthrow = false;
			}
			if (dynamic_cast<WaterPipe*>(e->obj))
			{
				isthrow = false;
			}
		}
	}
	//ball ra khoi camera
	float xcam = Camera::GetInstance()->GetCameraPosition().x;
	float ycam = Camera::GetInstance()->GetCameraPosition().y;

	if (x<xcam || x>xcam + SCREEN_WIDTH || y<ycam || y>ycam + SCREEN_HEIGHT)
		isthrow = false;
	ColitionWithEnemy(coObjects);
	//Grid::GetInstance()->Update(this);
}

void Ball::Render()
{
	int ani = 0;
	if (nx > 0)
		ani = BALL_ANIMATION_RIGHT;
	else if (nx < 0)
		ani = BALL_ANIMATION_LEFT;
	animation_set->at(ani)->Render(x, y);
}

Ball::Ball()
{
	isthrow = true;
	vx = BALL_WALKING_SPEED_X;
	vy = BALL_WALKING_SPEED_Y;
}

void Ball::ColitionWithEnemy(vector<LPGAMEOBJECT>* listObject)
{
	//isColiWithKoopas = false;

	vector<LPGAMEOBJECT> listkoopas;
	vector<LPGAMEOBJECT> listgoomba;
	listkoopas.clear();

	for (int i = 0; i < listObject->size(); i++)
	{
		if (dynamic_cast<CKoopas*>(listObject->at(i)))
		{
			//CKoopas* k = dynamic_cast<CKoopas*>(listObject->at(i));
			//if (!k->ishold)
				listkoopas.push_back(listObject->at(i));
		}
		if (dynamic_cast<CGoomba*>(listObject->at(i)))
		{
			//CGoomba* k = dynamic_cast<CGoomba*>(listObject->at(i));
			listgoomba.push_back(listObject->at(i));
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
		CalcPotentialCollisions(&listkoopas, coEvents);
		CalcPotentialCollisions(&listgoomba, coEvents);

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		//isColiWithKoopas = false;

	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);
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

			if (dynamic_cast<CKoopas*>(e->obj))
			{
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				koopas->SetState(KOOPAS_STATE_DIE);
				isthrow = false;

			}
			if (dynamic_cast<CGoomba*>(e->obj))
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				goomba->SetState(GOOMBA_STATE_DIE);
				goomba->SetSpeed(0, -0.1);
				isthrow = false;
			}
		}
	}
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++)
		delete coEvents[i];
}
