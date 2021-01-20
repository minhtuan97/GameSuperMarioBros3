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
#include "BrickMove.h"
#include "Number3.h"
#include "Hammer.h"
#include "HammerBros.h"

#define animationball	5

using namespace std;

CMario* CPlayScene::player = NULL;
//CMario* CPlayScene::playermap = NULL;

CPlayScene::CPlayScene(int id, LPCWSTR filePath):
	CScene(id, filePath)
{
	key_handler = new CPlayScenceKeyHandler(this);
	cam = Camera::GetInstance();
	cam->SetCameraPosition(0, 0);
	board = new Board(0, 0);
	time_start = GetTickCount64();
	map = Map::GetInstance();
	time = GetTickCount64();
}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
	See scene1.txt, scene2.txt for detail format specification
*/
#define	SCENCE_TYPE_INTRO	0
#define	SCENCE_TYPE_WORLDMAP	1
#define	SCENCE_TYPE_GAMEPLAY	2

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_TEXTURES 2
#define SCENE_SECTION_SPRITES 3
#define SCENE_SECTION_ANIMATIONS 4
#define SCENE_SECTION_ANIMATION_SETS	5
#define SCENE_SECTION_OBJECTS	6
#define SCENE_SECTION_MAP	7
#define SCENE_SECTION_SETTING	8


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
#define OBJECT_TYPE_BRICKMOVE	12
#define OBJECT_TYPE_NUMBER3	13
#define OBJECT_TYPE_SELECTPLAYER	14
#define OBJECT_TYPE_QUESTIONBRICK_2	15
#define OBJECT_TYPE_HAMMERBROS	16
#define OBJECT_TYPE_CARD	17

#define OBJECT_TYPE_PORTAL	50

#define MAX_SCENE_LINE 1024

#define BROAD_X_WORLDMAP 8
#define BROAD_Y_WORLDMAP 200


void CPlayScene::_ParseSection_SETTINGSCENCE(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines
	int movecamera = atoi(tokens[0].c_str());
	if (movecamera == 1)
		isCameraAutoMove = true;
	else if(movecamera == 0)
		isCameraAutoMove = false;

	typeScence = atoi(tokens[1].c_str());
}

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

void CPlayScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	if (tokens.size() < 3) return; // skip invalid lines - an object set must have at least id, x, y

	int object_type = atoi(tokens[0].c_str());
	float x = atof(tokens[1].c_str());
	float y = atof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());

	int cell_grid = atoi(tokens[4].c_str());
	int row_grid = atoi(tokens[5].c_str());

	CAnimationSets * animation_sets = CAnimationSets::GetInstance();

	CGameObject *obj = NULL;

	if (typeScence== SCENCE_TYPE_INTRO)
	{
		switch (object_type)
		{
		case OBJECT_TYPE_BRICK:
		{
			obj = new CBrick(x, y);
			CBrick* brick = (CBrick*)obj;
			int r = atof(tokens[6].c_str());
			int b = atof(tokens[7].c_str());
			brick->right = r;
			brick->bot = b;
			break;
		}
		case OBJECT_TYPE_NUMBER3:
		{
			obj = new Number3();
			break;
		}
		case OBJECT_TYPE_SELECTPLAYER:
		{
			obj = new SelectPlayer();
			selectplayer = (SelectPlayer*)obj;
			break;
		}
		}
		// General object setup
		obj->SetPosition(x, y);

		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

		obj->SetAnimationSet(ani_set);
		objects.push_back(obj);
		return;
	}
	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		if (typeScence== SCENCE_TYPE_WORLDMAP)
		{
			//if (playermap != NULL)
			{
				playermap = new CMario(x,y);
				LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
				playermap->scence = id;
				playermap->SetAnimationSet(ani_set);
				DebugOut(L"[ERROR] MARIO 1 object was created before!\n");
				return;
			}
		}
		else
		{
			if (player != NULL)
			{
				LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
				player->scence = id;
				player->SetAnimationSet(ani_set);
				player->iscard = false;
				player->portal = NULL;
				player->isCanSwitchScenceDown = false;
				player->isCanSwitchScenceUP = false;
				player->SetPosition(x, y);
				DebugOut(L"[ERROR] MARIO object was created before!\n");
				return;
			}
			//obj = new CMario(x,y); 
			obj = CMario::GetInstance();
			obj->SetPosition(x, y);
			player = (CMario*)obj;
			player->scence = id;
			DebugOut(L"[INFO] Player object created!\n");
		}
		
		break;
	case OBJECT_TYPE_GOOMBA: 
	{
		int nx = atof(tokens[6].c_str());
		int type = atof(tokens[7].c_str());
		obj = new CGoomba(nx,type);
		break;
	}
	case OBJECT_TYPE_SCENCESELECT:
	{
		int l = atoi(tokens[6].c_str());
		int r = atoi(tokens[7].c_str());
		int u = atoi(tokens[8].c_str());
		int d = atoi(tokens[9].c_str());
		int id = atoi(tokens[10].c_str());
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
		int r = atoi(tokens[6].c_str());
		int b = atoi(tokens[7].c_str());
		int type = atoi(tokens[8].c_str());
		CBrick* brick=(CBrick*)obj;
		brick->right = r;
		brick->bot = b;
		brick->type = type;
		break;
	}
	case OBJECT_TYPE_BOX:
	{
		obj = new Box();
		int r = atof(tokens[6].c_str());
		int b = atof(tokens[7].c_str());
		Box* brick = (Box*)obj;
		brick->right = r;
		brick->bot = b;
		break;
	}
	case OBJECT_TYPE_QUESTIONBRICK:
	{
		obj = new QuestionBrick(); 
		QuestionBrick* brick = (QuestionBrick*)obj;
		brick->iditem = atof(tokens[6].c_str());
		brick->xde = x;
		brick->yde = y;
		break;
	}
	case OBJECT_TYPE_QUESTIONBRICK_2:
	{
		obj = new QuestionBrick();
		QuestionBrick* brick = (QuestionBrick*)obj;
		brick->iditem = atoi(tokens[6].c_str());
		brick->countColi = atoi(tokens[7].c_str());
		brick->xde = x;
		brick->yde = y;
		break;
	}
	case OBJECT_TYPE_WATERPIPE:
	{
		obj = new WaterPipe();
		WaterPipe* p = (WaterPipe*)obj;
		int ani = atoi(tokens[6].c_str());
		p->ani = ani;
		break;
	}
	case OBJECT_TYPE_ITEM:
	{
		obj = new Item();
		Item* item = (Item*)obj;
		item->type= atof(tokens[6].c_str());
		item->id= atof(tokens[7].c_str());
		item->xde = x;
		item->yde = y;
		objects_item.push_back(obj);
		break;
	}
	case OBJECT_TYPE_KOOPAS: 
	{
		float xmin = atof(tokens[6].c_str());
		float xmax = atof(tokens[7].c_str());
		int type = atoi(tokens[8].c_str());
		obj = new CKoopas(type,x,y);
		CKoopas* k = (CKoopas*)obj;
		k->x_min = xmin;
		k->x_max = xmax;
		break;
	}
	case OBJECT_TYPE_HAMMERBROS:
	{
		obj = new HammerBros();
		float xmin = atof(tokens[6].c_str());
		float xmax = atof(tokens[7].c_str());
		HammerBros* k = (HammerBros*)obj;
		k->x_min = xmin;
		k->x_max = xmax;
		break;
	}
	case OBJECT_TYPE_PLANT:
	{
		float ymin = atof(tokens[6].c_str());
		float ymax = atof(tokens[7].c_str());
		int type = atoi(tokens[8].c_str());
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
			float r = atof(tokens[6].c_str());
			float b = atof(tokens[7].c_str());
			float xm = atof(tokens[9].c_str());
			float ym = atof(tokens[10].c_str());

			int scene_id = atoi(tokens[8].c_str());
			obj = new CPortal(x, y, r, b, scene_id);
			CPortal* c;
			c= (CPortal*)obj;
			c->yCPortal = atoi(tokens[11].c_str());
			c->nynewscence= atoi(tokens[12].c_str());
			c->xmario = xm;
			c->ymario = ym;
		}
		break;
	case OBJECT_TYPE_BRICKMOVE:
	{
		obj = new BrickMove();
		break;
	}
	case OBJECT_TYPE_NUMBER3:
	{
		obj = new Number3();
		break;
	}
	case OBJECT_TYPE_CARD:
	{
		int x1 = atoi(tokens[1].c_str());
		int y1 = atoi(tokens[2].c_str());
		int x2 = atoi(tokens[3].c_str());
		int y2 = atoi(tokens[6].c_str());
		int x3 = atoi(tokens[7].c_str());
		int y3 = atoi(tokens[8].c_str());

		string1 = new Font(x1, y1, s1);
		string2 = new Font(x2, y2, s2);
		card = new Font(x3, y3);
		return;
	}
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);
	obj->SetGrid(cell_grid, row_grid);

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	obj->SetAnimationSet(ani_set);
	grid = Grid::GetInstance();
	try
	{
		if (obj != NULL && !dynamic_cast<CMario*>(obj)&&!dynamic_cast<Item*>(obj))
			grid->addObjectOff(obj);
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
		if (line == "[SETTING]") {
			section = SCENE_SECTION_SETTING; continue;
		}
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
			case SCENE_SECTION_SETTING: _ParseSection_SETTINGSCENCE(line); break;
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
	switch (typeScence)
	{
	case SCENCE_TYPE_INTRO:
		for (int i = 0; i < objects.size(); i++)
		{
			objects.at(i)->Update(dt, &objects);
		}
		break;
	case SCENCE_TYPE_WORLDMAP:
		cam->SetCameraPosition(0, 0);
		board->SetPosition(8, 200);
		playermap->Update(dt, &listscenceselect);
		return;
		break;
	case SCENCE_TYPE_GAMEPLAY:
	{
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
				if (brick->isColi && !brick->isAddItem)
				{
					for (int m = 0; m < objects_item.size(); m++)
					{
						Item* item = dynamic_cast<Item*>(objects_item.at(m));
						if (item->id == brick->iditem)
						{
							if (item->type == 0)
							{
								item->SetSpeed(0, -0.3);
								//item->SetPosition(brick->x, brick->y);

							}
							if (item->type == 1)item->SetSpeed(0, -0.01);
							if (item->type == 2)
							{
								item->SetPosition(brick->x, brick->y);
								item->SetSpeed(0, -SPEED_ITEM_Y);
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
				if (brick->type == BRICK5 && brick->isP && !brick->isAddItem)
				{

					SwitchBlock* s = new SwitchBlock(brick->x, brick->y - 16);
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
		//update cammera
		cam->Update(isCameraAutoMove,cxcount);
		//update board
		float cx, cy;
		cx = Camera::GetInstance()->GetCameraPosition().x;
		cy=Camera::GetInstance()->GetCameraPosition().y;
		if (board != NULL)
			board->SetPosition((int)cx + BROAD_X, (int)cy + BROAD_Y);
		//update sau khi hoan thanh 1 scence
		if (player->isfinishscence)
		{
			CGame::GetInstance()->SwitchScene(SCENCE_ID_MAP);
			player->isfinishscence = false;
		}
		break;
	}
	default:
		break;
	}
}

void CPlayScene::Render()
{
	switch (typeScence)
	{
	case SCENCE_TYPE_INTRO:
	{
		CSprites* sprites = CSprites::GetInstance();
		if (pace < height)
		{
			int y = height - pace;
			for (; y >= MAN_INTRO_X_MIN; y = y + MAN_INTRO_X_MIN)
			{
				sprites->Get(MAN_SPRITE_ID)->Draw(0, y);
			}
			sprites->Get(MAN_SPRITE_ID2)->Draw(0, height - pace);
			pace = pace + MAN_YUPDATE;
			objects.at(0)->Render();
			return;

		}
		else
		{
			isCanS = true;
		}
		sprites->Get(MAN_SPRITE_ID3)->Draw(0, 0);
		for (int i = 0; i < objects.size(); i++)
		{
			objects.at(i)->Render();
		}
	}
		break;
	case SCENCE_TYPE_WORLDMAP:
	{
		CSprites* sprites = CSprites::GetInstance();
		sprites->Get(MAN_SPRITE_MAP)->Draw(WORLDMAP_X, WORLDMAN_Y);
		board->Render(playermap, TIME_MAX - (GetTickCount64() - time_start) / 1000);
		if (playermap)
		{
			playermap->Render();
		}
		for (int i = 0; i < listscenceselect.size(); i++)
		{
			listscenceselect.at(i)->Render();
		}
	}
		break;
	case SCENCE_TYPE_GAMEPLAY:
	{
		map->DrawMap();
		grid->GetListOfObjects(&objects);
		vector<LPGAMEOBJECT> listobject;
		vector<LPGAMEOBJECT> listitem;
		vector<LPGAMEOBJECT> listPipe;
		vector<LPGAMEOBJECT> listPlant;
		vector<LPGAMEOBJECT> listbrick;

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
					else if (dynamic_cast<QuestionBrick*>(objects.at(i)))
						listbrick.push_back(objects.at(i));
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
		for (int i = 0; i < listbrick.size(); i++)
		{
			listbrick[i]->Render();
		}
		for (int i = 0; i < listPlant.size(); i++)
		{
			listPlant[i]->Render();
		}

		if (player)
		{
			player->Render();
			for (int i = 0; i < player->listball.size(); i++)
				player->listball.at(i)->Render();
		}
		for (int i = 0; i < listPipe.size(); i++)
		{
			listPipe[i]->Render();
		}
		if (player->iscard)
		{
			string1->Draw(string1->x, string1->y, s1);
			string2->Draw(string2->x, string2->y, s2);
			card->Draw(card->x, card->y, player->typecard);
		}

		board->Render(player, TIME_MAX - (GetTickCount64() - time_start) / 1000);
		break;
	}
	default:
		break;
	}

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
	CGame* game = CGame::GetInstance();
	CMario* mario = ((CPlayScene*)scence)->GetPlayer();
	CMario *mariomap = ((CPlayScene*)scence)->GetPlayerMap();
	SelectPlayer *select= ((CPlayScene*)scence)->GetSelectPlayer();
	switch (((CPlayScene*)scence)->typeScence)
	{
	case SCENCE_TYPE_INTRO:
		switch (KeyCode)
		{
		case DIK_DOWN:
			select->select++;
			break;
		case DIK_UP:
			select->select--;
			break;
		case DIK_S:
			if (((CPlayScene*)scence)->isCanS)
				CGame::GetInstance()->SwitchScene(1);
			break;
		}
		break;
	case SCENCE_TYPE_WORLDMAP:
		if (!(!mariomap->autoleft && !mariomap->autoright && !mariomap->autodown && !mariomap->autoup))
			return;
		switch (KeyCode)
		{
		case DIK_LEFT:
			if (!mariomap->autoleft && mariomap->canleft)
			{
				mariomap->autoleft = true;
				mariomap->x_star = mariomap->x;
			}
			mariomap->autoright = false;
			mariomap->autodown = false;
			mariomap->autoup = false;
			break;
		case DIK_RIGHT:
			mariomap->autoleft = false;
			if (!mariomap->autoright && mariomap->canright)
			{
				mariomap->autoright = true;
				mariomap->x_star = mariomap->x;

			}
			mariomap->autodown = false;
			mariomap->autoup = false;
			break;
		case DIK_UP:
			mariomap->autoleft = false;
			mariomap->autoright = false;
			mariomap->autodown = false;
			if (!mariomap->autoup && mariomap->canup)
			{
				mariomap->autoup = true;
				mariomap->y_start = mariomap->y;
			}
			break;
		case DIK_DOWN:
			mariomap->autoleft = false;
			mariomap->autoright = false;
			if (!mariomap->autodown && mariomap->candown)
			{
				mariomap->autodown = true;
				mariomap->y_start = mariomap->y;
			}
			mariomap->autoup = false;
			break;
		case DIK_S:
			if (mariomap->scenceselect > SCENCE_ID_MAP)
			{
				CGame::GetInstance()->SwitchScene(mariomap->scenceselect);
			}
			break;
		default:
			break;
		}
		break;
	case SCENCE_TYPE_GAMEPLAY:
		if (mario->GetState() == MARIO_STATE_PIPE)
			return;
		switch (KeyCode)
		{
		case DIK_N:
			mario->SetPosition(2200, 50);
			break;
		case DIK_M:
			mario->SetPosition(1945, 50);
			break;
		case DIK_DOWN:
			if (mario->isCanSwitchScenceDown)
			{
				mario->SetState(MARIO_STATE_PIPE);
				if (mario->GetLevel() == MARIO_LEVEL_SMALL)
					mario->SetSpeed(0, MARIOSAMLL_SPEED_Y_PIPE);
				else
					mario->SetSpeed(0, MARIOSUPPER_SPEED_Y_PIPE);

			}
			break;
		case DIK_UP:
			if (mario->isCanSwitchScenceUP)
			{
				mario->SetState(MARIO_STATE_PIPE);
				if (mario->GetLevel() == MARIO_LEVEL_SMALL)
					mario->SetSpeed(0, -MARIOSAMLL_SPEED_Y_PIPE);
				else
					mario->SetSpeed(0, -MARIOSUPPER_SPEED_Y_PIPE);

			}
			break;
		case DIK_X:
			if (mario->isJump || mario->isjumpX)
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
			if (mario->isJump && !mario->isBonusvy && mario->GetLevel() == MARIO_LEVEL_RACCOON)
			{
				mario->isfly = true;
				mario->SetSpeed(mario->vx, -MARIO_SPEED_Y_FLY);
			}
			break;
		case DIK_A:
			//add ball
			if (mario->GetLevel() == MARIO_LEVEL_FIRE)
			{
				if (!mario->isthrow)
				{
					if (mario->listball.size() < MAX_BALL)
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
		case DIK_P:
			mario->SetLevel(MARIO_LEVEL_BIG);
			break;
		}
		break;
	default:
		break;
	}
}

void CPlayScenceKeyHandler::OnKeyUp(int KeyCode) 
{
	CGame* game = CGame::GetInstance();
	CMario* mario = ((CPlayScene*)scence)->GetPlayer();

	if (((CPlayScene*)scence)->typeScence == SCENCE_TYPE_GAMEPLAY)
	{
		switch (KeyCode)
		{
		case DIK_DOWN:
			mario->isDuck = false;
			break;
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
}

void CPlayScenceKeyHandler::KeyState(BYTE *states)
{
	CGame *game = CGame::GetInstance();
	CMario *mario = ((CPlayScene*)scence)->GetPlayer();

	// disable control key when Mario die
	if (((CPlayScene*)scence)->typeScence == SCENCE_TYPE_GAMEPLAY)
	{
		if (mario->GetState() == MARIO_STATE_DIE) return;
		if (mario->GetState() == MARIO_STATE_PIPE) return;

		if (game->IsKeyDown(DIK_RIGHT) && !mario->iswag)
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
		else if (game->IsKeyDown(DIK_LEFT) && !mario->iswag)
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
				if (mario->GetState() == MARIO_STATE_WALKING_LEFT)
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
		if (!((game->IsKeyDown(DIK_LEFT) || game->IsKeyDown(DIK_RIGHT)) && game->IsKeyDown(DIK_A)))
			mario->DecreasePower();
		//nut A
		if (game->IsKeyDown(DIK_A))
		{
			mario->iscanHold = true;
			if (mario->isColiWithKoopas)
			{
				if (!mario->isHold)
				{
					mario->isHold = true;
					mario->hold_start = GetTickCount64();
				}
				mario->rua->ishold = true;
			}
		}
		else
		{
			mario->iscanHold = false;
			mario->isHold = false;
		}
		if ((game->IsKeyDown(DIK_LEFT) || !game->IsKeyDown(DIK_RIGHT)) && game->IsKeyDown(DIK_DOWN))
			mario->isDuck = true;
	}
}