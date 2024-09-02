//随机图片不重叠 （成功）优化钩子形状，加上爪子(完成） 重新选择合适的背景(实现） 加上菜单与故事互动（实现）
 //菜单优化，包括：点击按钮，包括设计音量，音乐切换，设计人员致谢，人物介绍
 //音乐添加 音乐切换(完成）
 //关卡设计 关卡切换

 //故事结局


#include <iostream>
#include<easyx.h>
#include<Gui/Gui.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#include <graphics.h>
#include <conio.h>
#include <windows.h>
#define TwoPointDistance(x1,y1,x2,y2) sqrt(pow(x1-x2,2)+pow(y1-y2,2))
#define FNT_NUM 8 //黄金矿工执行时的随机家具数量
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")
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
int speed = 2;
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
	pthis->clawy = pthis->endy + pthis->len -13;
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
		if (fnt[i].type == 0) { fnt[i].value = 100;fnt[i].size = 172; }

		else if (fnt[i].type == 1) { fnt[i].value = 30; fnt[i].size = 76; }
		else if (fnt[i].type == 2) { fnt[i].value = 190;fnt[i].size = 140; }
		else if (fnt[i].type == 3) { fnt[i].value = 70;fnt[i].size = 140; }
		else if (fnt[i].type == 4) { fnt[i].value = 290; fnt[i].size = 140; }
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

		loadimage(img_fnt + 1, "resourse/image/jidi.png",60,60);
		loadimage(img_fnt + 2, "resourse/image/queen_slime.png");
		loadimage(img_fnt + 3, "resourse/image/guizi1.png", 140, 160);
		loadimage(img_fnt + 4, "resourse/image/xiedu.png", 128, 150);
		loadimage(img_fnt + 5, "resourse/image/kuluo1.png",30,30);
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
		loadimage(img_fnt + 7, "resourse/image/clock1.png",30,30);
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
		float swingSpeed = 0.0008; // 摆动速度
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


	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && pthis->state == Norml)
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

void judgegrap()
{
	for (int i = 0;i < FNT_NUM;i++)
	{
		if (fnt[i].flag == true && hook.endx > fnt[i].x - 10 && hook.endx<fnt[i].x + fnt[i].size + 5 && hook.endy > fnt[i].y && hook.endy < fnt[i].y + 150)
		{
			hook.index = i;money += fnt[i].value;
			j = money - moneyj;

			if (j == 200) {
				boost = 1;
				chooseskin = 2;
			}

				if (j == 150) {
					speed = 20;
					chooseskin = 3;
				}
				moneyj = money;
				mciSendString(_T("play bgm02 from 300"), NULL, 0, NULL);
			
		}

		if (fnt[i].flag == true && hook.endx1 > fnt[i].x - 10 && hook.endx1<fnt[i].x + fnt[i].size + 5 && hook.endy1 > fnt[i].y && hook.endy1 < fnt[i].y + 150)
		{
			hook.index = i;money += fnt[i].value;
			j = money - moneyj;

			if (j == 200) {
				boost = 1;
				chooseskin = 2;
			}

			if (j == 150) {
				speed = 20;
				chooseskin = 3;
			}
			moneyj = money;
			mciSendString(_T("play bgm02 from 300"), NULL, 0, NULL);

		}

		if (fnt[i].flag == true && hook.endx2 > fnt[i].x - 10 && hook.endx2<fnt[i].x + fnt[i].size + 5 && hook.endy2 > fnt[i].y && hook.endy2 < fnt[i].y + 150)
		{
			hook.index = i;money += fnt[i].value;
			j = money - moneyj;

			if (j == 200) {
				boost = 1;
				chooseskin = 2;
			}

			if (j == 150) {
				speed = 20;
				chooseskin = 3;
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
void game_draw()
{
	hook_draw(&hook);
	for (int i = 0;i < FNT_NUM;i++)
	{
		if (fnt[i].flag == true) {
			drawTexture(fnt[i].x, fnt[i].y, img_fnt + fnt[i].type);
		}
	}

	settextstyle(36, 0, "幼圆");
	settextcolor(WHITE);
	char str[50];
	sprintf_s(str, "库洛米赚了:%d", money);
	outtextxy(20, 20, str);
	if (hook.state == Short)
	{
		settextstyle(22, 22, "幼圆");

		char str[50];
		settextcolor(RGB(rand() % 1000, rand() % 1000, rand() % 1000));
		sprintf_s(str, "+%d", fnt[fnt->lastgrasp].value);
		outtextxy(270, 25, str);
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
void giflevelup()
{
	const int FRAME_COUNT = 14; // 假设有 14 帧
	const char* FRAME_TEMPLATES[FRAME_COUNT] = {
		"resourse/image/levelup/1.png", "resourse/image/levelup/2.png", "resourse/image/levelup/3.png", "resourse/image/levelup/4.png", "resourse/image/levelup/5.png",
		"resourse/image/levelup/6.png", "resourse/image/levelup/7.png", "resourse/image/levelup/8.png", "resourse/image/levelup/9.png", "resourse/image/levelup/10.png",
		"resourse/image/levelup/11.png", "resourse/image/levelup/12.png", "resourse/image/levelup/13.png", "resourse/image/levelup/14.png" };
	IMAGE frameImage;

	while (true) {
		// 播放所有帧
		for (int i = 0; i < FRAME_COUNT; ++i) {
			loadimage(&frameImage, FRAME_TEMPLATES[i]); // 加载当前帧
			putimage(0, 0, &frameImage); // 显示当前帧
			Sleep(50); // 每帧之间的延迟时间，单位是毫秒

			if (_kbhit()) {
				break;
			}
		}

		// 检测用户是否按下任意键


	}
}




int main()

{
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
	

	while (which < 9)
	{
		BeginBatchDraw();
		cleardevice();






		ExMessage msg;
		putimage(0, 0, res.img_start + which);

		//不断的处理消息 有消息就返回true 没消息返回false
		while (peekmessage(&msg, EX_MOUSE)) {

			int mousex = msg.x;
			int mousey = msg.y;

		}
		if (title == 0) { loadimage(img_claw, "resourse/image/claw.png"); }
		if (title == 1) { loadimage(img_claw, "resourse/image/C.png"); }
		if (title == 2) { loadimage(img_claw, "resourse/image/claw5.png"); }
		drawTexture(msg.x - 10, msg.y - 10, img_claw);EndBatchDraw();
		
		if (which == 8) {
			
			if (msg.message == WM_LBUTTONDOWN) {
				if (msg.x >= 85 && msg.x <= 85 + 110 && msg.y >= 86 && msg.y <= 86 + 110) { title = 0;++which; }
				else if (msg.x >= 110 && msg.x <= 110 + 110 && msg.y >= 230 && msg.y <= 230 + 110) { title = 1;++which;
				}
				else if (msg.x >= 146 && msg.x <= 146 + 100 && msg.y >= 373 && msg.y <= 373 + 100) { title = 2;++which;
				}
				
			}
	    }


		if (peekmessage(&msg, WH_MOUSE))// 进入新的步骤
		{
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
		

		while (fnt[0].flag != 0 || fnt[1].flag != 0 || fnt[2].flag != 0 || fnt[3].flag != 0 || fnt[4].flag != 0)
		{
			

			
	
			BeginBatchDraw();//双缓冲
			cleardevice();//清屏
			putimage(0, 70, &img_bkg);
			setfillcolor(RGB(144, 99, 164));
			solidrectangle(0, 0, getwidth(), 70);
			
			//配置透明通道
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

			EndBatchDraw();


		}


	}

	return 0;
}






