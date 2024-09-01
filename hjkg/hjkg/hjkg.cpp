

#include <iostream>
#include<easyx.h>
#include<Gui/Gui.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#define TwoPointDistance(x1,y1,x2,y2) sqrt(pow(x1-x2,2)+pow(y1-y2,2))
#define FNT_NUM 5 //黄金矿工执行时的随机家具数量
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")
enum Swingdir
{
	Left,
	Right
};//表示钩子的状态，来编写钩子的摇晃+
enum State
{
	Norml,
	Short,
	Long,
    Hitwall};
 int money=0;
int which = 0;
bool game_on = 0;
     struct Hook
{
	int x;
	int y;
	int len;
	int endx;
	int endy;
	float radian;//定义弧度
	Swingdir  swingdir ;
	State state;
	int dx;
	int dy;
	int index;
	int clawx;
	int clawy;
}hook; 
	 struct Fnt
 {
	 int x;
	 int y;
	 int size;
	 int type;//方便写随机
	 int value;
	 bool flag;//是否存在
	 int lastgrasp;
 }fnt[5];
	 struct Resource {
		 IMAGE img_start[8];
	 }res;
	
//初始化
void res_init(struct Resource*res)//不表明struct的话就引用不了res->
{
	//数组的查询方式+0
	loadimage(res->img_start + 0, "resourse/image/01.jpg",getwidth(),getheight());

	loadimage(res->img_start + 1, "resourse/image/02.jpg", getwidth(), getheight());
	loadimage(res->img_start + 2, "resourse/image/03.jpg", getwidth(), getheight());
	loadimage(res->img_start + 3, "resourse/image/04.jpg", getwidth(), getheight());
	loadimage(res->img_start + 4, "resourse/image/05.jpg", getwidth(), getheight());

	loadimage(res->img_start + 5, "resourse/image/06.jpg", getwidth(), getheight());
	loadimage(res->img_start + 6, "resourse/image/07.jpg", getwidth(), getheight());
	loadimage(res->img_start + 7, "resourse/image/08.jpg", getwidth(), getheight());
	}
//鼠标点击切换场景


void starupscene(ExMessage* msg)
{//表示现在是哪个画面
	
	if (msg->message == WM_LBUTTONDOWN&&which<7)
	{
			++which;
			mciSendString(_T("play bgm01 from 120"), NULL, 0, NULL);
			switch(which){
				case 2: { mciSendString(_T("play goodfriend from 0"), NULL, 0, NULL); break; }
				case 4: { mciSendString(_T("play badluck from 0"), NULL, 0, NULL);break; }
				case 5: {mciSendString(_T("play worst from 0"), NULL, 0, NULL);break;}
				case 6: { mciSendString(_T("play steal from 0"), NULL, 0, NULL);break; }
			}
	}
	
}
//判断鼠标是否在矩形区域
bool isinrect(ExMessage*msg,int x,int y,int w,int h)
{   if(msg->x>x&&msg->y>y&&msg->x<x+w&&msg->y<y+h)
{
	return true;
}
	return false;
}
void hook_init(Hook* pthis)
 { 
 pthis->len = 30;
	 pthis->x = 400;
	 pthis->y =120;
	 pthis->endx = pthis->x;
	 pthis->endy =pthis-> y+pthis->len;
	 pthis->swingdir = Right;
	 pthis->state = Norml;
	 pthis->index = -1;
	 pthis->clawx = pthis->endx-13;
	 pthis->clawy = pthis->endy + pthis->len-
		 13;
}
 IMAGE img_fnt[5];//单独在struct外面定义的，不需要Fnt->img_fnt
 IMAGE img_claw[1];
 
 void fnt_init()
 {
	 

	 for (int i = 0;i < FNT_NUM;i++)

	 {

		 fnt[i].x = 145 * i + rand() % 25 + 40;


		 fnt[i].y = rand() % 230 + 170;
		 fnt[i].type = rand() % 5;
		 
	 }
	
	 
	 for (int i = 0;i < FNT_NUM;i++) {
		 if (fnt[i].type == 0) { fnt[i].value = 100;fnt[i].size = 200; }

		 else if (fnt[i].type == 1) { fnt[i].value = 30; fnt[i].size = 76; }
		 else if (fnt[i].type == 2) { fnt[i].value = 200;fnt[i].size = 150; }
		 else if (fnt[i].type == 3) { fnt[i].value = 70;fnt[i].size = 140; }
		 else if (fnt[i].type == 4) { fnt[i].value = 300; fnt[i].size = 140; }
	 }
	
			 loadimage(img_fnt + 0, "resourse/image/sofa.png",200,100);
			 
			 loadimage(img_fnt + 1, "resourse/image/cup.png",76,85);
			 loadimage(img_fnt + 2, "resourse/image/bed.png",150,150);
			 loadimage(img_fnt + 3, "resourse/image/guizi.png",140,190);
			 loadimage(img_fnt + 4, "resourse/image/box.png",140,160);
			 for (int i = 0;i < FNT_NUM;i++) {fnt[i].flag = 1; }
		 
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
	 line(pthis->x, pthis->y, pthis->endx, pthis->endy);//line就是写出头尾的坐标
	 setlinestyle(PS_SOLID, 5);
	 setlinecolor(BLACK);
	 line(pthis->x, 0, pthis->x, 48);
	 loadimage(img_claw, "resourse/image/claw.png");
	 drawTexture(hook.clawx,hook.clawy, img_claw);
 }
 void hook_swing (Hook*pthis)
 {if(pthis->state==Norml)//if里面不能用=，这是赋值的操作！
	  {
		 if (pthis->swingdir == Right)
		 {
			 pthis->radian += 0.0015;
		 }
		 else
		 {
			 pthis->radian -= 0.0015;
		 }
		 if (pthis->radian > 4 * 3.14 / 9)
		 {
			 pthis->swingdir = Left;
		 }
		 else if (pthis->radian < -4 * 3.14 / 9)
		 {
			 pthis->swingdir = Right;
		 }



		 hook.endx = hook.x + sin(pthis->radian) * pthis->len;
		 hook.endy = hook.y + cos(pthis->radian) * pthis->len;//control互斥
		 hook.clawx = hook.endx;hook.clawy = hook.endy;
	 }
 }
 void hook_control(Hook* pthis) {
	 int speed = 2;
	 if (GetAsyncKeyState(VK_SPACE)&&pthis->state==Norml)
	 {
		 pthis->state = Long;
		 pthis->dx = sin(pthis->radian) * speed;
		 pthis->dy = cos(pthis->radian) * speed;
	 }
	 if (pthis->state == Long)
	 {
		 pthis->endx += pthis->dx;
		 pthis->endy += pthis->dy;
	 }
	 else if (pthis->state == Short||pthis->state==Hitwall)
	 {
		 pthis->endx -= pthis->dx;
		 pthis->endy -= pthis->dy;
	 }
		if(TwoPointDistance(pthis->x,pthis->y,pthis->endx,pthis->endy)<=pthis->len)
	 {
		 pthis->state = Norml;
	 }
		if(hook.endx<0||hook.endx>getwidth()||hook.endy>getheight())
		{
			hook.state = Hitwall;
		} hook.clawx = hook.endx-13;hook.clawy = hook.endy-13;
	 }//需要else if终结if的条件
//如果碰到边界

  
	 
 void judgegrap()
 { 
	 for (int i = 0;i < FNT_NUM;i++)
	 {		
		 if (fnt[i].flag == true && hook.endx > fnt[i].x - 10 && hook.endx<fnt[i].x + fnt[i].size + 5 && hook.endy > fnt[i].y+5 && hook.endy < fnt[i].y + 150)
		 {
			 hook.index = i;money += fnt[i].value;
			 mciSendString(_T("play bgm02 from 300"), NULL, 0, NULL);
		 }
		 }
	
	 if(hook.index!=-1)
	 {
		 hook.state = Short;
		 fnt[hook.index].x = hook.endx;
		 fnt[hook.index].y = hook.endy;//家具和钩子一起走了
		 fnt->lastgrasp = hook.index;
		 if (TwoPointDistance(hook.x, hook.y, hook.endx, hook.endy) <=30)
		 {
			 hook.state = Norml;//抓到之后收回停在normal阶段
			fnt[hook.index].flag =false;
			
			 hook.index = -1;//在循环中恢复初始状态

		 } hook.clawx = hook.endx-13;hook.clawy = hook.endy-13;
		 
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
 void game_swing(Hook*pthis)
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
 
 
 int main()

 {
	 music_init();
	
	 mciSendString(_T("play bgm repeat from 1050"), NULL, 0, NULL);
	
	 mciSendString(_T("play title from 0"), NULL, 0, NULL);
	 initgraph(800, 550, EX_SHOWCONSOLE);
	 res_init(&res);
	 game_init();

	 IMAGE img_bkg;
	 loadimage(&img_bkg, "resourse/image/bkg.png");//导入背景
	
	 IMAGE  img_klm;
	 loadimage(&img_klm , "resourse/image/klm.png");
	 
 
	 while(which < 7) {
		 BeginBatchDraw();cleardevice();
		 
		 ExMessage msg;putimage(0, 0, res.img_start + which);
		 
		 //不断的处理消息 有消息就返回true 没消息返回false
		 while (peekmessage(&msg, EX_MOUSE)) {
			 
			 int mousex=msg.x;
			 int mousey = msg.y;
			 
		 }
		     loadimage(img_claw, "resourse/image/claw.png");
			 drawTexture(msg.x-10, msg.y-10, img_claw);EndBatchDraw();
			 
	     if (peekmessage(&msg, WH_MOUSE))// 进入新的步骤
		 {
			 starupscene(&msg);

		 } 
		 
	 }
	 while (true) {
		 cleardevice();
		 while (fnt[0].flag != 0 || fnt[1].flag != 0 || fnt[2].flag != 0 || fnt[3].flag != 0 || fnt[4].flag != 0)
		 {
			 BeginBatchDraw();//双缓冲
			 cleardevice();//清屏
			 putimage(0, 70, &img_bkg);
			 setfillcolor(RGB(144, 99, 164));
			 solidrectangle(0, 0, getwidth(), 70);
			 drawTexture((getwidth() - img_klm.getwidth()) / 2, 5, &img_klm);//放入人类调整人的位置
			 //配置透明通道



			 game_draw();
			 game_swing(&hook);
			 hook_control(&hook);
			 judgegrap();

			 EndBatchDraw();
		 }

		 
	 }
	 
	 return 0;
 }




