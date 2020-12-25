#pragma once
#include <string>
#include <string>

class Font
{
	std::string s;
	float x, y;
public:
	Font();
	Font(float x, float y, std::string s);
	~Font();
	void Draw(float x, float y, std::string s);
};

