
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <deque>
#include <ctime>
#pragma warning(disable:4996)
using namespace std;
HANDLE hOut;
COORD pos;
enum mapSize { width = 60, height = 30 }; //�C���a��
//1.��{gotoxy�禡
void gotoxy(short x, short y)
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); //�������N�X
	pos = { x,y };
	SetConsoleCursorPosition(hOut, pos);  //���ʴ�Ш���w��m
}
void HideCursor() //���ô��
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);//��������i��и�T
	CursorInfo.bVisible = false;    //���ñ����i���
	SetConsoleCursorInfo(handle, &CursorInfo);//�]�w�����i��Ъ��A
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
	short position_x, position_y;    //�D���y��
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
	void draw_Frame()  //�e���
	{
		for (int i = 0; i < height; i++)
		{
			gotoxy(0, i);
			cout << "��";
			gotoxy(width, i);
			cout << "��";
		}
		for (int i = 0; i <= width; i += 2) //�� �o�ӲŸ�����Ӧr����m�A�ҥH�O+2
		{
			gotoxy(i, 0);
			cout << "��";
			gotoxy(i, height);
			cout << "��";
		}
	}
};

class Food :public node {
public:
	Food() {
		category = 4;
		interacts = 8;
	}
	short food_x, food_y;      //�����y��
	void draw_Food() //�e����
	{
		Food food;
		while (1)
		{
			food_x = rand() % (width - 4) + 2; //�����n�b�a�Ϥ��A����A�a����ؤW�A�a�Ϫ��Ÿ��bx��V����Ӧr����m�ҥH+2�A��-4�h�O��h���
			food_y = rand() % (height - 2) + 1; //�P�W�P�z
			if (!collision(food, map[food_y][food_x]) && food_x % 2 == 0)
				break;
		}
		gotoxy(food_x, food_y);
		cout << "O";
		map[food_y][food_x] = food;
	}
	void foodEaten() {
		gotoxy(food_x, food_y);
		cout << " "; //�����Q�Y��n�b��Ӫ���m�e�šA�_�h��з|�h��A�ɭP��ؿ���
	}
	void judge_eatFood(SnakeNode SnakeHead, bool& eat_Food) //�P�_�O�_�Y�쭹��
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
	
	void init_snake(deque<SnakeNode>& snake)  //��l�ƳD
	{
		SnakeNode temp = SnakeNode();
		temp.body = '#';
		temp.position_x = width / 2;
		temp.position_y = static_cast<short>(height / 2);
		snake.push_back(temp); //�s�W�D�Y
		for (int i = 0; i < 3; i++) //�D����l����`�ơA�i�۩w
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
		snake.push_back(temp); //�s�W�D���A����šA�H�K��᭱�s�W�`�Ʈɾާ@
	}
	void draw_Snake(deque<SnakeNode>& snake) //�e�D
	{
		for (int k = 0; k < snake.size(); k++)
		{
			gotoxy(snake[k].position_x, snake[k].position_y);
			cout << snake[k].body;
		}
	}
	void clear_Tail(deque<SnakeNode>& snake) //�M���D���A����ĳ�ϥβM�̨禡�A�קK�̰{
	{
		int k = snake.size() - 1;
		gotoxy(snake[k].position_x, snake[k].position_y);
		cout << " "; //�D�C���ʤ@���]�Y�@��^�A�N��W�@���e�X�Ӫ��D������
	}
	void key_Down(deque<SnakeNode>& snake) //�����T��
	{
		char key = hit;
		if (_kbhit()) //��������
			hit = _getch();
		for (int i = snake.size() - 1; i > 0; i--) //�D�����ʤ�k�A�C���ʤ@���A�᭱�@�`�������F�����e�@�`����W
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
		
		snake.back().body = 'o'; //���l�ƳD���ŧ���ܤƬ�o�A�ݨ쪺�ĪG�N�O�[�F�@�`
		snake.push_back(tail); //�A�s�W�@�`�ŧ��A�K��U���ާ@
	}
	bool judge_state(deque<SnakeNode>& snake) //�P�_�D�O�_����μ�����
	{
		return collision(snake[0], map[snake[0].position_y][snake[0].position_x]);
	}
private:
	char hit = 'w';       //�����J
};




//3.�C��������
class Game
{
protected:
	char image;
	int score = 0;        //�C������
	bool eat_Food = false;      //�O�_�Y�쭹��
	int speed = 400;       //�D���t��
	bool snake_state = true;     //�D�����A
	int level = 1;        //�C�����d
	
public:
	Game();
	void show()
	{
		gotoxy(65, 0);
		cout << "�A���o���O:";
		gotoxy(71, 1);
		cout << score;
		gotoxy(69, 2);
		cout << "���d:" << level;
	}
};



Game::Game()
{
	HideCursor();
	srand(static_cast<unsigned int>(time(NULL))); //�H���ƺؤl
	deque<SnakeNode> snakeBody; //�w�q�@�Ӧ�C�A�˳D������ 
	SnakeNode tail; //�D��

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
	system("mode con cols=100 lines=40"); //�]�w�}�ҵ����j�p
	system("color 7C"); //�]�w�I����M�e����
	system("title �g�Y�D v1.0"); //�]�w�������D
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
enum mapSize { width = 60, height = 30 }; //�C���a��
//1.��{gotoxy�禡
void gotoxy(short x, short y)
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); //�������N�X
	pos = { x,y };
	SetConsoleCursorPosition(hOut, pos);  //���ʴ�Ш���w��m
}
void HideCursor() //���ô��
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);//��������i��и�T
	CursorInfo.bVisible = false;    //���ñ����i���
	SetConsoleCursorInfo(handle, &CursorInfo);//�]�w�����i��Ъ��A
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
	short position_x, position_y;    //�D���y��
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
	void draw_Frame()  //�e���
	{
		for (int i = 0; i < height; i++)
		{
			gotoxy(0, i);
			cout << "��";
			gotoxy(width, i);
			cout << "��";
		}
		for (int i = 0; i <= width; i += 2) //�� �o�ӲŸ�����Ӧr����m�A�ҥH�O+2
		{
			gotoxy(i, 0);
			cout << "��";
			gotoxy(i, height);
			cout << "��";
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
	short food_x, food_y;      //�����y��
	void draw_Food() //�e����
	{
		Food food;
		while (1)
		{
			food_x = rand() % (width - 4) + 2; //�����n�b�a�Ϥ��A����A�a����ؤW�A�a�Ϫ��Ÿ��bx��V����Ӧr����m�ҥH+2�A��-4�h�O��h���
			food_y = rand() % (height - 2) + 1; //�P�W�P�z
			if (collision(food, map[food_y][food_x]) && food_x % 2 == 0)
				break;
		}
		gotoxy(food_x, food_y);
		cout << "O";
		map[food_y][food_x] = food;
	}
	void foodEaten() {
		gotoxy(food_x, food_y);
		cout << " "; //�����Q�Y��n�b��Ӫ���m�e�šA�_�h��з|�h��A�ɭP��ؿ���
	}
	void judge_eatFood(SnakeNode SnakeHead, bool& eat_Food) //�P�_�O�_�Y�쭹��
	{
		if (food_x == SnakeHead.position_x && food_y == SnakeHead.position_y)
			eat_Food = true;
	}
};

class Snake {
public:

	void init_snake(deque<SnakeNode>& snake)  //��l�ƳD
	{
		SnakeNode temp = SnakeNode();
		temp.body = '#';
		temp.position_x = width / 2;
		temp.position_y = static_cast<short>(height / 2);
		snake.push_back(temp); //�s�W�D�Y
		for (int i = 0; i < 3; i++) //�D����l����`�ơA�i�۩w
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
		snake.push_back(temp); //�s�W�D���A����šA�H�K��᭱�s�W�`�Ʈɾާ@
	}
	void draw_Snake(deque<SnakeNode>& snake) //�e�D
	{
		for (int k = 0; k < snake.size(); k++)
		{
			gotoxy(snake[k].position_x, snake[k].position_y);
			cout << snake[k].body;
		}
	}
	void clear_Tail(deque<SnakeNode>& snake) //�M���D���A����ĳ�ϥβM�̨禡�A�קK�̰{
	{
		int k = snake.size() - 1;
		gotoxy(snake[k].position_x, snake[k].position_y);
		cout << " "; //�D�C���ʤ@���]�Y�@��^�A�N��W�@���e�X�Ӫ��D������
	}
	void key_Down(deque<SnakeNode>& snake) //�����T��
	{
		char key = hit;
		if (_kbhit()) //��������
			hit = _getch();
		for (int i = snake.size() - 1; i > 0; i--) //�D�����ʤ�k�A�C���ʤ@���A�᭱�@�`�������F�����e�@�`����W
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

		snake.back().body = 'o'; //���l�ƳD���ŧ���ܤƬ�o�A�ݨ쪺�ĪG�N�O�[�F�@�`
		snake.push_back(tail); //�A�s�W�@�`�ŧ��A�K��U���ާ@
	}
	bool judge_state(deque<SnakeNode>& snake) //�P�_�D�O�_����μ�����
	{
		return collision(snake[0], map[snake[0].position_y][snake[0].position_x]);
	}
private:
	char hit = 'w';       //�����J
};




//3.�C��������
class Game
{
protected:
	char image;
	int score = 0;        //�C������
	bool eat_Food = false;      //�O�_�Y�쭹��
	int speed = 400;       //�D���t��
	bool snake_state = true;     //�D�����A
	int level = 1;        //�C�����d

public:
	Game();
	void show()
	{
		gotoxy(65, 0);
		cout << "�A���o���O:";
		gotoxy(71, 1);
		cout << score;
		gotoxy(69, 2);
		cout << "���d:" << level;
	}
};



Game::Game()
{
	HideCursor();
	srand(static_cast<unsigned int>(time(NULL))); //�H���ƺؤl
	deque<SnakeNode> snake; //�w�q�@�Ӧ�C�A�˳D������ 
	SnakeNode tail; //�D��
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
	system("mode con cols=100 lines=40"); //�]�w�}�ҵ����j�p
	system("color 7C"); //�]�w�I����M�e����
	system("title �g�Y�D v1.0"); //�]�w�������D
	Game game;
	gotoxy(0, 32);
	cout << "Game over!" << endl;
}

*/