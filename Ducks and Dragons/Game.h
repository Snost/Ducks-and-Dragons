#pragma once
#include <conio.h>
#include "Anotation.h"
#include "Map.h"
#include "Battle.h"
#include "Location.h"

#include "Duck.h"
#include "Dragon.h"
#include "Shrek.h"
#include "Boss.h"

#define ENTER 13
enum Arrows { Arrow = 224, Up = 72, Down = 80, Left = 75, Right = 77 };

class Game
{
public:
	void run() {
		string titles = "DUCKS & DRAGONS";
		Annotation title(titles);
		title.printTitle();

		//printAnnotation(annotationText);

		while (true) {
			printMap();

			//���������� �������
			createLocation();

			Location enteredLocation = mapPlayerMoving();

			system("cls");

			battle.startBattle(player, enteredLocation.enemy);


			if (battle.isFriend() && isPlayed == false) {
				printAnnotation(DragonF);
				isPlayed = true;
			}
			if (battle.isDead()) {
				if (battle.isFriend())
					printAnnotation(goodEnd);
				else
					printAnnotation(badEnd);
				return;
			}
		}
	}

	Game() {
		//̳��� ����� ����
		SetConsoleTitleA("Ducks & Dragons");

		//�� �������� �������� ����� ������
		HWND hwndConsole = GetConsoleWindow();
		LONG style = GetWindowLong(hwndConsole, GWL_STYLE);
		style &= ~WS_THICKFRAME; // �������� WS_THICKFRAME
		SetWindowLong(hwndConsole, GWL_STYLE, style);

		//��������� ������
		cursorInfo.dwSize = 1;
		cursorInfo.bVisible = false;
		SetConsoleCursorInfo(hConsole, &cursorInfo);

		//�������� ��� ��� ����� ������
		GetConsoleScreenBufferInfo(hConsole, &screenInfo);
	}

private:

#pragma region MapService

	void printMap() {
		vector<string> route = map.getRoute();

		for (size_t i = 0; i < route.size(); ++i) {
			COORD position = map.getIndents();
			position.Y += (short)i;

			for (size_t j = 0; j < route[0].size(); ++j) {
				position.X++;
				SetConsoleCursorPosition(hConsole, position);
				cout << route[i][j];
			}
		}
	}

	Location mapPlayerMoving() {
		vector<string> route = map.getRoute();

		//���������� ����� ������
		static COORD playerPos = map.getStartPos();

		//³������ ����� �� ������� � ������ �����
		COORD indents = map.getIndents();

		//���������� ������ ���������� �������
		COORD outputPos;
		outputPos.Y = indents.Y + playerPos.Y;
		outputPos.X = indents.X + playerPos.X + 1;

		const char player = '*';
		SetConsoleCursorPosition(hConsole, outputPos);
		cout << player;

		bool isAtLocation = false;
		while (true) {
			Location curLocation = getLocation(playerPos);
			if (!curLocation.name.empty()) {
				isAtLocation = true;
				showLocationName(curLocation.name);
			}

			if (_kbhit()) {
				unsigned char ch = _getch();

				if (ch == Arrow) {
					ch = _getch();

					SetConsoleCursorPosition(hConsole, outputPos);
					cout << route[playerPos.Y][playerPos.X];

					COORD previousPos = playerPos;
					if (ch == Up && route[playerPos.Y - 1][playerPos.X] != ' ') {
						playerPos.Y--;
						outputPos.Y--;
					}
					else if (ch == Down && route[playerPos.Y + 1][playerPos.X] != ' ') {
						playerPos.Y++;
						outputPos.Y++;
					}
					else if (ch == Left && route[playerPos.Y][playerPos.X - 1] != ' ') {
						playerPos.X--;
						outputPos.X--;
					}
					else if (ch == Right && route[playerPos.Y][playerPos.X + 1] != ' ') {
						playerPos.X++;
						outputPos.X++;
					}

					SetConsoleCursorPosition(hConsole, outputPos);
					cout << player;

					if (previousPos.Y != playerPos.Y || previousPos.X != playerPos.X) {
						isAtLocation = false;
						for (int i = 0; i < curLocation.name.size(); i++)
							curLocation.name[i] = ' ';
						showLocationName(curLocation.name);
					}
				}
				else if (ch == ENTER && isAtLocation) {
					return curLocation;
				}
			}
		}
	}

	const Location& getLocation(COORD pos) {
		//�������� ����� �������, ���� ����� �� ��
		for (auto i : locations)
			if (pos.X == i->coord.X && pos.Y == i->coord.Y) {
				pos.Y = 1;
				pos.X = (screenInfo.srWindow.Right - i->name.size()) / 2;
				return *i;
			}
		return Location(string(), COORD(), nullptr);
	}

	void showLocationName(const string& locName) {
		COORD namePos;
		namePos.Y = 1;
		namePos.X = (screenInfo.srWindow.Right - locName.size()) / 2;

		SetConsoleCursorPosition(hConsole, namePos);
		cout << locName;
	}

#pragma endregion

#pragma region LocationService

	void createLocation() {
		vector<COORD> locationCoords = map.getLocationCoords();

		if (!locations.empty())
			locations.clear();

		locations.push_back(new Location("Castle", locationCoords[0], new Boss()));
		locations.push_back(new Location("Field", locationCoords[1], new Duck()));
		locations.push_back(new Location("Swamp", locationCoords[2], new Ogre()));
		if (battle.isFriend()) {
			locations.push_back(new Location("Mountain", locationCoords[3], new Ogre()));
		}
		else
			locations.push_back(new Location("Mountain", locationCoords[3], new Dragon()));
	}

	void printAnnotation(const string& text) {
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);

		Annotation annotation(text);
		annotation.printStar();
		annotation.printAnnotation();

		SetConsoleCP(437);
		SetConsoleOutputCP(437);

		system("cls");
	}

#pragma endregion 

	bool isPlayed = false;

	vector<Location*> locations;

	Player player;

	Battle battle;

	Map map;

	string goodEnd = "ϳ��� ������ ���� � ������ ����������� ��� ����� � ���� ��������� �������� ��������.~�� ��������� �����, �� �� ������ � ���� �������, �� ��� �������� �� ���� �� ������~� ���������, ���������� ����� ����� ���������� �� �������.�����������, �� ��~������� ������� ����� ����������, ��� ������� ���� ������� ����������.���������,~�� �� � �������� ���������� ����� ��������� ����������, � ���� ���� �� ���� ����~���������.������� ������� � ������� �� ���� ������������ �� ������ ����������,~���� ���������� ���������� � ������������ ����� ������.~~~\t\t\t\t\tHAPPY END!";
	string badEnd = "��� � ������ �������, ����� ������ ���� ����� ������� �� ���� ��쳿. ³� �����������~������ ���� �������, ��� ��������� ������ � ��������� ���������� �� ��������. �����~�� �������� ��������� �� ������� �����. ���������� ���������� ��� ����� �������������~��������� - �������, � ��� ������ ����� �� ����� ������. ���� ����� ����������~����'� �� ������.���, ��������� ���� �� ����� �� ��� 䳿, ������������� � �������~�������, ���������� �� ����� ���������� �� ���������� ���� � ����� ���������.~³� ������� ����� �� ������ ������������� �� ����������,~����������� ��������� ��� ������� � �������� ����� ������� ��� ���.~~~\t\t\t\t\tEND!";
	string annotationText = "̳� ���� ������������ �� ������ - ����� ������� � ��������� ����'������ ��������.~�, ���, ������� �������, �������� � ������ ����� �������������� ����������.~��� �� ����� ��������, ���� �� ��� ��������� � �������� �� ����� ������ � ��������~������� ������ ���������� �� ��� ��'��.�� �������� ��������� ����, � � �����,~�� �� �� ������ �����������. ֳ������ �� ������� �������� ������� ����������� ���� ~�� ���������� �������, ��� �'�������, �� ������� �� ���� ����������� �������.~��� ����������� ��� ������� � �������, � ������� �������� �������� � �������,~�� ������� �� ���� � ���������.";
	string DragonF = "������ ������� ������.��� ����������� ������������� � ��������.";

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;
	CONSOLE_CURSOR_INFO cursorInfo;
};

