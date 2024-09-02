
#include <iostream>
#include<easyx.h>
#include<Gui/Gui.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <chrono>
#define TwoPointDistance(x1,y1,x2,y2) sqrt(pow(x1-x2,2)+pow(y1-y2,2))
#define FNT_NUM 5 //黄金矿工执行时的随机家具数量
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")
int level = 1;
int targetScore;
int levelTimeLimit;
int remainingTime;
int nextlevelTimeLimit;
int sum = 0;
int ifadd = 1;
int play = 0;
int clockm;
std::chrono::steady_clock::time_point levelStartTime;
std::chrono::steady_clock::time_point levelEndTime;
bool levelFailed = false;
bool levelPassed = false;
using namespace std;
enum Swingdir
{
	Left,
	Right
};//表示钩子的状态，来编写钩子的摇晃
enum State
{
	Norml,
	Short,
	Long,
	Hitwall
};
int money = 0;
int which = 0;
bool game_on = 0;
int boost = 0;
int moneyj = 0;
int j;
float speed = 0.2;
int chooseskin;
int title;
struct Hook {
	float x, y; // 起始点
	float endx, endy; // 结束点
	float endx1, endy1;
	float endx2, endy2;
	float len; // 钩子长度
	float radian; // 当前角度
	float dx, dy; // 速度
	int state; // 状态
	int swingdir; // 摆动方向
	int angleIndex; // 当前角度索引
	int clawx;
	int clawy;
	int clawx1;
	int clawy1;
	int clawx2;
	int clawy2;
	int index;
}hook;

const float discreteAngles[] = { -3.14 / 2,-3.14 / 2.5, -3.14 / 3,-3.14 / 3.5,-3.14 / 4,-3.14 / 6, -3.14 / 8,-3.14 / 10. - 3.14 / 12,-3.14 / 14, -3.14 / 16, 0,
3.14 / 16,3.14 / 14 ,3.14 / 12 ,3.14 / 10,3.14 / 8,3.14 / 6,3.14 / 4 ,3.14 / 3.5,3.14 / 3,3.14 / 2.5,3.14 / 2 }; // 离散角度
const int numAngles = sizeof(discreteAngles) / sizeof(discreteAngles[0]); // 角度数量
float findClosestAngle(float currentRadian, const float* angles, int numAngles) {
	float closestAngle = angles[0];
	float minDifference = fabs(currentRadian - closestAngle);

	for (int i = 1; i < numAngles; i++) {
		float difference = fabs(currentRadian - angles[i]);
		if (difference < minDifference) {
			minDifference = difference;
			closestAngle = angles[i];
		}
	}

	return closestAngle;
}

struct Fnt
{
	int x;
	int y;
	int size;
	int type;//方便写随机
	int value;
	bool flag;//是否存在
	int lastgrasp;
}fnt[8];
struct Resource {
	IMAGE img_start[9];
}res;

//初始化
void res_init(struct Resource* res)//不表明struct的话就引用不了res->
{
	//数组的查询方式+0
	loadimage(res->img_start + 0, "resourse/image/01.jpg", getwidth(), getheight());

	loadimage(res->img_start + 1, "resourse/image/02.jpg", getwidth(), getheight());
	loadimage(res->img_start + 2, "resourse/image/03.jpg", getwidth(), getheight());
	loadimage(res->img_start + 3, "resourse/image/04.jpg", getwidth(), getheight());
	loadimage(res->img_start + 4, "resourse/image/05.jpg", getwidth(), getheight());

	loadimage(res->img_start + 5, "resourse/image/06.jpg", getwidth(), getheight());
	loadimage(res->img_start + 6, "resourse/image/07.jpg", getwidth(), getheight());
	loadimage(res->img_start + 7, "resourse/image/08.jpg", getwidth(), getheight());
	loadimage(res->img_start + 8, "resourse/image/13.jpg", getwidth(), getheight());
}
//鼠标点击切换场景

void loadPage(const char* filePath) {
	// 清除当前内容
	cleardevice();
	// 创建图像对象并加载图像
	IMAGE img;
	loadimage(&img, filePath);
	// 显示图像
	putimage(0, 0, &img);
}

void resetHookPosition(Hook* pthis) {
	// 重新设置钩子的起始点和结束点
	pthis->x = 400;
	pthis->y = 120;
	pthis->endx = 400;
	pthis->endy = 120;
	pthis->clawx = 400;
	pthis->clawy = 120;
	pthis->index = -1;
}

void starupscene(ExMessage* msg)
{//表示现在是哪个画面

	if (msg->message == WM_LBUTTONDOWN && which < 8)
	{
		++which;
		mciSendString(_T("play bgm01 from 120"), NULL, 0, NULL);
		switch (which) {
		case 2: { mciSendString(_T("play goodfriend from 0"), NULL, 0, NULL); break; }
		case 4: { mciSendString(_T("play badluck from 0"), NULL, 0, NULL);break; }
		case 5: { mciSendString(_T("play worst from 0"), NULL, 0, NULL);break; }
		case 6: { mciSendString(_T("play steal from 0"), NULL, 0, NULL);break; }
		}
	}

}
//判断鼠标是否在矩形区域
bool isinrect(ExMessage* msg, int x, int y, int w, int h)
{
	if (msg->x > x && msg->y > y && msg->x < x + w && msg->y < y + h)
	{
		return true;
	}
	return false;
}
void hook_init(Hook* pthis)
{
	pthis->len = 30;
	pthis->x = 400;
	pthis->y = 120;
	pthis->endx = pthis->x;
	pthis->endy = pthis->y + pthis->len;

	pthis->swingdir = Right;
	pthis->state = Norml;
	pthis->index = -1;
	pthis->clawx = pthis->endx - 13;
	pthis->clawy = pthis->endy + pthis->len - 13;
	if (boost == 1) {
		hook.endx1 = hook.endx + 0.5 * pthis->len;
		hook.endx2 = hook.endx - 0.5 * pthis->len;
		hook.endy1 = hook.endy - 0.134 * pthis->len;
		hook.endy2 = hook.endy - 0.134 * pthis->len;

		pthis->clawx1 = pthis->endx1 - 13;
		pthis->clawy1 = pthis->endy1 + pthis->len - 13;

		pthis->clawx2 = pthis->endx2 - 13;
		pthis->clawy2 = pthis->endy2 + pthis->len - 13;
	}

}
IMAGE img_fnt[8];//单独在struct外面定义的，不需要Fnt->img_fnt
IMAGE img_claw[1];


void fnt_init()
{


	for (int i = 0;i < FNT_NUM;i++)

	{

		fnt[i].x = 160 * i + rand() % 25 + 20;


		fnt[i].y = rand() % 250 + 180;
		fnt[i].type = rand() % 8;

	}


	for (int i = 0;i < FNT_NUM;i++) {
		if (fnt[i].type == 0) { fnt[i].value = 120;fnt[i].size = 172; }

		else if (fnt[i].type == 1) { fnt[i].value = 50; fnt[i].size = 76; }
		else if (fnt[i].type == 2) { fnt[i].value = 180;fnt[i].size = 140; }
		else if (fnt[i].type == 3) { fnt[i].value = 80;fnt[i].size = 140; }
		else if (fnt[i].type == 4) { fnt[i].value = 210; fnt[i].size = 140; }
		else if (fnt[i].type == 5) { fnt[i].value = 150; fnt[i].size = 70; }
		else if (fnt[i].type == 6) { fnt[i].value = 200; fnt[i].size = 100; }
		else if (fnt[i].type == 7) { fnt[i].value = 100; fnt[i].size = 100; }
	}
	if (title == 0) {
		loadimage(img_fnt + 0, "resourse/image/sofa.png");

		loadimage(img_fnt + 1, "resourse/image/cup.png");
		loadimage(img_fnt + 2, "resourse/image/bed.png", 140, 160);
		loadimage(img_fnt + 3, "resourse/image/guizi.png", 140, 160);
		loadimage(img_fnt + 4, "resourse/image/box.png", 140, 160);
		loadimage(img_fnt + 5, "resourse/image/kuluo.png");
		loadimage(img_fnt + 6, "resourse/image/lajiao.png");
		loadimage(img_fnt + 7, "resourse/image/time.png", 40, 40);
	}
	if (title == 1) {
		loadimage(img_fnt + 0, "resourse/image/sofa1.png");

		loadimage(img_fnt + 1, "resourse/image/jidi.png", 60, 60);
		loadimage(img_fnt + 2, "resourse/image/queen_slime.png");
		loadimage(img_fnt + 3, "resourse/image/guizi1.png", 140, 160);
		loadimage(img_fnt + 4, "resourse/image/xiedu.png", 128, 150);
		loadimage(img_fnt + 5, "resourse/image/kuluo1.png", 30, 30);
		loadimage(img_fnt + 6, "resourse/image/lajiao1.png");
		loadimage(img_fnt + 7, "resourse/image/clock.png");
	}

	if (title == 2) {
		loadimage(img_fnt + 0, "resourse/image/jiji1.png");

		loadimage(img_fnt + 1, "resourse/image/jiji2.png", 60, 60);
		loadimage(img_fnt + 2, "resourse/image/jiji3.png");
		loadimage(img_fnt + 3, "resourse/image/jiji4.png");
		loadimage(img_fnt + 4, "resourse/image/jiji5.png");
		loadimage(img_fnt + 5, "resourse/image/niu.png", 30, 30);
		loadimage(img_fnt + 6, "resourse/image/lajiao3.png");
		loadimage(img_fnt + 7, "resourse/image/clockb.png");
	}


	for (int i = 0;i < FNT_NUM;i++) { fnt[i].flag = 1; }

}
//载入家具照片
void music_init()
{
	mciSendString(_T("open resourse/music/bgm01.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open resourse/music/guochang.mp3 alias bgm01"), NULL, 0, NULL);
	mciSendString(_T("open resourse/music/grasp.mp3 alias bgm02"), NULL, 0, NULL);
	mciSendString(_T("open resourse/music/title.mp3 alias title"), NULL, 0, NULL);
	mciSendString(_T("open resourse/music/goodfriend.mp3 alias goodfriend"), NULL, 0, NULL);
	mciSendString(_T("open resourse/music/badluck.mp3 alias badluck"), NULL, 0, NULL);
	mciSendString(_T("open resourse/music/worst.mp3 alias worst"), NULL, 0, NULL);
	mciSendString(_T("open resourse/music/steal.mp3 alias steal"), NULL, 0, NULL);
	mciSendString(_T("open resourse/music/levelup.mp3 alias levelup"), NULL, 0, NULL);
	mciSendString(_T("open resourse/music/caidan.mp3 alias caidan"), NULL, 0, NULL);
}
void game_init()
{
	srand(time(NULL));
	hook_init(&hook);
	setbkmode(TRANSPARENT);
	fnt_init();
}

void hook_draw(Hook* pthis)
{
	setlinestyle(PS_SOLID, 3);
	setlinecolor(RGB(184, 180, 164));
	line(pthis->x, pthis->y, pthis->endx, pthis->endy);
	if (boost == 1) {
		line(pthis->x, pthis->y, pthis->endx1, pthis->endy1);
		line(pthis->x, pthis->y, pthis->endx2, pthis->endy1);
	}// 绘制钩子线

	setlinestyle(PS_SOLID, 5);
	setlinecolor(BLACK);
	line(pthis->x, 0, pthis->x, 48); // 绘制钩子摆动指示线
	if (title == 0) {
		loadimage(img_claw, "resourse/image/claw.png");
	}
	if (title == 1) {
		loadimage(img_claw, "resourse/image/C.png");
	}
	if (title == 2) {
		loadimage(img_claw, "resourse/image/claw5.png");
	}
	drawTexture(hook.clawx, hook.clawy, img_claw);
	if (boost == 1) {
		drawTexture(hook.clawx1, hook.clawy1, img_claw);
		drawTexture(hook.clawx2, hook.clawy2, img_claw);
	}// 绘制钩子爪子
}
void hook_swing(Hook* pthis)
{
	if (pthis->state == Norml)
	{
		// 根据当前的摆动方向更新角度
		float swingSpeed; // 摆动速度
		if (title == 0) {
			swingSpeed = 0.0008;

		}
		else if (title == 1) {
			swingSpeed = 0.0012;
		}
		else {
			swingSpeed = 0.0015;
		}

		if (pthis->swingdir == Right)
		{
			pthis->radian += swingSpeed;
		}
		else
		{
			pthis->radian -= swingSpeed;
		}

		// 设定摆动角度范围
		if (pthis->radian > 3.14 / 2)
		{
			pthis->swingdir = Left;
			pthis->radian = 3.14 / 2;
		}
		else if (pthis->radian < -3.14 / 2)
		{
			pthis->swingdir = Right;
			pthis->radian = -3.14 / 2;
		}

		// 更新钩子的位置
		hook.endx = hook.x + sin(pthis->radian) * pthis->len;
		hook.endy = hook.y + cos(pthis->radian) * pthis->len;


		hook.clawx = hook.endx;
		hook.clawy = hook.endy;

		if (boost == 1) {
			hook.endx1 = hook.endx + 0.5 * pthis->len;
			hook.endx2 = hook.endx - 0.5 * pthis->len;
			hook.endy1 = hook.endy - 0.134 * pthis->len;
			hook.endy2 = hook.endy - 0.134 * pthis->len;
			hook.clawx1 = hook.endx1;
			hook.clawy1 = hook.endy1;
			hook.clawx2 = hook.endx2;
			hook.clawy2 = hook.endy1;
		}
	}
}
void hook_control(Hook* pthis) {

	if ((GetAsyncKeyState(VK_SPACE) & 0x8000 || (MouseHit() && GetMouseMsg().uMsg == WM_LBUTTONDOWN)) && pthis->state == Norml)
	{
		pthis->state = Long;

		// 找到最接近当前摆动角度的离散角度
		pthis->radian = findClosestAngle(pthis->radian, discreteAngles, numAngles);

		// 更新速度
		pthis->dx = sin(pthis->radian) * speed;
		pthis->dy = cos(pthis->radian) * speed;

	}

	if (pthis->state == Long)
	{
		pthis->endx += pthis->dx;
		pthis->endy += pthis->dy;
	}
	else if (pthis->state == Short || pthis->state == Hitwall)
	{
		pthis->endx -= pthis->dx;
		pthis->endy -= pthis->dy;
	}

	if (TwoPointDistance(pthis->x, pthis->y, pthis->endx, pthis->endy) <= pthis->len)
	{
		pthis->state = Norml;
		ifadd = 1;

	}

	if (hook.endx < 0 || hook.endx > getwidth() || hook.endy > getheight())
	{
		hook.state = Hitwall;
	}

	hook.clawx = hook.endx - 13;
	hook.clawy = hook.endy - 13;
	if (boost == 1) {
		hook.endx1 = hook.endx + 0.5 * pthis->len;
		hook.endx2 = hook.endx - 0.5 * pthis->len;
		hook.endy1 = hook.endy - 0.134 * pthis->len;
		hook.endy2 = hook.endx - 0.134 * pthis->len;

		hook.clawx1 = hook.endx1 - 13;
		hook.clawy1 = hook.endy1 - 13;

		hook.clawx2 = hook.endx2 - 13;
		hook.clawy2 = hook.endy1 - 13;
	}
}
int previousScore1 = 0; // 斐波那契数列的第一个数
int previousScore2 = 500; // 斐波那契数列的第二个数，初始值与 targetScore 的初始值相匹配

int previousLevelTimeLimit = 0; // 上一关的 levelTimeLimit


void game_draw() {
	hook_draw(&hook);
	for (int i = 0; i < FNT_NUM; i++) {
		if (fnt[i].flag) {
			drawTexture(fnt[i].x, fnt[i].y, img_fnt + fnt[i].type);
		}
	}

	// 显示当前积分
	settextstyle(32, 0, "幼圆");
	settextcolor(WHITE);
	char str[50];
	sprintf_s(str, "金钱:%d", money);
	outtextxy(155, 20, str);

	// 显示剩余时间

	using namespace std::chrono;
	auto now = steady_clock::now();
	auto elapsedTime = duration_cast<seconds>(now - levelStartTime).count();
	int remainingTime = levelTimeLimit - elapsedTime;
	sprintf_s(str, "时间:%d", remainingTime > 0 ? remainingTime : 0);
	outtextxy(470, 20, str);


	// 显示目标积分
	sprintf_s(str, "目标:%d", targetScore);
	outtextxy(605, 20, str);

	//显示当前关卡数
	sprintf_s(str, "关卡:%d", level);
	outtextxy(20, 20, str);


	if (hook.state == Short) {
		settextstyle(22, 22, "幼圆");
		settextcolor(RGB(rand() % 1000, rand() % 1000, rand() % 1000));
		sprintf_s(str, "+%d", fnt[fnt->lastgrasp].value);
		outtextxy(270, 25, str);
	}
}

void level_init() {
	levelPassed = false;
	levelFailed = false;
	levelStartTime = std::chrono::steady_clock::now();
	// 更新 targetScore 以斐波那契数列递增
	int nextScore = previousScore1 + previousScore2;
	if (level < 10) {
		targetScore = nextScore;
		previousScore1 = previousScore2;
		previousScore2 = nextScore;
	}
	else if (level >= 10 && level < 20) {
		targetScore = targetScore + level * (500 + 50 * (level - 10));
	}
	else if (level >= 20) {
		targetScore = targetScore + level * level * 50;
	}

	// 设置初始关卡时间
	if (level == 1) {
		levelTimeLimit = 100;
	}
	else if (level == 2) {
		levelTimeLimit = 20;
	}
	else {
		levelTimeLimit = nextlevelTimeLimit;
	}
	previousLevelTimeLimit = levelTimeLimit; // 更新上一个关卡的时间

	levelStartTime = std::chrono::steady_clock::now();
}

void check_level_status() {
	using namespace std::chrono;
	auto now = steady_clock::now();
	auto elapsedTime = duration_cast<seconds>(now - levelStartTime).count();

	if (elapsedTime > levelTimeLimit) {
		levelFailed = true;
	}

	if (money >= targetScore) {
		levelPassed = true; // 确保目标积分达到时设置为 true
	}
	if (levelPassed || levelFailed) {
		// 关卡已结束，记录结束时间
		levelEndTime = std::chrono::steady_clock::now();

		// 计算关卡总用时
		auto elapsedTime1 = std::chrono::duration_cast<std::chrono::seconds>(levelEndTime - levelStartTime).count();
		if (level <= 20) {
			nextlevelTimeLimit = levelTimeLimit - elapsedTime1 + 2 * level;
		}
		else {
			nextlevelTimeLimit = levelTimeLimit - elapsedTime1 + level;
		}
	}
}


void judgegrap()
{
	for (int i = 0;i < FNT_NUM;i++)
	{
		if ((ifadd == 1 || play == 1) && fnt[i].flag == true && hook.endx > fnt[i].x - 10 && hook.endx<fnt[i].x + fnt[i].size + 5 && hook.endy > fnt[i].y && hook.endy < fnt[i].y + 150)
		{
			ifadd = 0;
			if (hook.endx >= 430 || hook.endy >= 150) {
				hook.index = i;money += fnt[i].value;
				j = money - moneyj;
				sum += 1;
				cout << sum;
			}

			if (j == 200) {
				boost = 1;
				chooseskin = 2;
			}

			if (j == 150) {
				speed = 3;
				chooseskin = 3;
			}
			if (j == 100) {
				play = 1;
				clockm = money;
			}
			moneyj = money;
			mciSendString(_T("play bgm02 from 300"), NULL, 0, NULL);

		}

		if ((ifadd == 1 || play == 1) && fnt[i].flag == true && hook.endx1 > fnt[i].x - 10 && hook.endx1<fnt[i].x + fnt[i].size + 5 && hook.endy1 > fnt[i].y && hook.endy1 < fnt[i].y + 150)
		{
			ifadd = 0;
			if (hook.endx1 >= 430 || hook.endy1 >= 150) {
				hook.index = i;money += fnt[i].value;
				j = money - moneyj;
				sum += 1;
				cout << sum;
			}

			if (j == 200) {
				boost = 1;
				chooseskin = 2;
			}

			if (j == 150) {
				speed = 3;
				chooseskin = 3;
			}
			if (j == 100) {
				play = 1;
				clockm = money;
			}
			moneyj = money;
			mciSendString(_T("play bgm02 from 300"), NULL, 0, NULL);

		}

		if ((ifadd == 1 || play == 1) && fnt[i].flag == true && hook.endx2 > fnt[i].x - 10 && hook.endx2<fnt[i].x + fnt[i].size + 5 && hook.endy2 > fnt[i].y && hook.endy2 < fnt[i].y + 150)
		{
			ifadd = 0;
			if (hook.endx2 >= 430 || hook.endy2 >= 150) {
				hook.index = i;money += fnt[i].value;
				j = money - moneyj;
				sum += 1;
				cout << sum;
			}

			if (j == 200) {
				boost = 1;
				chooseskin = 2;
			}

			if (j == 150) {
				speed = 3;
				chooseskin = 3;
			}
			if (j == 100) {
				play = 1;
				clockm = money;
			}
			moneyj = money;
			mciSendString(_T("play bgm02 from 300"), NULL, 0, NULL);

		}
	}

	if (hook.index != -1)
	{
		hook.state = Short;
		fnt[hook.index].x = hook.endx;
		fnt[hook.index].y = hook.endy;//家具和钩子一起走了
		fnt->lastgrasp = hook.index;
		if (TwoPointDistance(hook.x, hook.y, hook.endx, hook.endy) <= 30)
		{
			hook.state = Norml;//抓到之后收回停在normal阶段
			fnt[hook.index].flag = false;

			hook.index = -1;//在循环中恢复初始状态

		} hook.clawx = hook.endx - 13;hook.clawy = hook.endy - 13;



	}

}



void game_swing(Hook* pthis)
{
	hook_swing(&hook);
}

void levelup()
{
	for (int i = 0;i < FNT_NUM;i++)
	{

		fnt[i].flag = 1;


	}
}
void giflevelup() {

	const int FRAME_COUNT = 15; // 动画帧数量
	const int FRAME_DELAY = 100; // 每帧之间的延迟时间（毫秒）
	const char* FRAME_TEMPLATES[FRAME_COUNT] = {
		"resourse/image/levelup/1.png", "resourse/image/levelup/2.png", "resourse/image/levelup/3.png",
		"resourse/image/levelup/4.png", "resourse/image/levelup/5.png", "resourse/image/levelup/6.png",
		"resourse/image/levelup/7.png", "resourse/image/levelup/8.png", "resourse/image/levelup/9.png",
		"resourse/image/levelup/10.png", "resourse/image/levelup/11.png", "resourse/image/levelup/12.png",
		"resourse/image/levelup/13.png", "resourse/image/levelup/14.png","resourse/image/levelup/15.png"
	};
	mciSendString(_T("play levelup from 300"), NULL, 0, NULL);
	IMAGE frameImage;

	for (int i = 0; i < FRAME_COUNT; ++i) {
		loadimage(&frameImage, FRAME_TEMPLATES[i]); // 加载当前帧
		BeginBatchDraw();
		cleardevice(); // 清屏
		putimage(0, 0, &frameImage); // 绘制当前帧
		EndBatchDraw();

		Sleep(FRAME_DELAY); // 等待以显示下一帧

		if (_kbhit()) { // 检测是否有键盘输入
			_getch(); // 清除输入缓冲区
			break; // 用户按键时中断动画
		}
	}
}

void gifgameover() {

	const int FRAME_COUNT = 30; // 动画帧数量
	const int FRAME_DELAY = 100; // 每帧之间的延迟时间（毫秒）
	const char* FRAME_TEMPLATES[FRAME_COUNT] = {
		"resourse/image/gameover/1.png", "resourse/image/gameover/2.png", "resourse/image/gameover/3.png",
		"resourse/image/gameover/4.png", "resourse/image/gameover/5.png", "resourse/image/gameover/6.png",
		"resourse/image/gameover/7.png", "resourse/image/gameover/8.png", "resourse/image/gameover/9.png",
		"resourse/image/gameover/10.png", "resourse/image/gameover/11.png", "resourse/image/gameover/12.png",
		"resourse/image/gameover/13.png", "resourse/image/gameover/14.png","resourse/image/gameover/15.png",
		"resourse/image/gameover/16.png","resourse/image/gameover/17.png","resourse/image/gameover/18.png",
		"resourse/image/gameover/19.png","resourse/image/gameover/20.png","resourse/image/gameover/21.png",
		"resourse/image/gameover/22.png","resourse/image/gameover/23.png","resourse/image/gameover/24.png",
		"resourse/image/gameover/25.png","resourse/image/gameover/26.png","resourse/image/gameover/27.png",
		"resourse/image/gameover/28.png","resourse/image/gameover/29.png","resourse/image/gameover/30.png"
	};

	IMAGE frameImage;

	for (int i = 0; i < FRAME_COUNT; ++i) {
		loadimage(&frameImage, FRAME_TEMPLATES[i]); // 加载当前帧
		BeginBatchDraw();
		cleardevice(); // 清屏
		putimage(0, 0, &frameImage); // 绘制当前帧
		EndBatchDraw();

		Sleep(FRAME_DELAY); // 等待以显示下一帧

		if (_kbhit()) { // 检测是否有键盘输入
			_getch(); // 清除输入缓冲区
			break; // 用户按键时中断动画
		}
	}
}

void gifcongratulation() {

	const int FRAME_COUNT = 20; // 动画帧数量
	const int FRAME_DELAY = 100; // 每帧之间的延迟时间（毫秒）
	const char* FRAME_TEMPLATES[FRAME_COUNT] = {
		"resourse/image/end/1.png", "resourse/image/end/2.png", "resourse/image/end/3.png",
		"resourse/image/end/4.png", "resourse/image/end/5.png", "resourse/image/end/6.png",
		"resourse/image/end/7.png", "resourse/image/end/8.png", "resourse/image/end/9.png",
		"resourse/image/end/10.png", "resourse/image/end/11.png", "resourse/image/end/12.png",
		"resourse/image/end/13.png", "resourse/image/end/14.png","resourse/image/end/15.png",
		"resourse/image/end/16.png","resourse/image/end/17.png","resourse/image/end/18.png",
		"resourse/image/end/19.png","resourse/image/end/20.png"
	};

	IMAGE frameImage;

	for (int i = 0; i < FRAME_COUNT; ++i) {
		loadimage(&frameImage, FRAME_TEMPLATES[i]); // 加载当前帧
		BeginBatchDraw();
		cleardevice(); // 清屏
		putimage(0, 0, &frameImage); // 绘制当前帧
		EndBatchDraw();

		Sleep(FRAME_DELAY); // 等待以显示下一帧

		if (_kbhit()) { // 检测是否有键盘输入
			_getch(); // 清除输入缓冲区
			break; // 用户按键时中断动画
		}
	}
}

void gifcaidan() {

	const int FRAME_COUNT = 1; // 动画帧数量
	const int FRAME_DELAY = 0; // 每帧之间的延迟时间（毫秒）
	const char* FRAME_TEMPLATES[FRAME_COUNT] = {
		"resourse/image/caidan.png"
	};
	IMAGE frameImage;

	for (int i = 0; i < FRAME_COUNT; ++i) {
		loadimage(&frameImage, FRAME_TEMPLATES[i]); // 加载当前帧
		BeginBatchDraw();
		cleardevice(); // 清屏
		putimage(0, 0, &frameImage); // 绘制当前帧
		EndBatchDraw();

		Sleep(FRAME_DELAY); // 等待以显示下一帧

		if (_kbhit()) { // 检测是否有键盘输入
			_getch(); // 清除输入缓冲区
			break; // 用户按键时中断动画
		}
	}
}

void menu() {
	bool continueProcess = false;
	bool pageChanged = false;
	const char* previousPage = "resourse/image/levelup/15.png"; // 保存上一个页面路径

	while (!continueProcess) {
		loadPage(previousPage); // 加载初始页面

		while (!continueProcess) {
			if (_kbhit()) { // 检测键盘输入
				char key = _getch(); // 获取按键输入
				if (key == 27) { // ESC 键
					if (pageChanged) {
						// 返回到上一个页面
						loadPage(previousPage);
						pageChanged = false; // 重置页面状态
					}
					else {
						continueProcess = true; // 继续后续进程
					}
				}
			}
			if (GetAsyncKeyState(18)) {
				continueProcess = true; // 继续后续进程

			}
			if (MouseHit()) { // 检测鼠标点击
				MOUSEMSG mouseMsg = GetMouseMsg();
				if (mouseMsg.uMsg == WM_LBUTTONDOWN) { // 鼠标左键点击
					int mouseX = mouseMsg.x;
					int mouseY = mouseMsg.y;

					if (!pageChanged) {
						if (mouseX >= 70 && mouseX <= 122 && mouseY >= 150 && mouseY <= 203) {
							// 跳转到指定页面
							previousPage = "resourse/image/menu.png"; // 保存当前页面路径为上一个页面
							pageChanged = true;
							loadPage(previousPage);

						}
						else if (mouseX >= 677 && mouseX <= 733 && mouseY >= 150 && mouseY <= 206) {
							continueProcess = true; // 继续后续进程
						}
					}
				}
				else if (mouseMsg.uMsg == WM_RBUTTONDOWN) { // 鼠标右键点击
					if (pageChanged) {
						// 返回到上一个页面

						loadPage("resourse/image/levelup/15.png"); // 使用之前保存的页面路径
						pageChanged = false; // 重置页面状态
					}
				}
				// 清除鼠标消息
				FlushMouseMsgBuffer();
			}

			Sleep(10); // 稍微延迟，避免过多占用 CPU
		}
	}
}

void congratulation() {
	bool continueProcess = false;
	bool pageChanged = false;
	const char* previousPage = "resourse/image/end/20.png"; // 保存上一个页面路径

	while (!continueProcess) {
		loadPage(previousPage); // 加载初始页面

		while (!continueProcess) {
			if (_kbhit()) { // 检测键盘输入
				char key = _getch(); // 获取按键输入
				if (key == 27) {
					if (pageChanged) {
						// 返回到上一个页面
						loadPage(previousPage);
						pageChanged = false; // 重置页面状态
					}
					else {
						continueProcess = true; // 继续后续进程
					}
				}
			}
			if (GetAsyncKeyState(18)) {
				continueProcess = true; // 继续后续进程

			}
			if (MouseHit()) { // 检测鼠标点击
				MOUSEMSG mouseMsg = GetMouseMsg();
				if (mouseMsg.uMsg == WM_LBUTTONDOWN) { // 鼠标左键点击
					int mouseX = mouseMsg.x;
					int mouseY = mouseMsg.y;

					if (!pageChanged) {
						if (mouseX >= 70 && mouseX <= 122 && mouseY >= 150 && mouseY <= 203) {
							// 跳转到指定页面
							previousPage = "resourse/image/menu.png"; // 保存当前页面路径为上一个页面
							pageChanged = true;
							loadPage(previousPage);
						}
						else if (mouseX >= 677 && mouseX <= 733 && mouseY >= 150 && mouseY <= 206) {
							continueProcess = true; // 继续后续进程
						}
					}
				}
				else if (mouseMsg.uMsg == WM_RBUTTONDOWN) { // 鼠标右键点击
					if (pageChanged) {
						// 返回到上一个页面
						loadPage("resourse/image/end/20.png"); // 使用之前保存的页面路径
						pageChanged = false; // 重置页面状态
					}
				}
				// 清除鼠标消息
				FlushMouseMsgBuffer();
			}

			Sleep(10); // 稍微延迟，避免过多占用 CPU
		}
	}
}

void caidan() {
	bool continueProcess = false;
	while (!continueProcess) {

		if (MouseHit()) { // 检测鼠标点击
			MOUSEMSG mouseMsg = GetMouseMsg();
			if (mouseMsg.uMsg == WM_LBUTTONDOWN) { // 鼠标左键点击
				continueProcess = true; // 继续进程
			}
		}
		if (GetAsyncKeyState(18)) {
			continueProcess = true; // 继续后续进程

		}
		Sleep(10); // 稍微延迟，避免过多占用 CPU
	}
}


const int scoreLine1 = 10000;
const int scoreLine2 = 150000;// 设定分数线
bool congratulationDisplayed = false; // 标记是否已经跳转到 congratulation 界面
bool caidanDisplayed = false;
void end_level() {
	BeginBatchDraw();
	if (levelFailed) {
		gifgameover(); // 播放“game over”动画
		EndBatchDraw();
		exit(0);
	}
	else if (levelPassed) {
		Sleep(1000);

		if (money >= scoreLine1 && !congratulationDisplayed) {

			gifcongratulation();
			congratulation(); // 跳转到 congratulation 界面
			congratulationDisplayed = true; // 标记已跳转
		}

		else if (money >= scoreLine2 && !caidanDisplayed) {
			mciSendString(_T("play caidan from 300"), NULL, 0, NULL);
			gifcaidan();
			caidan(); // 跳转到 congratulation 界面
			caidanDisplayed = true; // 标记已跳转
		}
		else {
			giflevelup(); // 播放“level up”动画
			menu();
		}
		EndBatchDraw();

		fnt_init();
		resetHookPosition(&hook);
		sum = 0;
		level++;

		level_init();
	}
	EndBatchDraw();
}

int main() {

	music_init();
	mciSendString(_T("play bgm repeat from 1050"), NULL, 0, NULL);
	mciSendString(_T("play title from 0"), NULL, 0, NULL);
	initgraph(800, 550, EX_SHOWCONSOLE);
	res_init(&res);
	IMAGE  img_klm;
	loadimage(&img_klm, "resourse/image/klm.png");
	IMAGE  img_klm2;
	loadimage(&img_klm2, "resourse/image/klm2.png");
	IMAGE  img_klm3;
	loadimage(&img_klm3, "resourse/image/klm3.png");


	level_init();

	while (which < 9) {
		BeginBatchDraw();
		cleardevice();
		ExMessage msg;
		putimage(0, 0, res.img_start + which);
		while (peekmessage(&msg, EX_MOUSE)) {
			int mousex = msg.x;
			int mousey = msg.y;
		}
		if (title == 0) { loadimage(img_claw, "resourse/image/claw.png"); }
		if (title == 1) { loadimage(img_claw, "resourse/image/C.png"); }
		if (title == 2) { loadimage(img_claw, "resourse/image/claw5.png"); }
		drawTexture(msg.x - 10, msg.y - 10, img_claw);
		EndBatchDraw();
		if (which == 8) {

			if (msg.message == WM_LBUTTONDOWN) {
				if (msg.x >= 85 && msg.x <= 85 + 110 && msg.y >= 86 && msg.y <= 86 + 110) { title = 0;++which; }
				else if (msg.x >= 110 && msg.x <= 110 + 110 && msg.y >= 230 && msg.y <= 230 + 110) {
					title = 1;speed = 0.5, ++which;
				}
				else if (msg.x >= 146 && msg.x <= 146 + 100 && msg.y >= 373 && msg.y <= 373 + 100) {
					title = 2;speed = 0.8, ++which;
				}

			}
		}
		if (peekmessage(&msg, WH_MOUSE)) {
			starupscene(&msg);
		}
	}
	IMAGE img_bkg;
	if (title == 0) { loadimage(&img_bkg, "resourse/image/bkg.png"); }//导入背景
	if (title == 1) {
		loadimage(&img_bkg, "resourse/image/bkg11.jpg");
	}
	if (title == 2) {
		loadimage(&img_bkg, "resourse/image/bkg2.png");
	}
	game_init();
	while (true) {
		cleardevice();
		while (fnt[0].flag || fnt[1].flag || fnt[2].flag || fnt[3].flag || fnt[4].flag) {
			BeginBatchDraw();
			cleardevice();
			putimage(0, 70, &img_bkg);
			setfillcolor(RGB(144, 99, 164));
			solidrectangle(0, 0, getwidth(), 70);
			if (chooseskin == 2) {
				drawTexture((getwidth() - img_klm.getwidth()) / 2, 5, &img_klm2);
			}

			else if (chooseskin == 3)
			{
				drawTexture((getwidth() - img_klm.getwidth()) / 2, 5, &img_klm3);

			}
			else { drawTexture((getwidth() - img_klm.getwidth()) / 2, 5, &img_klm); }
			game_draw();
			game_swing(&hook);
			hook_control(&hook);
			judgegrap();
			check_level_status(); // 确保每帧调用
			if (sum >= 3 && (ifadd == 1 || play == 1)) {

				fnt_init();
				boost = 0;
				if (title == 0) {
					speed = 0.2;;

				}
				else if (title == 1) {
					speed = 0.5;;
				}
				else {
					speed = 0.8;;
				}

				sum = 0;
				chooseskin = 0;

				cout << sum;
			}

			if ((money - clockm) >= 3000) { play = 0; }
			if (levelFailed || levelPassed) {
				end_level();
				play = 0;
				break;
			}
			EndBatchDraw();
		}
	}

	return 0;
}


