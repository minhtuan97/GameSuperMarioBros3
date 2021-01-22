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
#include <atlstr.h>


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
#define MAX_GAME_LINE 1024

#define SCREEN_WIDTH 260
#define SCREEN_HEIGHT 240

#define GAME_FILE_SECTION_UNKNOWN -1
#define GAME_FILE_SECTION_SETTINGS 1
#define GAME_FILE_SECTION_SCENES 2

typedef long long long64;


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

string convLPCWSTRtoString(LPCWSTR wString)
{
	int size = wcslen(wString);
	char* cString = new char[size];
	for (int i = 0; i < size; i++)
	{
		cString[i] = wString[i];
	}
	string String(cString);
	string st;
	for (int i = 0; i < size; i++)
		st = st + String[i];
	return st;
}
string insertGrid(string str1)
{
	int  i = str1.size() - 4;
	str1.insert(i, "-grid");
	return str1;
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
void _ParseSection_SETTINGS(string line)
{

}
string _ParseSection_SCENES(string line)
{
	string lineresult;
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return lineresult;
	int id = atoi(tokens[0].c_str());
	LPCWSTR path = ToLPCWSTR(tokens[1]);
	string newpath = insertGrid(tokens[1]);
	lineresult = tokens[0] + '\t' + insertGrid(tokens[1])+'\n';

	//doc secence
	Docfile(path, ToLPCWSTR(newpath));
	return lineresult;
}

void Doc(LPCWSTR data_file)
{
	ifstream f;
	f.open(data_file);

	string newfilegrid = convLPCWSTRtoString(data_file);
	newfilegrid = insertGrid(newfilegrid);
	//std::cout << newfile;
	ofstream fg(newfilegrid);
	char str[MAX_SCENE_LINE];
	// current resource section flag
	int section = GAME_FILE_SECTION_UNKNOWN;

	while (f.getline(str, MAX_GAME_LINE))
	{
		string line(str);

		if (line[0] == '#') 
		{
			fg << (line + "\n");
			continue;
		}	// skip comment lines	

		if (line == "[SETTINGS]") { fg << (line + "\n"); section = GAME_FILE_SECTION_SETTINGS; continue; }
		if (line == "[SCENES]") { fg << (line + "\n"); section = GAME_FILE_SECTION_SCENES; continue; }

		//
		// data section
		//
		switch (section)
		{
		case GAME_FILE_SECTION_SETTINGS: fg << (line + "\n"); break;
		case GAME_FILE_SECTION_SCENES:fg<<_ParseSection_SCENES(line); break;
		}
	}
	f.close();
	fg.close();

}

int main()
{
	LPCWSTR st = L"mario-sample.txt";
	Doc(st);
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
