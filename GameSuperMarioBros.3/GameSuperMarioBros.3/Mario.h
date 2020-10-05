#pragma once
#include "GameObject.h"

#define MARIO_WALKING_SPEED		0.01f 
//0.1f
#define MARIO_JUMP_SPEED_Y		0.05f
#define MARIO_JUMP_DEFLECT_SPEED 0.2f
#define MARIO_GRAVITY			0.001f
#define MARIO_DIE_DEFLECT_SPEED	 0.5f

#define MARIO_STATE_IDLE			0
#define MARIO_STATE_WALKING_RIGHT	100
#define MARIO_STATE_WALKING_LEFT	200
#define MARIO_STATE_JUMP			300
#define MARIO_STATE_DIE				400

#define MARIO_STATE_IDLE_LEFT		1
#define MARIO_STATE_IDLE_RIGHT		2
#define MARIO_STATE_WALKING_LEFT		3
#define MARIO_STATE_WALKING_RIGHT		4
#define MARIO_STATE_SLIDE_LEFT		5
#define MARIO_STATE_SLIDE_RIGHT		6
#define MARIO_STATE_JUMP_LEFT		7
#define MARIO_STATE_JUMP_RIGHT		8
#define MARIO_STATE_SKID_LEFT		9
#define MARIO_STATE_SKID_RIGHT		10
#define MARIO_STATE_RUN_LEFT		11
#define MARIO_STATE_RUN_RIGHT		12
#define MARIO_STATE_RUNJUMP_LEFT		13
#define MARIO_STATE_RUNJUMP_RIGHT		14
#define MARIO_STATE_HOLD_LEFT		15
#define MARIO_STATE_HOLD_RIGHT		16
#define MARIO_STATE_KICK_LEFT		17
#define MARIO_STATE_KICK_RIGHT		18
#define MARIO_STATE_SWIMIDLE_LEFT		19
#define MARIO_STATE_SWIMIDLE_RIGHT		20
#define MARIO_STATE_SWIMINPUT_LEFT		21
#define MARIO_STATE_SWIMINPUT_RIGHT		22
#define MARIO_STATE_CLIMB_LEFT		23
#define MARIO_STATE_CLIMB_RIGHT		24
#define MARIO_STATE_PIPE		25
#define MARIO_STATE_DIE		26


//ANIMATION SMALL MARIO

#define MARIO_ANIMATION_SMALL_IDLE_LEFT		0
#define MARIO_ANIMATION_SMALL_IDLE_RIGHT		1
#define MARIO_ANIMATION_SMALL_WALKING_LEFT		2
#define MARIO_ANIMATION_SMALL_WALKING_RIGHT		3
#define MARIO_ANIMATION_SMALL_SLIDE_LEFT		4
#define MARIO_ANIMATION_SMALL_SLIDE_RIGHT		5
#define MARIO_ANIMATION_SMALL_JUMP_LEFT		6
#define MARIO_ANIMATION_SMALL_JUMP_RIGHT		7
#define MARIO_ANIMATION_SMALL_SKID_LEFT		8
#define MARIO_ANIMATION_SMALL_SKID_RIGHT		9
#define MARIO_ANIMATION_SMALL_RUN_LEFT		10
#define MARIO_ANIMATION_SMALL_RUN_RIGHT		11
#define MARIO_ANIMATION_SMALL_RUNJUMP_LEFT		12
#define MARIO_ANIMATION_SMALL_RUNJUMP_RIGHT		13
#define MARIO_ANIMATION_SMALL_HOLD_LEFT		14
#define MARIO_ANIMATION_SMALL_HOLD_RIGHT		15
#define MARIO_ANIMATION_SMALL_KICK_LEFT		16
#define MARIO_ANIMATION_SMALL_KICK_RIGHT		17
#define MARIO_ANIMATION_SMALL_SWIMIDLE_LEFT		18
#define MARIO_ANIMATION_SMALL_SWIMIDLE_RIGHT		19
#define MARIO_ANIMATION_SMALL_SWIMINPUT_LEFT		20
#define MARIO_ANIMATION_SMALL_SWIMINPUT_RIGHT		21
#define MARIO_ANIMATION_SMALL_CLIMB_LEFT		22
#define MARIO_ANIMATION_SMALL_CLIMB_RIGHT		23
#define MARIO_ANIMATION_SMALL_PIPE		24
#define MARIO_ANIMATION_SMALL_DIE		25


#define MARIO_ANI_BIG_IDLE_RIGHT		0
#define MARIO_ANI_BIG_IDLE_LEFT			1
#define MARIO_ANI_SMALL_IDLE_RIGHT		2
#define MARIO_ANI_SMALL_IDLE_LEFT			3

#define MARIO_ANI_BIG_WALKING_RIGHT			4
#define MARIO_ANI_BIG_WALKING_LEFT			5
#define MARIO_ANI_SMALL_WALKING_RIGHT		6
#define MARIO_ANI_SMALL_WALKING_LEFT		7

#define MARIO_ANI_DIE				8

#define	MARIO_LEVEL_SMALL	1
#define	MARIO_LEVEL_BIG		2

#define MARIO_BIG_BBOX_WIDTH  16
#define MARIO_BIG_BBOX_HEIGHT 32

#define MARIO_SMALL_BBOX_WIDTH  16
#define MARIO_SMALL_BBOX_HEIGHT 16

#define MARIO_UNTOUCHABLE_TIME 5000
#define MARIO_SKID_TIME 200


class CMario : public CGameObject
{
	int level;
	int untouchable;
	DWORD untouchable_start;

	float start_x;			// initial position of Mario at scene
	float start_y; 
public: 
	DWORD skid_start;


	bool isJump = false;
	bool iswalking = false;
	bool isBonusvy = true;
	bool ispressX = false;
	bool isjumpX = false;
	bool iscanjumpS = false;
	bool isUpS = true;
	float power = 0.0f;
	bool isRun = false;
	bool isSkid = false;

	CMario(float x = 0.0f, float y = 0.0f);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount64(); }

	void Reset();

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	void DecreasePower();
	void IncreasePower();

};