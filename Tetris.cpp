#include <iostream>
#include <stdio.h>
//#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <string.h>

using namespace std;

enum Direction { UP = 72, DOWN = 80, LEFT = 75, RIGHT = 77, SPACE = 32, ESC = 27, ENTER = 13 };
enum ConsoleColor {
	Black = 0, Blue = 1, Green = 2, Cyan = 3, Red = 4, Magenta = 5, Brown = 6,
	LightGray = 7, DarkGray = 8, LightBlue = 9, LightGreen = 10, LightCyan = 11, LightRed = 12,
	LightMagenta = 13, Yellow = 14, White = 15
};

void Zastavka();
void Pole(int color);
void print();
int rnd(int max);
void printnextmap();
int getkey();
void SetCur(int xpos, int ypos);
int valnewpos(int x, int y);
void rotatemap();
void sleep(int milsec);
void deleteline();
void createmap();
void clearscreen();
void startgame();
int Menu();
void How_to_play();
void SetColor(int color);
void CursorOpasity();
void Options();
void OptionsElements();

#define WIDTH 14      // ширина стакана
#define HEIGHT 16     // высота стакана
#define FIGURES 7     // количество фигур

#define SCR_SP  32       // символ "пустоты"
#define SYM_FIGURES  177 // символ "закрашенного места"

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

int screen[WIDTH][HEIGHT] = { 0 };
int map[4][4];
int px, py, score, nextmap;

bool zast = false; // переменная определяет показ заставки только один раз
int color_figures = White;


int fmap[FIGURES][4][4] =
{
	{
		{ 1, 1, 0, 0 },
		{ 1, 1, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	},
	{
		{ 1, 0, 0, 0 },
		{ 1, 0, 0, 0 },
		{ 1, 0, 0, 0 },
		{ 1, 0, 0, 0 }
	},
	{
		{ 0, 0, 1, 0 },
		{ 1, 1, 1, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	},
	{
		{ 1, 1, 1, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	},
	{
		{ 0, 1, 1, 0 },
		{ 1, 1, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	},
	{
		{ 1, 1, 0, 0 },
		{ 0, 1, 1, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	},
	{
		{ 1, 0, 0, 0 },
		{ 1, 1, 0, 0 },
		{ 1, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	}
};

int main()
{
	srand(time(0));
	system("title TETRIS"); // название игры в шапке консоли
	system("mode con cols=40 lines=20"); // размеры консоли
	system("cls");
	if (zast == false) // если первый вход - показываем заставку
		Zastavka();
	switch (Menu())
	{
	case 1:
		startgame();
		break;
	case 2:
		How_to_play();
		break;
	case 3:
		Options();
		break;
	}


}

void Pole(int color)  // функция прорисовки "колбы"
{
	CursorOpasity();
	SetColor(color); // цвет рамки
	short startPosX = 0;
	short startPosY = 0;
	SetCur(startPosX, startPosY);

	for (int i = 0; i <= HEIGHT + 1; i++)
	{
		for (int j = 0; j <= WIDTH; j++)
		{
			if (i == HEIGHT + 1 || j == 0 || j == WIDTH)
				cout << char(219);
			else
				cout << " ";
		}
		cout << endl;
		SetCur(startPosX, startPosY++);
	}
	startPosY = 0;
}

void print()
{
	SetColor(color_figures);
	int i, j;
	int buff[WIDTH][HEIGHT];

	for (i = 0; i < HEIGHT; i++)
		for (j = 0; j < WIDTH; j++)
			buff[j][i] = screen[j][i];

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (map[j][i])
				buff[j + px][i + py] = 1;

	SetCur(0, 0);
	for (i = 0; i < HEIGHT; i++)
	{
		for (j = 0; j < WIDTH; j++)
		{
			putchar(buff[j][i] == 0 ? SCR_SP : SYM_FIGURES);
			//cout << (buff[j][i] == 0) ? SCR_SP : SYM_FIGURES; // тетрис в "бинарном" режиме :)
		}
		cout << endl;
	}

	SetCur(WIDTH + 3, 1);
	cout << "Score: " << score;
}
void printnextmap()
{
	int i, j;

	SetCur(WIDTH + 3, 4);
	cout << "Next figure: ";

	for (i = 0; i < 4; i++)
	{
		SetCur(WIDTH + 6, i + 7);
		for (j = 0; j < 4; j++)
		{
			putchar(fmap[nextmap][j][i] == 0 ? SCR_SP : SYM_FIGURES);
		}
	}
}

int getkey()
{
	int c;
	if (_kbhit())
	{
		if ((c = _getch()) == 224) c = _getch();
		return c;
	}
	return 0;
}

void SetCur(int xpos, int ypos)
{
	COORD point;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	point.X = xpos; point.Y = ypos;
	SetConsoleCursorPosition(h, point);
}

int valnewpos(int x, int y)
{
	int i, j;
	if (x < 0) return 0;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (map[j][i])
			{
				if ((j + x >= WIDTH) || (i + y >= HEIGHT))
					return 0;
				if (screen[j + x][i + y])
					return 0;
			}
		}
	}
	return 1;
}

void rotatemap()
{
#define inv(x) ((x * (-1)) + 3)

	int _map[4][4];
	int i, j, sx = 4, sy = 4;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
		{
			_map[j][i] = map[j][i];
			if (map[j][i])
			{
				if (i < sx) sx = i;
				if (inv(j) < sy) sy = inv(j);
			}
			map[j][i] = 0;
		}

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (_map[inv(i)][j])    map[j - sx][i - sy] = 1;

	if (!valnewpos(px, py)) for (i = 0; i < 4; i++) for (j = 0; j < 4; j++) map[j][i] = _map[j][i];
}

int rnd(int max)
{
	max++;
	return (int)(rand() * max / RAND_MAX);
}

void sleep(int milsec)
{
	clock_t t = clock();
	while (clock() - t < milsec);
}

void deleteline()
{
	int i, j, k, cl;

	for (i = HEIGHT - 1; i >= 0; i--)
	{
		cl = 1;
		for (j = 0, cl = 1; j < WIDTH; j++)
		{
			if (!screen[j][i]) cl = 0;
		}
		if (cl)
		{
			//   Анимация удаления строки
			SetCur(0, i);
			for (k = 0; k < WIDTH; k++)
				putchar('>'), sleep(40);
			//

			score += (((i * (-1)) + HEIGHT) * 10);

			for (k = i; k > 0; k--)
			{
				for (j = 0; j < WIDTH; j++)
				{
					screen[j][k] = screen[j][k - 1];
				}
			}
			i++;
			print();
		}
	}
}

void createmap()
{
	int i, j, rn = 0;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			map[j][i] = fmap[nextmap][j][i];
	py = 0;
	px = WIDTH / 2;

	nextmap = rnd(FIGURES - 1);
	printnextmap();
}

void clearscreen()
{
	int i, j;
	for (i = 0; i < HEIGHT; i++)
		for (j = 0; j < WIDTH; j++)
			screen[j][i] = 0;
}

void startgame()
{
	system("cls");
	CursorOpasity();
	SetColor(color_figures);

	int i, j, c;
	time_t tm;

	px = WIDTH / 2;
	py = 0;
	score = 0;

	tm = clock();

	nextmap = rnd(FIGURES - 1);
	createmap();

	Pole(White); // прорисовываем границу стакана белым цветом

	while (1)
	{
		c = getkey();
		switch (c)
		{
		case UP:
		case SPACE:
			rotatemap();
			break;
		case DOWN:
			for (; valnewpos(px, py + 1); py++);
			for (i = 0; i < 4; i++)
				for (j = 0; j < 4; j++)
					if (map[j][i])
						screen[px + j][py + i] = 1;

			print();
			deleteline();
			createmap();
			break;
		case LEFT:
			if (valnewpos(px - 1, py))
				px--;
			break;
		case RIGHT:
			if (valnewpos(px + 1, py))
				px++;
			break;
		case 'p':             // Пауза
		case ENTER:
			_getch();
			break;
		case ESC:
			Sleep(2000);
			main();
		}

		if ((clock() - tm) > 450)        // скорость произвольного падения
		{
			tm = clock();

			if (!(valnewpos(px, py + 1)))
			{
				for (i = 0; i < 4; i++)
					for (j = 0; j < 4; j++)
						if (map[j][i])
							screen[px + j][py + i] = 1;

				createmap();
				deleteline();
			}
			else py++;
		}
		print();
		for (i = 0; i < WIDTH; i++)
		{
			if (screen[i][0])
			{
				SetCur(WIDTH / 2 - 5, 8);
				cout << "Game Over!";
				SetCur(0, HEIGHT + 2);
				cout << "You lose! Press ESC to Menu...\n";
				while (_getch() != ESC);
				{
					system("cls");
					main();
				}
			}
		}
	}
}

void Zastavka() // название в начале игры
{
	CursorOpasity();
	SetColor(Green);

	for (int i = 0; i < 3; i++)
	{
		if (i == 0)
			SetColor(Red);
		if (i == 1)
			SetColor(Yellow);
		if (i == 2)
			SetColor(Green);
		SetCur(0, 6);
		cout << R"X(
   $$$$$ $$$$ $$$$$ $$$$   $$$  $$$
     $   $      $   $   $   $  $
     $   $$$$   $   $$$$    $   $$$
     $   $      $   $   $   $      $
     $   $$$$   $   $   $  $$$  $$$)X";
		Sleep(500);
	}
	Sleep(1000);
	system("cls");
}

int Menu()
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	CursorOpasity(); //Прозрачность (удаление) курсора
	SetColor(Green);

	short x = 10;
	short y = 7;
	COORD point{ x, y };
	int choise = -1;
	for (int i = 0; i < 5; i++)  // цикл прорисовывает текст меню
	{
		SetCur(x + 2, y + i);
		if (i == 0)
			cout << "Start new game\n";
		if (i == 1)
			cout << "How to play\n";
		if (i == 2)
			cout << "Options\n";
		if (i == 3)
			cout << "Exit\n";
	}
	SetConsoleCursorPosition(h, point);
	while (1)
	{
		cout << char(62);
		SetConsoleCursorPosition(h, point);
		int code = _getch();
		if (code == 224)  // выбор для стрелок ВВЕРХ и ВНИЗ
		{
			SetConsoleCursorPosition(h, point);
			cout << ' ';
			code = _getch();
			switch (code)
			{
			case UP:
				if (point.Y < y + 1)
					break;
				point.Y--;
				break;
			case DOWN:
				if (point.Y > y + 2)
					break;
				point.Y++;
				break;

			default:
				break;
			}
			SetConsoleCursorPosition(h, point);
		}

		else if (code == ENTER)      // для ENTER
		{
			if (point.Y == y)        // START
			{
				clearscreen();
				zast = true;
				return 1;
			}
			else if (point.Y == y + 1)
			{
				system("cls");
				zast = true;
				return 2;
			}
			else if (point.Y == y + 2)
			{
				system("cls");
				zast = true;
				return 3;
			}
			else if (point.Y == y + 3)
				exit(0);
		}
		else if (code == ESC)
			exit(0);
	}
	return 0;
}

void How_to_play()  // Информация "как играть"
{
	cout << R"X(


		   LEFT arrow   	Left
   RIGHT arrow  	Righ
   DOWN arrow   	Put Down
   SPACE    		Rotate
   P	    		Pause
   ESC	    		Exit


				
   Press SPACE to MENU or ESC to EXIT)X";
	int code = 0;
	while (code != ESC || code != ENTER || code != SPACE)
	{
		code = _getch();
		switch (code)
		{
		case ESC:
			exit(0);
			break;
		case ENTER:
		case SPACE:
			system("cls");
			main();
			break;
		}
	}
}

void SetColor(int color)
{
	SetConsoleTextAttribute(h, color | FOREGROUND_INTENSITY);
}

void CursorOpasity()
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = false;
	cursor.dwSize = 100;
	SetConsoleCursorInfo(h, &cursor);
}

void Options()
{
	SetCur(9, 2);
	cout << "Set colour of figures:" << endl << endl;
	for (int i = 0; i < 8; i++)
	{
		SetCur(7, 5 + i);
		if (i == 0)
		{
			SetColor(White);
			OptionsElements();
		}
		else if (i == 1)
		{
			SetColor(DarkGray);
			OptionsElements();
		}
		else if (i == 2)
		{
			SetColor(Cyan);
			OptionsElements();
		}
		else if (i == 3)
		{
			SetColor(Red);
			OptionsElements();
		}
		else if (i == 4)
		{
			SetColor(Magenta);
			OptionsElements();
		}
		else if (i == 5)
		{
			SetColor(Green);
			OptionsElements();
		}
		else if (i == 6)
		{
			SetColor(Yellow);
			OptionsElements();
		}
		else if (i == 7)
		{
			SetColor(Blue);
			OptionsElements();
		}
	}

	short x = 5, y = 5;
	COORD point{ x, y };
	SetConsoleCursorPosition(h, point);
	while (1)
	{
		SetColor(White);
		cout << char(62);
		int code = _getch();
		if (code == 224)  // выбор для стрелок ВВЕРХ и ВНИЗ
		{
			SetConsoleCursorPosition(h, point);
			cout << ' ';
			code = _getch();
			switch (code)
			{
			case UP:
				if (point.Y < 6)
					break;
				point.Y--;
				break;
			case DOWN:
				if (point.Y > 11)
					break;
				point.Y++;
				break;
			}
			SetConsoleCursorPosition(h, point);
		}

		else if (code == ENTER)      // для ENTER
		{
			if (point.Y == y)
			{
				color_figures = White;
				system("cls");
				main();
			}

			else if (point.Y == y + 1)
			{
				color_figures = DarkGray;
				system("cls");
				main();
			}

			else if (point.Y == y + 2)
			{
				color_figures = Cyan;
				system("cls");
				main();
			}

			else if (point.Y == y + 3)
			{
				color_figures = Red;
				system("cls");
				main();
			}

			else if (point.Y == y + 4)
			{
				color_figures = Magenta;
				system("cls");
				main();
			}

			else if (point.Y == y + 5)
			{
				color_figures = Green;
				system("cls");
				main();
			}

			else if (point.Y == y + 6)
			{
				color_figures = Yellow;
				system("cls");
				main();
			}

			else if (point.Y == y + 7)
			{
				color_figures = Blue;
				system("cls");
				main();
			}
		}

		else if (code == ESC)
			main();
	}
}

void OptionsElements()
{
	int u = 0;
	while (u < 5)
	{
		cout << char(219);
		u++;
	}
}