
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <deque>
#include <ctime>
#pragma warning(disable:4996)
using namespace std;
HANDLE hOut;
COORD pos;
enum mapSize { width = 60, height = 30 }; //遊戲地圖
//1.實現gotoxy函式
void gotoxy(short x, short y)
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); //獲取控制代碼
	pos = { x,y };
	SetConsoleCursorPosition(hOut, pos);  //移動游標到指定位置
}
void HideCursor() //隱藏游標
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);//獲取控制檯游標資訊
	CursorInfo.bVisible = false;    //隱藏控制檯游標
	SetConsoleCursorInfo(handle, &CursorInfo);//設定控制檯游標狀態
}
class node;
class SnakeNode;
class Wall;
class Food;
class Space;



class node {
public:
	unsigned short category;
	unsigned short interacts;

	/*
	Snake: 0001
	wall:  0010
	food:  0100
	space: 1000
	*/
};

bool collision(node colliding, node collided) {
	return colliding.interacts & collided.category;
}
node map[height][width];

class Space :public node{
public:
	Space() {
		category = 8;
		interacts = 15;
	}
};

class SnakeNode :public node {
public:
	char body;
	short position_x, position_y;    //蛇的座標
	SnakeNode() {
		category = 1;
		interacts = 12;
	}
};

class Wall :public node {
public:
	Wall() {
		category = 2;
		interacts = 0;
	}
	void draw_Frame()  //畫邊框
	{
		for (int i = 0; i < height; i++)
		{
			gotoxy(0, i);
			cout << "■";
			gotoxy(width, i);
			cout << "■";
		}
		for (int i = 0; i <= width; i += 2) //■ 這個符號佔兩個字元位置，所以是+2
		{
			gotoxy(i, 0);
			cout << "■";
			gotoxy(i, height);
			cout << "■";
		}
	}
};

class Food :public node {
public:
	Food() {
		category = 4;
		interacts = 8;
	}
	short food_x, food_y;      //食物座標
	void draw_Food() //畫食物
	{
		Food food;
		while (1)
		{
			food_x = rand() % (width - 4) + 2; //食物要在地圖中，不能再地圖邊框上，地圖的符號在x方向佔兩個字元位置所以+2，而-4則是減去邊框
			food_y = rand() % (height - 2) + 1; //與上同理
			if (!collision(food, map[food_y][food_x]) && food_x % 2 == 0)
				break;
		}
		gotoxy(food_x, food_y);
		cout << "O";
		map[food_y][food_x] = food;
	}
	void foodEaten() {
		gotoxy(food_x, food_y);
		cout << " "; //食物被吃後要在原來的位置畫空，否則游標會退位，導致邊框錯位
	}
	void judge_eatFood(SnakeNode SnakeHead, bool& eat_Food) //判斷是否吃到食物
	{
		if (food_x == SnakeHead.position_x && food_y == SnakeHead.position_y)
			eat_Food = true;
	}
};


void buildMap() {
	Wall wall;
	Space space;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
				map[i][j] = wall;
			}
			else map[i][j] = space;
		}
	}
}

void buildSnakeInMap(deque<SnakeNode> snake) {
	SnakeNode snode;
	for (auto i : snake) {
		map[i.position_y][i.position_x] = snode;
	}
}


class Snake {
public:
	
	void init_snake(deque<SnakeNode>& snake)  //初始化蛇
	{
		SnakeNode temp = SnakeNode();
		temp.body = '#';
		temp.position_x = width / 2;
		temp.position_y = static_cast<short>(height / 2);
		snake.push_back(temp); //新增蛇頭
		for (int i = 0; i < 3; i++) //蛇的初始身體節數，可自定
		{
			temp = SnakeNode();
			temp.body = 'o';
			temp.position_x = width / 2;
			temp.position_y = static_cast<short>((height / 2) + 1 + i);

			snake.push_back(temp);
		}

		temp = SnakeNode();
		temp.body = ' ';
		temp.position_x = width / 2;
		temp.position_y = static_cast<short>((height / 2) + 4);
		snake.push_back(temp); //新增蛇尾，先放空，以便於後面新增節數時操作
	}
	void draw_Snake(deque<SnakeNode>& snake) //畫蛇
	{
		for (int k = 0; k < snake.size(); k++)
		{
			gotoxy(snake[k].position_x, snake[k].position_y);
			cout << snake[k].body;
		}
	}
	void clear_Tail(deque<SnakeNode>& snake) //清除蛇尾，不建議使用清屏函式，避免屏閃
	{
		int k = snake.size() - 1;
		gotoxy(snake[k].position_x, snake[k].position_y);
		cout << " "; //蛇每移動一次（即一格），就把上一次畫出來的蛇尾擦掉
	}
	void key_Down(deque<SnakeNode>& snake) //按鍵響應
	{
		char key = hit;
		if (_kbhit()) //接受按鍵
			hit = _getch();
		for (int i = snake.size() - 1; i > 0; i--) //蛇的移動方法，每移動一次，後面一節的身體到了它的前一節身體上
		{
			snake[i].position_x = snake[i - 1].position_x;
			snake[i].position_y = snake[i - 1].position_y;
		}
		if ((hit == 'a' || hit == 'A') && hit != 'd')
		{
			hit = 'a'; snake[0].position_x--;
		}
		else if ((hit == 'd' || hit == 'D') && hit != 'a')
		{
			hit = 'd'; snake[0].position_x++;
		}
		else if ((hit == 'w' || hit == 'W') && hit != 's')
		{
			hit = 'w'; snake[0].position_y--;
		}
		else if ((hit == 's' || hit == 'S') && hit != 'w')
		{
			hit = 's'; snake[0].position_y++;
		}
	}
	void addOneSnakeNode(deque<SnakeNode>& snake, SnakeNode tail) {
		
		snake.back().body = 'o'; //把初始化蛇的空尾顯示化為o，看到的效果就是加了一節
		snake.push_back(tail); //再新增一節空尾，便於下次操作
	}
	bool judge_state(deque<SnakeNode>& snake) //判斷蛇是否撞牆或撞身體
	{
		return collision(snake[0], map[snake[0].position_y][snake[0].position_x]);
	}
private:
	char hit = 'w';       //按鍵輸入
};




//3.遊戲執行類
class Game
{
protected:
	char image;
	int score = 0;        //遊戲分數
	bool eat_Food = false;      //是否吃到食物
	int speed = 400;       //蛇的速度
	bool snake_state = true;     //蛇的狀態
	int level = 1;        //遊戲關卡
	
public:
	Game();
	void show()
	{
		gotoxy(65, 0);
		cout << "你的得分是:";
		gotoxy(71, 1);
		cout << score;
		gotoxy(69, 2);
		cout << "關卡:" << level;
	}
};



Game::Game()
{
	HideCursor();
	srand(static_cast<unsigned int>(time(NULL))); //隨機數種子
	deque<SnakeNode> snakeBody; //定義一個佇列，裝蛇的身體 
	SnakeNode tail; //蛇尾

	Food food;
	Snake snake;
	Wall wall;
	snake.init_snake(snakeBody);
	food.draw_Food();
	while (1)
	{
		buildMap();
		buildSnakeInMap(snakeBody);
		wall.draw_Frame();
		tail = snakeBody.back();
		if (eat_Food)
		{
			snake.addOneSnakeNode(snakeBody, tail);
			food.foodEaten();
			food.draw_Food();
			score++;
			if (score % 5 == 0)
			{
				speed *= 0.8;
				level++;
			}
			eat_Food = false;
		}
		if (level == 10)
			break;
		snake.key_Down(snakeBody);
		snake.draw_Snake(snakeBody);
		snake_state = snake.judge_state(snakeBody);
		if (!snake_state)
			break;
		food.judge_eatFood(snakeBody[0], eat_Food);
		Sleep(speed);
		snake.clear_Tail(snakeBody);
		show();
	}
}
int main()
{
	system("mode con cols=100 lines=40"); //設定開啟視窗大小
	system("color 7C"); //設定背景色和前景色
	system("title 貪吃蛇 v1.0"); //設定視窗標題
	Game game;
	gotoxy(0, 32);
	cout << "Game over!" << endl;
}








/*
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <deque>
#include <ctime>
#pragma warning(disable:4996)
using namespace std;
HANDLE hOut;
COORD pos;
enum mapSize { width = 60, height = 30 }; //遊戲地圖
//1.實現gotoxy函式
void gotoxy(short x, short y)
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); //獲取控制代碼
	pos = { x,y };
	SetConsoleCursorPosition(hOut, pos);  //移動游標到指定位置
}
void HideCursor() //隱藏游標
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);//獲取控制檯游標資訊
	CursorInfo.bVisible = false;    //隱藏控制檯游標
	SetConsoleCursorInfo(handle, &CursorInfo);//設定控制檯游標狀態
}




class node {
public:
	unsigned short category;
	unsigned short interacts;

	/*
	Snake: 0001
	wall:  0010
	food:  0100
	space: 1000
	*//*
};

bool collision(node colliding, node collided) {
	return colliding.interacts & collided.category;
}
node map[height][width];





class Space :public node {
public:
	Space() {
		category = 8;
		interacts = 15;
	}
};

class SnakeNode :public node {
public:
	char body;
	short position_x, position_y;    //蛇的座標
	SnakeNode() {
		category = 1;
		interacts = 12;
	}
};

class Wall :public node {
public:
	Wall() {
		category = 2;
		interacts = 0;
	}
	void draw_Frame()  //畫邊框
	{
		for (int i = 0; i < height; i++)
		{
			gotoxy(0, i);
			cout << "■";
			gotoxy(width, i);
			cout << "■";
		}
		for (int i = 0; i <= width; i += 2) //■ 這個符號佔兩個字元位置，所以是+2
		{
			gotoxy(i, 0);
			cout << "■";
			gotoxy(i, height);
			cout << "■";
		}
	}
};

void buildSnakeInMap(deque<SnakeNode> snake) {
	SnakeNode snode;
	for (auto i : snake) {
		map[i.position_y][i.position_x] = snode;
	}
}
void buildMap() {
	Wall wall;
	Space space;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
				map[i][j] = wall;
			}
			else map[i][j] = space;
		}
	}
}
class Food :public node {
public:
	Food() {
		category = 4;
		interacts = 8;
	}
	short food_x, food_y;      //食物座標
	void draw_Food() //畫食物
	{
		Food food;
		while (1)
		{
			food_x = rand() % (width - 4) + 2; //食物要在地圖中，不能再地圖邊框上，地圖的符號在x方向佔兩個字元位置所以+2，而-4則是減去邊框
			food_y = rand() % (height - 2) + 1; //與上同理
			if (collision(food, map[food_y][food_x]) && food_x % 2 == 0)
				break;
		}
		gotoxy(food_x, food_y);
		cout << "O";
		map[food_y][food_x] = food;
	}
	void foodEaten() {
		gotoxy(food_x, food_y);
		cout << " "; //食物被吃後要在原來的位置畫空，否則游標會退位，導致邊框錯位
	}
	void judge_eatFood(SnakeNode SnakeHead, bool& eat_Food) //判斷是否吃到食物
	{
		if (food_x == SnakeHead.position_x && food_y == SnakeHead.position_y)
			eat_Food = true;
	}
};

class Snake {
public:

	void init_snake(deque<SnakeNode>& snake)  //初始化蛇
	{
		SnakeNode temp = SnakeNode();
		temp.body = '#';
		temp.position_x = width / 2;
		temp.position_y = static_cast<short>(height / 2);
		snake.push_back(temp); //新增蛇頭
		for (int i = 0; i < 3; i++) //蛇的初始身體節數，可自定
		{
			temp = SnakeNode();
			temp.body = 'o';
			temp.position_x = width / 2;
			temp.position_y = static_cast<short>((height / 2) + 1 + i);

			snake.push_back(temp);
		}

		temp = SnakeNode();
		temp.body = ' ';
		temp.position_x = width / 2;
		temp.position_y = static_cast<short>((height / 2) + 4);
		snake.push_back(temp); //新增蛇尾，先放空，以便於後面新增節數時操作
	}
	void draw_Snake(deque<SnakeNode>& snake) //畫蛇
	{
		for (int k = 0; k < snake.size(); k++)
		{
			gotoxy(snake[k].position_x, snake[k].position_y);
			cout << snake[k].body;
		}
	}
	void clear_Tail(deque<SnakeNode>& snake) //清除蛇尾，不建議使用清屏函式，避免屏閃
	{
		int k = snake.size() - 1;
		gotoxy(snake[k].position_x, snake[k].position_y);
		cout << " "; //蛇每移動一次（即一格），就把上一次畫出來的蛇尾擦掉
	}
	void key_Down(deque<SnakeNode>& snake) //按鍵響應
	{
		char key = hit;
		if (_kbhit()) //接受按鍵
			hit = _getch();
		for (int i = snake.size() - 1; i > 0; i--) //蛇的移動方法，每移動一次，後面一節的身體到了它的前一節身體上
		{
			snake[i].position_x = snake[i - 1].position_x;
			snake[i].position_y = snake[i - 1].position_y;
		}
		if ((hit == 'a' || hit == 'A') && hit != 'd')
		{
			hit = 'a'; snake[0].position_x--;
		}
		else if ((hit == 'd' || hit == 'D') && hit != 'a')
		{
			hit = 'd'; snake[0].position_x++;
		}
		else if ((hit == 'w' || hit == 'W') && hit != 's')
		{
			hit = 'w'; snake[0].position_y--;
		}
		else if ((hit == 's' || hit == 'S') && hit != 'w')
		{
			hit = 's'; snake[0].position_y++;
		}
	}
	void addOneSnakeNode(deque<SnakeNode>& snake, SnakeNode tail) {

		snake.back().body = 'o'; //把初始化蛇的空尾顯示化為o，看到的效果就是加了一節
		snake.push_back(tail); //再新增一節空尾，便於下次操作
	}
	bool judge_state(deque<SnakeNode>& snake) //判斷蛇是否撞牆或撞身體
	{
		return collision(snake[0], map[snake[0].position_y][snake[0].position_x]);
	}
private:
	char hit = 'w';       //按鍵輸入
};




//3.遊戲執行類
class Game
{
protected:
	char image;
	int score = 0;        //遊戲分數
	bool eat_Food = false;      //是否吃到食物
	int speed = 400;       //蛇的速度
	bool snake_state = true;     //蛇的狀態
	int level = 1;        //遊戲關卡

public:
	Game();
	void show()
	{
		gotoxy(65, 0);
		cout << "你的得分是:";
		gotoxy(71, 1);
		cout << score;
		gotoxy(69, 2);
		cout << "關卡:" << level;
	}
};



Game::Game()
{
	HideCursor();
	srand(static_cast<unsigned int>(time(NULL))); //隨機數種子
	deque<SnakeNode> snake; //定義一個佇列，裝蛇的身體 
	SnakeNode tail; //蛇尾
	Food f;
	Snake s;
	Wall w;
	s.init_snake(snake);
	
	f.draw_Food();
	while (1)
	{
		buildMap();
		buildSnakeInMap(snake);
		w.draw_Frame();
		tail = snake.back();
		if (eat_Food)
		{
			s.addOneSnakeNode(snake, tail);
			f.foodEaten();
			f.draw_Food();
			score++;
			if (score % 5 == 0)
			{
				speed *= 0.8;
				level++;
			}
			eat_Food = false;
		}
		if (level == 10)
			break;
		s.key_Down(snake);
		s.draw_Snake(snake);
		snake_state = s.judge_state(snake);
		if (!snake_state)
			break;
		f.judge_eatFood(snake[0], eat_Food);
		Sleep(speed);
		s.clear_Tail(snake);
		show();
	}
}
int main()
{
	system("mode con cols=100 lines=40"); //設定開啟視窗大小
	system("color 7C"); //設定背景色和前景色
	system("title 貪吃蛇 v1.0"); //設定視窗標題
	Game game;
	gotoxy(0, 32);
	cout << "Game over!" << endl;
}

*/