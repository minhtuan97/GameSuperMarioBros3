#include <iostream>
#include <fstream>

#include "PlayScence.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "Box.h"
#include "QuestionBrick.h"
#include "Item.h"
#include "WaterPipe.h"
#include "Ball.h"
#include "Coin.h"
#include "Plant.h"
#include "BallPlant.h"
#include "ItemSelect.h"
#include "ScenceSelect.h"
#include "SwitchBlock.h"

#define animationball	5

using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath):
	CScene(id, filePath)
{
	key_handler = new CPlayScenceKeyHandler(this);
	cam = Camera::GetInstance();
	board = new Board(0, 0);
	time_start = GetTickCount64();

}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
	See scene1.txt, scene2.txt for detail format specification
*/

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_TEXTURES 2
#define SCENE_SECTION_SPRITES 3
#define SCENE_SECTION_ANIMATIONS 4
#define SCENE_SECTION_ANIMATION_SETS	5
#define SCENE_SECTION_OBJECTS	6
#define SCENE_SECTION_MAP	7


#define OBJECT_TYPE_MARIO	0
#define OBJECT_TYPE_BRICK	1
#define OBJECT_TYPE_GOOMBA	2
#define OBJECT_TYPE_KOOPAS	3
#define OBJECT_TYPE_BOX		4
#define OBJECT_TYPE_QUESTIONBRICK	5
#define	OBJECT_TYPE_ITEM	6
#define	OBJECT_TYPE_WATERPIPE	7
#define	OBJECT_TYPE_COIN	8
#define	OBJECT_TYPE_PLANT	9
#define	OBJECT_TYPE_ITEMSELECT	10
#define OBJECT_TYPE_SCENCESELECT	11

#define OBJECT_TYPE_PORTAL	50

#define MAX_SCENE_LINE 1024


void CPlayScene::_ParseSection_TEXTURES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 5) return; // skip invalid lines

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());

	CTextures::GetInstance()->Add(texID, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void CPlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPDIRECT3DTEXTURE9 tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return; 
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CPlayScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i+1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void CPlayScene::_ParseSection_ANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines - an animation set must at least id and one animation id

	int ani_set_id = atoi(tokens[0].c_str());

	LPANIMATION_SET s = new CAnimationSet();

	CAnimations *animations = CAnimations::GetInstance();

	for (int i = 1; i < tokens.size(); i++)
	{
		int ani_id = atoi(tokens[i].c_str());
		
		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
	}

	CAnimationSets::GetInstance()->Add(ani_set_id, s);
}

/*
	Parse a line in section [OBJECTS] 
*/
void CPlayScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	if (tokens.size() < 3) return; // skip invalid lines - an object set must have at least id, x, y

	int object_type = atoi(tokens[0].c_str());
	float x = atof(tokens[1].c_str());
	float y = atof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());

	CAnimationSets * animation_sets = CAnimationSets::GetInstance();

	CGameObject *obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		if (player!=NULL) 
		{
			DebugOut(L"[ERROR] MARIO object was created before!\n");
			return;
		}
		//obj = new CMario(x,y); 
		obj = CMario::GetInstance();
		obj->SetPosition(x, y);
		player = (CMario*)obj;  
		player->scence = id;
		if (id == 0)
			player->SetSpeed(0, 0);
		DebugOut(L"[INFO] Player object created!\n");
		break;
	case OBJECT_TYPE_GOOMBA: 
	{
		int nx = atof(tokens[4].c_str());
		int type = atof(tokens[5].c_str());
		obj = new CGoomba(nx,type);
		break;
	}
	case OBJECT_TYPE_SCENCESELECT:
	{
		int l = atoi(tokens[4].c_str());
		int r = atoi(tokens[5].c_str());
		int u = atoi(tokens[6].c_str());
		int d = atoi(tokens[7].c_str());
		int id = atoi(tokens[8].c_str());
		obj = new ScenceSelect();
		ScenceSelect* s = (ScenceSelect*)obj;
		s->canleft = l;
		s->canright = r;
		s->candown = d;
		s->canup = u;
		s->id_scence = id;
		listscenceselect.push_back(s);
		s->SetPosition(x, y);
		return;
	}
	case OBJECT_TYPE_BRICK: 
	{
		obj = new CBrick(x,y);
		int r = atoi(tokens[4].c_str());
		int b = atoi(tokens[5].c_str());
		int type = atoi(tokens[6].c_str());
		CBrick* brick=(CBrick*)obj;
		brick->right = r;
		brick->bot = b;
		brick->type = type;
		break;
	}
	case OBJECT_TYPE_BOX:
	{
		obj = new Box();
		int r = atof(tokens[4].c_str());
		int b = atof(tokens[5].c_str());
		Box* brick = (Box*)obj;
		brick->right = r;
		brick->bot = b;
		break;
	}
	case OBJECT_TYPE_QUESTIONBRICK:
	{
		obj = new QuestionBrick(); 
		QuestionBrick* brick = (QuestionBrick*)obj;
		brick->iditem = atof(tokens[4].c_str());
		brick->xde = x;
		brick->yde = y;
		break;
	}
	case OBJECT_TYPE_WATERPIPE:
	{
		obj = new WaterPipe();
		WaterPipe* p = (WaterPipe*)obj;
		int ani = atoi(tokens[4].c_str());
		p->ani = ani;
		break;
	}
	case OBJECT_TYPE_ITEM:
	{
		obj = new Item();
		Item* item = (Item*)obj;
		item->type= atof(tokens[4].c_str());
		item->id= atof(tokens[5].c_str());
		item->xde = x;
		item->yde = y;
		objects_item.push_back(obj);
		break;
	}
	case OBJECT_TYPE_KOOPAS: 
	{
		float xmin = atof(tokens[4].c_str());
		float xmax = atof(tokens[5].c_str());
		int type = atoi(tokens[6].c_str());
		obj = new CKoopas(type,x,y);
		CKoopas* k = (CKoopas*)obj;
		k->x_min = xmin;
		k->x_max = xmax;

		break;
	}
	case OBJECT_TYPE_PLANT:
	{
		float ymin = atof(tokens[4].c_str());
		float ymax = atof(tokens[5].c_str());
		int type = atoi(tokens[6].c_str());
		obj = new Plant(type, x, y);
		Plant* k = (Plant*)obj;
		k->y_min = ymin;
		k->y_max = ymax;
		break;
	}
	case OBJECT_TYPE_COIN:
	{
		obj = new Coin(x,y);
		break;
	}
	case OBJECT_TYPE_ITEMSELECT:
	{
		obj = new ItemSelect();
		break;
	}
	case OBJECT_TYPE_PORTAL:
		{	
			float r = atof(tokens[4].c_str());
			float b = atof(tokens[5].c_str());
			int scene_id = atoi(tokens[6].c_str());
			obj = new CPortal(x, y, r, b, scene_id);
		}
		break;
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	obj->SetAnimationSet(ani_set);
	grid = Grid::GetInstance();
	try
	{
		if (obj != NULL && !dynamic_cast<CMario*>(obj)&&!dynamic_cast<Item*>(obj))
			grid->addObject(obj);
	}
	catch (const std::exception& e)
	{
		DebugOut(L"loi %s\n", e);
	}
	//objects.push_back(obj);
}

void CPlayScene::_ParseSection_MAP(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines

	LPCWSTR pathMap = ToLPCWSTR(tokens[1].c_str());
	map = Map::GetInstance();
	map->LoadFile(pathMap, atoi(tokens[0].c_str()));
}



void CPlayScene::Load()
{
	DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;					

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[TEXTURES]") { section = SCENE_SECTION_TEXTURES; continue; }
		if (line == "[SPRITES]") { 
			section = SCENE_SECTION_SPRITES; continue; }
		if (line == "[ANIMATIONS]") { 
			section = SCENE_SECTION_ANIMATIONS; continue; }
		if (line == "[ANIMATION_SETS]") { 
			section = SCENE_SECTION_ANIMATION_SETS; continue; }
		if (line == "[OBJECTS]") { 
			section = SCENE_SECTION_OBJECTS; continue; }
		if (line == "[MAP]") {
			section = SCENE_SECTION_MAP; continue;}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }	

		//
		// data section
		//
		switch (section)
		{ 
			case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
			case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
			case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
			case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
			case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
			case SCENE_SECTION_MAP: _ParseSection_MAP(line); break;
		}
	}

	f.close();

	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));

	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);
}

void CPlayScene::Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	if (id == 0)
	{
		cam->SetCameraPosition(0,0);
		board->SetPosition(8, 200);
		player->Update(dt, &listscenceselect);
		return;
	}

	vector<LPGAMEOBJECT> coObjects;
	//for (size_t i = 1; i < objects.size(); i++)
	//{
	//	coObjects.push_back(objects[i]);
	//}
	grid->GetListOfObjects(&coObjects);
	vector<BallPlant*> listballplant;
	listballplant.clear();
	vector<Coin*> listCoin;
	listCoin.clear();
	vector<CBrick*> listBrick;
	listBrick.clear();
	vector<Plant*> listPlant;
	listPlant.clear();

	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &coObjects);
	}

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return; 
	player->Update(dt, &coObjects);

	//add item vao grid
	for (int i = 0; i < objects.size(); i++)
	{
		if (dynamic_cast<QuestionBrick*>(objects.at(i)))
		{
			QuestionBrick* brick = dynamic_cast<QuestionBrick*>(objects.at(i));
			if (brick->isColi&&!brick->isAddItem)
			{
				for (int m = 0; m < objects_item.size(); m++)
				{
					Item* item = dynamic_cast<Item*>(objects_item.at(m));
					if (item->id == brick->iditem)
					{
						if (item->type == 0)item->SetSpeed(0, -0.3);
						if (item->type == 1)item->SetSpeed(0, -0.01);
						if (item->type == 2)
						{
							item->SetPosition(brick->x,brick->y);
							item->SetSpeed(0, -0.3);
						}
						grid->addObject(item);
						brick->isAddItem = true;
					}
				}
			}
		}
			
	}
	//add nut switch P
	for (int i = 0; i < objects.size(); i++)
	{
		if (dynamic_cast<CBrick*>(objects.at(i)))
		{
			CBrick* brick = dynamic_cast<CBrick*>(objects.at(i));
			if (brick->type==BRICK5 &&brick->isP && !brick->isAddItem)
			{

						SwitchBlock* s = new SwitchBlock(brick->x, brick->y-16);
						grid->addObject(s);
						brick->isAddItem = true;
			}
		}
	}




	for (size_t i = 0; i < objects.size(); i++)
	{
		if (dynamic_cast<BallPlant*>(objects.at(i)))
		{
			BallPlant* e = dynamic_cast<BallPlant*>(objects.at(i));
			listballplant.push_back(e);
		}		
	}
	for (size_t i = 0; i < listballplant.size(); i++)
	{
		if (listballplant.at(i)->isattack == true)
			grid->deleteObject(listballplant.at(i));
	}
	//update coin
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (dynamic_cast<Coin*>(objects.at(i)))
		{
			Coin* e = dynamic_cast<Coin*>(objects.at(i));
			listCoin.push_back(e);
		}
	}
	for (size_t i = 0; i < listCoin.size(); i++)
	{
		if (listCoin.at(i)->iscoli == true)
			grid->deleteObject(listCoin.at(i));
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		if (dynamic_cast<CBrick*>(objects.at(i)))
		{
			CBrick* e = dynamic_cast<CBrick*>(objects.at(i));
			listBrick.push_back(e);
		}
	}
	for (size_t i = 0; i < listBrick.size(); i++)
	{
		if (listBrick.at(i)->isColi == true)
			grid->deleteObject(listBrick.at(i));
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		if (dynamic_cast<Plant*>(objects.at(i)))
		{
			Plant* e = dynamic_cast<Plant*>(objects.at(i));
			listPlant.push_back(e);
		}
	}
	for (size_t i = 0; i < listPlant.size(); i++)
	{
		if (listPlant.at(i)->isColi == true)
			grid->deleteObject(listPlant.at(i));
	}


	// Update camera to follow mario
	float cx, cy, mapheight, mapwidth;
	mapheight = map->GetHeight();
	mapwidth = map->GetWidth();
	player->GetPosition(cx, cy);
	if (player->iswag)
	{
		if (player->nx > 0)
			cx -= 8;
		else if (player->nx < 0)
			cx += 8;
	}
	D3DXVECTOR3 pos = cam->GetCameraPosition();
	if (mapwidth > SCREEN_WIDTH) {
		if (cx + 5 < SCREEN_WIDTH / 2) {
			cx = pos.x;
		}
		else if (cx + SCREEN_WIDTH / 2 > mapwidth - 1) {
			cx = mapwidth - SCREEN_WIDTH;
		}
		else {
			cx = cx + 5 + SCREEN_WIDTH / 2 - SCREEN_WIDTH;
		}
	}
	else {
		cx = 0;
	}

	if (mapheight > SCREEN_HEIGHT )
	{
		if (cy < SCREEN_HEIGHT-32)
		{
			cy = 0;
		} 
		else if (cy > mapheight - SCREEN_HEIGHT/2)
		{
			cy= mapheight - SCREEN_HEIGHT+32;
		}
		else //if (cy < mapheight - SCREEN_HEIGHT)
		{
			cy = cy - SCREEN_HEIGHT/2+32;
		}
	}
	else 
	{
		cy = mapheight - SCREEN_HEIGHT;
	}
	if (cy < 0) cy = 0;
	//cy -= SCREEN_HEIGHT / 2;

	cam->SetCameraPosition((int)cx, (int)cy);
	if (board != NULL)
		board->SetPosition((int)cx + 8, (int) cy+200);
}

void CPlayScene::Render()
{
	if (id == 0)
	{

		CSprites* sprites = CSprites::GetInstance();
		sprites->Get(40050)->Draw(5, 20);
		board->Render(player, 300 - (GetTickCount64() - time_start) / 1000);
		if (player)
		{
			player->Render();
		}
		for (int i = 0; i < listscenceselect.size(); i++)
		{
			listscenceselect.at(i)->Render();
		}
		return;
	}
	map->DrawMap();
	grid->GetListOfObjects(&objects);
	objects=Camera::GetInstance()->GetlistinCamera(objects);
	vector<LPGAMEOBJECT> listobject;
	vector<LPGAMEOBJECT> listitem;
	vector<LPGAMEOBJECT> listPipe;
	vector<LPGAMEOBJECT> listPlant;

	for (int i = 0; i < objects.size(); i++)
	{
			if (dynamic_cast<Item*>(objects.at(i)))
				listitem.push_back(objects.at(i));
			else
				if (dynamic_cast<WaterPipe*>(objects.at(i)))
					listPipe.push_back(objects.at(i));
				else
					if (dynamic_cast<Plant*>(objects.at(i)))
						listPlant.push_back(objects.at(i));
					else
						listobject.push_back(objects.at(i));
	}
	
	for (int i = 0; i < listobject.size(); i++)
	{
		listobject[i]->Render();
	}
	for (int i = 0; i < listitem.size(); i++)
	{
		listitem[i]->Render();
	}
	
	for (int i = 0; i < listPlant.size(); i++)
	{
		listPlant[i]->Render();
	}
	for (int i = 0; i < listPipe.size(); i++)
	{
		listPipe[i]->Render();
	}
	if (player)
	{
		player->Render();
		for (int i = 0; i < player->listball.size(); i++)
			player->listball.at(i)->Render();
	}

	if (player->iscard)
	{
		string1.Draw(2650, 260, s1);
		string2.Draw(2630, 280, s2);
		card.Draw(2770, 270, player->typecard);
	}

	board->Render(player, 300-(GetTickCount64()-time_start)/1000);
}

/*
	Unload current scene
*/
void CPlayScene::Unload()
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];
	for (int i = 0; i < listscenceselect.size(); i++)
		delete listscenceselect[i];
	for (int i = 0; i < objects_item.size(); i++)
		delete objects_item[i];

	objects.clear();
	listscenceselect.clear();
	objects_item.clear();

	//player->SetPosition(30, 10);

	Grid::GetInstance()->ClearObject();
	map->Clear();
	Camera::GetInstance()->SetCameraPosition(0, 0);


	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void CPlayScenceKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	CGame* game = CGame::GetInstance();
	CMario *mario = ((CPlayScene*)scence)->GetPlayer();
	if (mario->scence == 0)
	{
		if (!(!mario->autoleft && !mario->autoright && !mario->autodown && !mario->autoup))
			return;
		switch (KeyCode)
		{
		case DIK_LEFT:
			if (!mario->autoleft&&mario->canleft)
			{
				mario->autoleft = true;
				mario->x_star = mario->x;
			}
			mario->autoright = false;
			mario->autodown = false;
			mario->autoup = false;
			break;
		case DIK_RIGHT:
			mario->autoleft = false;
			if (!mario->autoright&&mario->canright)
			{
				mario->autoright = true;
				mario->x_star = mario->x;

			}
			mario->autodown = false;
			mario->autoup = false;
			break;
		case DIK_UP:
			mario->autoleft = false;
			mario->autoright = false;
			mario->autodown = false;
			if (!mario->autoup&&mario->canup)
			{
				mario->autoup = true;
				mario->y_start = mario->y;
			}
			break;
		case DIK_DOWN:
			mario->autoleft = false;
			mario->autoright = false;
			if (!mario->autodown&&mario->candown)
			{
				mario->autodown = true;
				mario->y_start = mario->y;
			}
			mario->autoup = false;
			break;
		case DIK_S:
			if (mario->scenceselect > 0)
			{
				CGame::GetInstance()->SwitchScene(mario->scenceselect);

			}
			break;
		default:
			break;
		}
		return;
	}
	switch (KeyCode)
	{
	
	case DIK_X:
		if (mario->isJump||mario->isjumpX)
			break;
		mario->ispressX = true;
		mario->isjumpX = true;
		if (mario->isRun)
		{
			if (mario->nx > 0)
				mario->SetState(MARIO_STATE_JUMP_RIGHT);
			else
				mario->SetState(MARIO_STATE_JUMP_LEFT);
		}
		else
		{
			if (mario->nx > 0)
				mario->SetState(MARIO_STATE_JUMP_RIGHT);
			else
				mario->SetState(MARIO_STATE_JUMP_LEFT);
		}
		break;
	case DIK_S:
		mario->isUpS = false;
		if (mario->isJump&&!mario->isBonusvy&&mario->GetLevel()==MARIO_LEVEL_RACCOON)
		{
			mario->isfly = true;
			mario->SetSpeed(mario->vx, -0.1);
		}
		break;
	case DIK_A: 
		//add ball
		if (mario->GetLevel() == MARIO_LEVEL_FIRE)
		{
			if (!mario->isthrow)
			{
				if (mario->listball.size() < 3)
				{
					Ball* b = new Ball();
					b->SetPosition(mario->x, mario->y);
					b->nx = mario->nx;
					LPANIMATION_SET ani_set = CAnimationSets::GetInstance()->Get(animationball);
					b->SetAnimationSet(ani_set);
					mario->listball.push_back(b);
					mario->isthrow = true;
					mario->throw_start = GetTickCount64();
				}
			}
		}
		if (mario->GetLevel() == MARIO_LEVEL_RACCOON)
		{
			if (!mario->iswag)
			{
				mario->iswag = true;
				mario->wag_start = GetTickCount64();
				if (mario->nx > 0)
					mario->SetPosition(mario->x + 8, mario->y);
				if (mario->nx < 0)
					mario->SetPosition(mario->x - 8, mario->y);

			}
		}

		break;
	case DIK_ESCAPE:
		DestroyWindow(game->getHwnd());
		break;
	case DIK_F:
		mario->SetLevel(MARIO_LEVEL_FIRE);
		break;
	case DIK_R:
		mario->SetLevel(MARIO_LEVEL_RACCOON);
		break;
	}
}

void CPlayScenceKeyHandler::OnKeyUp(int KeyCode) 
{
	CGame* game = CGame::GetInstance();
	CMario* mario = ((CPlayScene*)scence)->GetPlayer();
	if (mario->scence == 0) return;
	switch (KeyCode)
	{
	case DIK_X:
		mario->ispressX = false;
		break;
	case DIK_A:
		mario->SetState(MARIO_STATE_IDLE);
		if (mario->rua)
		{
			mario->rua->ishold = false;
			mario->rua->SetState(KOOPAS_STATE_KICK);
			mario->rua == NULL;
		}
		mario->isColiWithKoopas = false;
		break;
	case DIK_M:
		mario->nx = -mario->nx;
		break;
	case DIK_S:
		mario->isUpS = true;
		mario->iscanjumpS = false;
		if (mario->isfly)
		{
			mario->isfly = false;
		}
		break;
	}
}

void CPlayScenceKeyHandler::KeyState(BYTE *states)
{
	CGame *game = CGame::GetInstance();
	CMario *mario = ((CPlayScene*)scence)->GetPlayer();
	if (mario->scence == 0)
		return;
	// disable control key when Mario die 
	if (mario->GetState() == MARIO_STATE_DIE) return;
	if (game->IsKeyDown(DIK_RIGHT))
	{
		if (mario->state == MARIO_STATE_WALKING_LEFT)
		{
			mario->isSkid = true;
			mario->skid_start = GetTickCount64();
		}
		mario->SetState(MARIO_STATE_WALKING_RIGHT);
		if (game->IsKeyDown(DIK_A))
		{
			mario->IncreasePower();			
		}
		if (mario->isRun)
		{
			if (mario->GetState() == MARIO_STATE_WALKING_RIGHT);
				mario->SetState(MARIO_STATE_RUN_RIGHT);
		}
			

		
	}
	else if (game->IsKeyDown(DIK_LEFT))
	{
		if (mario->state == MARIO_STATE_WALKING_RIGHT)
		{
			mario->isSkid = true;
			mario->skid_start = GetTickCount64();
		}
		mario->SetState(MARIO_STATE_WALKING_LEFT);
		if (game->IsKeyDown(DIK_A))
		{
			mario->IncreasePower();
		}

		if (mario->isRun)
		{
			if(mario->GetState()== MARIO_STATE_WALKING_LEFT)
				mario->SetState(MARIO_STATE_RUN_LEFT);
		}
	}
	//nut s
	if (game->IsKeyDown(DIK_S) && !mario->isjumpX && mario->iscanjumpS)
	{
		if (mario->isRun)
		{
			if (mario->nx > 0)
				mario->SetState(MARIO_STATE_RUNJUMP_RIGHT);
			else
				mario->SetState(MARIO_STATE_RUNJUMP_LEFT);
			mario->vy = -0.1f;
		}
		else
		{
			if (mario->nx > 0)
				mario->SetState(MARIO_STATE_JUMP_RIGHT);
			else
				mario->SetState(MARIO_STATE_JUMP_LEFT);
		}
	}
	if (!((game->IsKeyDown(DIK_LEFT)|| game->IsKeyDown(DIK_RIGHT)) && game->IsKeyDown(DIK_A)))
		mario->DecreasePower();
	//DebugOut(L"power:%f\n", mario->power);
	//nut A
	if (game->IsKeyDown(DIK_A))
	{
		mario->iscanHold = true;
		if (mario->isColiWithKoopas)
		{
			DebugOut(L"Vao ham hold\n");
			if (!mario->isHold)
			{
				mario->isHold = true;
				mario->hold_start = GetTickCount64();
				DebugOut(L"vao ham gettickcount\n");
			}
			mario->rua->ishold = true;
		}
	}
	else
	{
		mario->iscanHold = false;
		mario->isHold = false;		
	}

}