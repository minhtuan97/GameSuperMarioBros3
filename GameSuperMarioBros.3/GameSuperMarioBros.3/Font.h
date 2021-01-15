#pragma once
#include <string>
#include <string>

class Font
{

public:
	std::string s;
	float x, y;
	Font();
	Font(float x, float y);
	Font(float x, float y, std::string s);
	~Font();
	void Draw(float x, float y, std::string s);
	void Draw(float x, float y, int type);
};

