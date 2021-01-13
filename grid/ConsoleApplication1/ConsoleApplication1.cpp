// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<iostream>
#include<fstream>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>

using namespace std;

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
#define OBJECT_TYPE_BRICKMOVE	12

#define OBJECT_TYPE_PORTAL	50

#define MAX_SCENE_LINE 1024

#define SCREEN_WIDTH 260
#define SCREEN_HEIGHT 240

vector<string> split(string line, string delimeter = "\t")
{
	vector<string> tokens;
	size_t last = 0; size_t next = 0;
	while ((next = line.find(delimeter, last)) != string::npos)
	{
		tokens.push_back(line.substr(last, next - last));
		last = next + 1;
	}
	tokens.push_back(line.substr(last));

	return tokens;
}

/*
char * string to wchar_t* string.
*/
wstring ToWSTR(string st)
{
	const char* str = st.c_str();

	size_t newsize = strlen(str) + 1;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, str, _TRUNCATE);

	wstring wstr(wcstring);

	// delete wcstring   // << can I ? 
	return wstr;
}

/*
	Convert char* string to wchar_t* string.
*/
LPCWSTR ToLPCWSTR(string st)
{
	const char* str = st.c_str();

	size_t newsize = strlen(str) + 1;
	wchar_t* wcstring = new wchar_t[newsize];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wcstring, newsize, str, _TRUNCATE);

	wstring* w = new wstring(wcstring);

	// delete wcstring   // << can I ? 
	return w->c_str();
}
string convert_int_tostring(int x)
{
	stringstream ss;
	ss << x;
	string s;
	ss >> s;
	return s;
}
/*
	Parse a line in section [OBJECTS]
*/
string ParseSection_OBJECTS(string line)
{
	std::cout << "doc object\n";
	string lineresult;
	vector<string> tokensresult;
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return line; // skip invalid lines - an object set must have at least id, x, y

	int object_type = atoi(tokens[0].c_str());
	float x = atof(tokens[1].c_str());
	float y = atof(tokens[2].c_str());
	int cellwidth = (int)SCREEN_WIDTH / 2;
	int cellheight = (int)SCREEN_HEIGHT / 2;
	int cellgrid = floor(x / cellwidth);
	int rowgrid = floor(y / cellheight);
	
	for (int i = 0; i <=3; i++)
		tokensresult.push_back(tokens.at(i));

	tokensresult.push_back(convert_int_tostring(cellgrid));
	tokensresult.push_back(convert_int_tostring(rowgrid));

	for(int i=4;i< tokens.size();i++)
		tokensresult.push_back(tokens.at(i));

	for (int i = 0; i < tokensresult.size(); i++)
	{
		lineresult = lineresult + tokensresult.at(i);
		if (i < tokensresult.size() - 1)
			lineresult = lineresult + "	";
	}
	return lineresult;
}



void Docfile(LPCWSTR sceneFilePath, LPCWSTR sceneFilePathgrid)
{
	ifstream f;
	f.open(sceneFilePath);

	ofstream fg(sceneFilePathgrid);
	fg << "#Day la file demo su dung cach doc va ghi file su dung fstream\n";

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);
		if (line == "[OBJECTS]") {
			section = SCENE_SECTION_OBJECTS; 
			fg << (line + "\n");
			continue;
		}
		switch (section)
		{
		case SCENE_SECTION_OBJECTS:
			fg << (ParseSection_OBJECTS(line)+ "\n");
			break;
		default:
			fg << (line + "\n");

		}
	}

	f.close();
	fg.close();

}

void GhiFile(LPCWSTR data_file)
{
	ofstream FileDemo(data_file);
	FileDemo << "#Day la file demo su dung cach doc va ghi file su dung fstream";
	FileDemo.close();
}
int main()
{
	//LPCWSTR st = L"scene0.txt";
	//LPCWSTR stgrid = L"scene0-grid.txt";
	Docfile(L"scene0.txt", L"scene0-grid.txt");
	Docfile(L"scene1-1.txt", L"scene1-1-grid.txt");
	Docfile(L"scene1-2.txt", L"scene1-2-grid.txt");
	Docfile(L"scene3-1.txt", L"scene3-1-grid.txt");
	Docfile(L"scene3-2.txt", L"scene3-2-grid.txt");
	Docfile(L"scene-intro.txt", L"scene-intro-grid.txt");
	Docfile(L"scene-test.txt", L"scene-test-grid.txt");

	//Docfile(st, stgrid);
	//GhiFile(stgrid);
	return 0;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
