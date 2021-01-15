#include "Font.h"
#include "Sprites.h"
#include "Textures.h"


Font::Font()
{
}

Font::Font(float x, float y)
{
	this->x = x;
	this->y = y;
}

Font::Font(float x, float y, std::string s)
{
	this->x = x;
	this->y = y;
	this->s = s;
}

Font::~Font()
{
}

void Font::Draw(float x, float y, std::string s)
{
	this->x = x;
	this->y = y;
	this->s = s;
	CSprites* sprites = CSprites::GetInstance();

	for (size_t i = 0; i < s.size(); i++)
	{
		if (s[i] == ' ')
		{
			continue;
		}
		if (s[i] >= 'A' && s[i] <= 'Z')
		{
			sprites->Get(40017 +( s[i] - 'A'))->Draw(x+i*8, y);
		}
		else
			if (s[i] >= '0' && s[i] <= '9')
			{
				sprites->Get(40007 +(s[i] - '0'))->Draw(x + i * 8, y);
			}
	}
}

void Font::Draw(float x, float y, int type)
{
	this->x = x;
	this->y = y;
	this->s = s;
	CSprites* sprites = CSprites::GetInstance();
	sprites->Get(40044 + type)->Draw(x , y);
}
