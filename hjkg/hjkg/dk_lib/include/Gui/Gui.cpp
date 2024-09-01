#include "Gui.h"
#include "../Core/Dictionary.h"
#include <malloc.h>
#include <stdio.h>
#include <time.h>
/*style*/
GUIStyle ButtonStyle = { {RGB(240,240,240),RGB(166,166,166)},{BLACK ,BLACK ,16,"Arial"},{RGB(179,202,234),RGB(179,202,234),1,10},{RGB(196,213,255)} };
GUIStyle EditStyle = { {RGB(242,245,252),RGB(252,252,252)},{BLACK ,BLACK,16,"Arial"},{RGB(133,145,162),RGB(32,61,119),1,10},{RGB(252,252,252)} };

GUIStyle SaveButtonStyle;
GUIStyle SaveEditStyle;

struct DK
{
	dictionary* imageCache;
}dk;
void dk_init(DK* pthis);
void dk_clean(DK* pthis);
Texture* dk_addImage(DK* pthis, const char* name, Texture* image);
Texture* dk_getImage(DK* pthis, const char* name);

void dk_init(DK* pthis)
{
	pthis->imageCache = dict_new(10);
}
void dk_clean(DK* pthis)
{
	Dict_foreach(Pair, p, pthis->imageCache)
	{
		delete p.val;
	}
	dict_del(pthis->imageCache);
}
Texture* dk_addImage(DK* pthis, const char* name, Texture* image)
{
	 Texture* value = (Texture*)dict_get(pthis->imageCache, name, nullptr);
	 if (value)
	 {
		 printf("[dk_addImage]: name is <%s> image already exist~\n", name);
		 return value;
	 }
	dict_set(pthis->imageCache, name, image);
	return image;
}
Texture* dk_getImage(DK* pthis, const char* name)
{
	Texture* value = (Texture*)dict_get(pthis->imageCache, name, nullptr);
	if (!value)
	{
		printf("[dk_getImage]:name is <%s> image not exist~\n", name);
		return nullptr;
	}
	return value;
}



/*UI状态*/
struct GUIState
{
	//Mouse
	int mx;
	int my;
	bool mouseDown;

	int hotItem;	//鼠标当前悬停的item
	int activeItem;	//拥有鼠标焦点的item

	//key
	int kbdItem;		//拥有键盘焦点的item
	int keyEntered;		//当前按键
	int keyModifier;	//修饰符按键

	//char
	char ch;

	int lastWidget;
}state = { 0,0,false  };

void setTextFont(int size, const char* family)
{
	LOGFONT font = { 0 };
	font.lfHeight = size;
	strcpy_s(font.lfFaceName, family);
	settextstyle(&font);
}
/*控件*/
bool GUI_button(int id, Rect rect, const char* text)
{
	if (pointInRect({ state.mx, state.my }, rect))
	{
		state.hotItem = id;
		//如果没有获取焦点的item，并且鼠标按下了，就获取焦点
		if (state.activeItem == 0 && state.mouseDown)
			state.activeItem = id;
	}

	//如果鼠标在当前按钮上悬停
	if (state.hotItem == id)
	{
		//并且有焦点
		if (state.activeItem == id)
		{
			setfillcolor(ButtonStyle.focus.color);
			setlinecolor(ButtonStyle.focus.color);
		}
		else
		{
			setfillcolor(ButtonStyle.background.hoverColor);
			setlinecolor(ButtonStyle.border.hoverColor);
		}
		settextcolor(ButtonStyle.text.hoverColor);
	}
	else
	{
		setfillcolor(ButtonStyle.background.color);
		setlinecolor(ButtonStyle.border.color);
		settextcolor(ButtonStyle.text.color);
	}
	settextstyle(ButtonStyle.text.size, 0, ButtonStyle.text.font);
	setlinestyle(PS_SOLID, ButtonStyle.border.width);

	//绘制按钮
	//gDrawFunc(&rect);
	drawRoundRect(&rect, ButtonStyle.border.radius, ButtonStyle.border.radius);
	//绘制文本
	outtextxy(rect.x + (rect.w - textwidth(text)) / 2,
		rect.y + (rect.h - textheight(text)) / 2, text);


	//鼠标弹起时触发点击
	if (state.mouseDown == false && state.hotItem == id && state.activeItem == id)
		return true;

	return false;
}
/*bool GUI_button(int id, Rect rect, const char* text)
{
	//如果没有控件有键盘焦点，获取它
	if (state.kbdItem == 0)
		state.kbdItem = id;

	if (pointInRect({ state.mx, state.my }, rect))
	{
		state.hotItem = id;
		//如果没有获取焦点的item，并且鼠标按下了，就获取焦点
		if (state.activeItem == 0 && state.mouseDown)
			state.activeItem = id;
	}


	//如果鼠标在当前按钮上悬停
	if(state.hotItem == id)
	{
		//并且有焦点
		if (state.activeItem == id)
		{
			setfillcolor(BROWN);
			setlinecolor(ButtonPalette.focusColor);
			//setfillcolor(BLACK);
			//setlinestyle(PS_DASHDOT, 1);
			//setlinecolor(RED);
			//Rect r = { rect.x - 1,rect.y - 1,rect.w + 2,rect.h + 2 };
			//gDrawFunc(&r);
			//setlinestyle(PS_SOLID, 1);
		}
		else
		{
			setfillcolor(ButtonPalette.bgHoverColor);
			setlinecolor(ButtonPalette.borderHoverColor);
			//setfillcolor(gTheme->bgHoverColor);
			//setlinecolor(gTheme->borderHoverColor);
			//settextcolor(gTheme->fgHoverColor);
		}
		settextcolor(ButtonPalette.fgHoverColor);
	}
	else
	{
		setfillcolor(ButtonPalette.bgNormalColor);
		setlinecolor(ButtonPalette.borderNormalColor);
		settextcolor(ButtonPalette.fgNormalColor);
		//setfillcolor(gTheme->bgNormalColor);
		//setlinecolor(gTheme->borderNormalColor);
		//settextcolor(gTheme->fgNormalColor);
	}

	if (state.kbdItem == 0)
		state.kbdItem = id;

	if (state.kbdItem == id)
	{
		setfillcolor(ButtonPalette.bgNormalColor);
		setlinecolor(ButtonPalette.focusColor);
		//setfillcolor(BLACK);
		//setlinestyle(PS_DASHDOT, 1);
		//setlinecolor(RED);
		//Rect r = { rect.x - 1,rect.y - 1,rect.w + 2,rect.h + 2 };
		//gDrawFunc(&r);
		//setlinestyle(PS_SOLID, 1);
	}

	//绘制按钮
	gDrawFunc(&rect);
	//绘制文本
	outtextxy(rect.x + (rect.w - textwidth(text)) / 2,
		rect.y + (rect.h - textheight(text)) / 2, text);

	if (state.kbdItem == id)
	{
		switch (state.keyEntered)
		{
		case VK_TAB:
			state.kbdItem = 0;

			if (state.keyModifier == 1)
				state.kbdItem = state.lastWidget;

			state.keyEntered = 0;

			break;
		case VK_RETURN:
			return true;
			break;
		}
	}
	state.lastWidget = id;

	//鼠标弹起时触发点击
	if (state.mouseDown == false && state.hotItem == id && state.activeItem == id)
		return true;

	return false;
}*/

bool GUI_textureButton(int id,Point pos, Texture* normalImg, Texture* hoverImg)
{
	if (!normalImg)
		return false;
	bool inRect = pointInRect({ state.mx, state.my }, {pos.x,pos.y,normalImg->getwidth(),normalImg->getheight()});
	if (inRect)
	{
		state.hotItem = id;
		//如果没有获取焦点的item，并且鼠标按下了，就获取焦点
		if (state.activeItem == 0 && state.mouseDown)
			state.activeItem = id;		
	}

	//如果鼠标在当前按钮上悬停
	if (state.hotItem == id && hoverImg)
	{
		drawTexture(pos.x - 1, pos.y - 1, hoverImg);
	}
	else
	{
		drawTexture(pos.x, pos.y, normalImg);
	}

	//鼠标弹起时触发点击
	if (state.mouseDown == false && state.hotItem == id && state.activeItem == id)
		return true;

	return false;
}

bool GUI_lucencyButton(int id, Rect rect,bool test)
{
	bool inRect = pointInRect({ state.mx, state.my },rect);
	if (inRect)
	{
		state.hotItem = id;
		//如果没有获取焦点的item，并且鼠标按下了，就获取焦点
		if (state.activeItem == 0 && state.mouseDown)
			state.activeItem = id;
	}

	if (test)
	{
		COLORREF tmp = getlinecolor();
		//如果鼠标在当前按钮上悬停
		if (state.hotItem == id)
		{
			setlinecolor(ButtonStyle.border.color);
			rectangle(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
		}
		else
		{
			setlinecolor(ButtonStyle.border.hoverColor);
			rectangle(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
		}
		setlinecolor(tmp);
	}


	//鼠标弹起时触发点击
	if (state.mouseDown == false && state.hotItem == id && state.activeItem == id)
		return true;

	return false;
}

bool GUI_edit(int id, Rect rect, char* buffer, size_t size)
{
	if (pointInRect({ state.mx, state.my }, rect))
	{
		state.hotItem = id;
		if (state.activeItem == 0 && state.mouseDown)
		{
			state.activeItem = id;
		}
	}


	if (state.activeItem == id || state.hotItem == id)
	{
		setfillcolor(EditStyle.background.hoverColor);
		setlinecolor(EditStyle.border.hoverColor);
		settextcolor(EditStyle.text.hoverColor);
	}
	else
	{
		setfillcolor(EditStyle.background.color);
		setlinecolor(EditStyle.border.color);
		settextcolor(EditStyle.text.color);
	}
	settextstyle(EditStyle.text.size, 0, EditStyle.text.font);
	setlinestyle(PS_SOLID, EditStyle.border.width);

	//如果没有控件有键盘焦点，获取它
	if (state.kbdItem == 0)
		state.kbdItem = id;

	if (state.kbdItem == id)
	{
		setfillcolor(EditStyle.background.hoverColor);
		setlinecolor(EditStyle.border.hoverColor);
		settextcolor(EditStyle.text.hoverColor);
	}

	//绘制按钮
	//gDrawFunc(&rect);
	drawRoundRect(&rect, EditStyle.border.radius, EditStyle.border.radius);
	//绘制文本
	outtextxy(rect.x + 5,
		rect.y + (rect.h - textheight(buffer)) / 2, buffer);

	int len = strlen(buffer);
	bool changed = false;
	if (state.kbdItem == id && (GetTickCount() >> 9) & 1)
		outtextxy(rect.x +5 + textwidth(buffer), rect.y + (rect.h - textheight("|")) / 2, "|");


	if (state.kbdItem == id)
	{
		switch (state.keyEntered)
		{
		case VK_TAB:
			state.kbdItem = 0;
			//按tab键，让焦点切换到下一个输入框
			if (state.keyModifier == 1)
				state.kbdItem = state.lastWidget;

			state.keyEntered = 0;
		
			break;
		case '\b':
			//起码还有两个字符，而且这两个字符组成一个中文，就删除最后两个（中文每个字节都是负数）
			if (len > 1 && buffer[len - 1] < 0 && buffer[len - 2] < 0)
			{
				buffer[len - 2] = '\0';
				changed = true;
			}
			//删除一个字符
			else if (len > 0)
			{
				buffer[len - 1] = '\0';
				changed = true;
			}
			
			break;
		case '\r':
		case '\n':
			state.kbdItem = -1;	//清除焦点
			return changed;
			break;
		default:
			//最多只能输入size-1长度
			if (len < size)
			{
				//只接受可打印字符和中文输入
				if ((state.ch >= 32 && state.ch < 127) || state.ch < 0)
				{
					buffer[len++] = state.ch;
					buffer[len] = '\0';
					changed = true;
				}
			}

			break;
		}
	}

	if (state.mouseDown == false &&
		state.hotItem == id &&
		state.activeItem == id)
	{
		state.kbdItem = id;
	}
	state.lastWidget = id;
	return changed;
}

void GUI_label(Rect rect, const char* text, COLORREF color, int aliginment)
{
	int tx = rect.x, ty = rect.y;
	if (rect.w != 0 && rect.h != 0)
	{
		int tw = textwidth(text);
		int th = textheight(text);

		int hspace = (rect.w - tw) / 2;
		int vspace = (rect.h - th) / 2;
		//对齐方式
		switch (aliginment)
		{
		case  AlignTop:
			ty = rect.y;
			break;
		case  AlignBottom:
			ty = rect.y + rect.h - th;
			break;
		case  AlignLeft:
			tx = rect.x;
			break;
		case  AlignRight:
			tx = rect.x + rect.w - tw;
			break;
		case  AlignHCenter | AlignTop:
			tx = rect.x + hspace;
			ty = rect.y;
			break;
		case  AlignHCenter | AlignBottom:
			tx = rect.x + hspace;
			ty = rect.y + rect.h - th;
			break;
		case  AlignVCenter | AlignLeft:
			ty = rect.y + vspace;
			tx = rect.x;
			break;
		case  AlignVCenter | AlignRight:
			ty = rect.y + vspace;
			tx = rect.x + rect.w - tw;
			break;
		case  AlignCenter:
			tx = rect.x + hspace;
			ty = rect.y + vspace;
			break;
		default:
			tx = rect.x;
			ty = rect.y;
			break;
		}
	}
	settextcolor(color);
	outtextxy(tx,ty, text);
}

bool GUI_dialog(const char* title, Rect rect, Rect* clientRect)
{
	//绘制对话框
	setfillcolor(ButtonStyle.background.color);
	setlinecolor(RGB(82, 82, 82));
	drawRoundRect(&rect, 10, 10);

	//绘制阴影
	for (int i = 1; i < 10; i++)
	{
		//213 244
		int c = 160 + i * 8;
		setlinecolor(RGB(c,c,c));
		roundrect(rect.x-i, rect.y-i, rect.x + rect.w +i, rect.y + rect.h+i,10,10);
	}
	
	//绘制标题栏下面的线条
	setlinecolor(ButtonStyle.border.color);
	line(rect.x, rect.y +33, rect.x + rect.w, rect.y + 33);

	//绘制标题
	settextstyle(18, 0, "Arial");
	GUI_label({ rect.x + 5,rect.y + 8,textwidth(title),39 }, title, BLACK, AlignVCenter);

	
	//计算客户区矩形
	*clientRect = { rect.x + 1,rect.y + 33,rect.w - 2,rect.h - 33 };
	//添加关闭按钮
	if (GUI_button(GUID, { rect.x + rect.w - 33,rect.y+1,32,32 }, "X"))
	{
		return true;
	}
	return false;
}

bool pointInRect(Point pos, Rect rect)
{
	return pos.x >rect.x && pos.x<rect.x+rect.w && pos.y>rect.y && pos.y <rect.y + rect.h;
}

void drawRoundRect(Rect* rect)
{
	fillroundrect(rect->x, rect->y, rect->x + rect->w, rect->y + rect->h,10,10);
}

void drawRoundRect(Rect* rect, int ellipsewidth, int ellipseheight)
{
	fillroundrect(rect->x, rect->y, rect->x + rect->w, rect->y + rect->h, ellipsewidth, ellipseheight);
}

bool loadTexture(Texture* img, const char* filenmae, int w, int h)
{
	loadimage(img, filenmae, w, h);
	if (img->getwidth() == 0 || img->getheight() == 0)
	{
		printf("[loadTexture]:<%s> load failed:%s", filenmae, "请检查路径或文件名是否正确\n");
		return false;
	}
	return true;
}

Texture* loadTexture(const char* filename, int w, int h)
{
	Texture* img = new Texture;
	if (loadTexture(img, filename, w, h))
	{
		return img;
	}
	delete img;
	return nullptr;
}

Texture* cacheTexture(const char* name, const char* filename,int w,int h)
{
	if (!dk.imageCache)
	{
		printf("Call the <GUI_Init> function first\n");
		return nullptr;
	}
	return dk_addImage(&dk, name, loadTexture(filename,w,h));
}

Texture* getTexture(const char* name)
{
	if (!dk.imageCache)
	{
		printf("Call the <GUI_Init> function first\n");
		return nullptr;
	}
	return dk_getImage(&dk,name);
}

Size getTextureSize(Texture* tex)
{
	if (!tex)
		return { 0,0 };
	return Size{ tex->getwidth(),tex->getheight() };;
}

/*状态函数*/
void GUI_init()
{
	setbkcolor(RGB(232, 232, 236));
	setbkmode(TRANSPARENT);
	setTextFont(18, "Arial");
	dk_init(&dk);
}

void GUI_clean()
{
	dk_clean(&dk);
}

void GUI_update(ExMessage* msg)
{
	if (msg->message == WM_MOUSEMOVE)
	{
		state.mx = msg->x;
		state.my = msg->y;
	}
	else if(msg->message == WM_LBUTTONDOWN)
	{
			state.mouseDown = true;
	}
	else if (msg->message == WM_LBUTTONUP)
	{
			state.mouseDown = false;
	}
	else if (msg->message == WM_CHAR)
	{
		state.ch = msg->ch;
	}
	else if (msg->message == WM_KEYDOWN)
	{
		state.keyEntered = msg->vkcode;
		state.keyModifier = msg->ctrl ? 1 : (msg->shift ? 2 : 0);
	}
	else if (msg->message == WM_KEYUP)
	{
	}
}

void GUI_prepare()
{
	BeginBatchDraw();
	cleardevice();

	//每帧开始时，重置鼠标悬停
	state.hotItem = 0;
}

void GUI_finish()
{
	EndBatchDraw();
	
	//如果鼠标没有按下
	if (!state.mouseDown)
	{
		//取消焦点
		state.activeItem = 0;
	}
	//如果鼠标按下了
	else
	{
		if (state.activeItem == 0)
			state.activeItem = -1;
	}

	if (state.keyEntered == VK_TAB)
		state.kbdItem = 0;
	state.keyEntered = 0;
	state.ch = 0;
}

/* 帧率控制*/
struct FrameRoate
{
	clock_t startTicks;
	clock_t frameTicks;
	float fps;
}frameRoate = {0,0,1000/60.0f};

void startFrame()
{
	frameRoate.startTicks = clock();
}

void endFrame()
{
	frameRoate.frameTicks = clock() - frameRoate.startTicks;
	//printf("%d\n", frameRoate.frameTicks);
	if (frameRoate.fps - frameRoate.frameTicks > 0)
	{
		Sleep(frameRoate.fps - frameRoate.frameTicks);
	}
}

void setFps(float fps)
{
	frameRoate.fps = fps;
}


void saveStyle()
{
	SaveButtonStyle = ButtonStyle;
	SaveEditStyle = EditStyle;
}

void restoreStyle()
{
	ButtonStyle = SaveButtonStyle;
	EditStyle = SaveEditStyle;
}

void drawTexture(int x, int y, Texture* src)
{
	if (!src)
		return;
	// 变量初始化
	DWORD* pwin = GetImageBuffer();			//窗口缓冲区指针
	DWORD* psrc = GetImageBuffer(src);		//图片缓冲区指针
	int win_w = getwidth();				//窗口宽高
	int win_h = getheight();
	int src_w = src->getwidth();				//图片宽高
	int src_h = src->getheight();

	// 计算贴图的实际长宽
	int real_w = (x + src_w > win_w) ? win_w - x : src_w;			// 处理超出右边界
	int real_h = (y + src_h > win_h) ? win_h - y : src_h;			// 处理超出下边界
	if (x < 0) { psrc += -x;			real_w -= -x;	x = 0; }	// 处理超出左边界
	if (y < 0) { psrc += (src_w * -y);	real_h -= -y;	y = 0; }	// 处理超出上边界


	// 修正贴图起始位置
	pwin += (win_w * y + x);

	// 实现透明贴图
	for (int iy = 0; iy < real_h; iy++)
	{
		for (int ix = 0; ix < real_w; ix++)
		{
			byte a = (byte)(psrc[ix] >> 24);//计算透明通道的值[0,256) 0为完全透明 255为完全不透明
			if (a > 100)
			{
				pwin[ix] = psrc[ix];
			}
		}
		//换到下一行
		pwin += win_w;
		psrc += src_w;
	}
}

void drawTexture(int x, int y, int dstW, int dstH, Texture* src, int srcX, int srcY)
{
	if (!src)
		return;
	// 变量初始化
	DWORD* pwin = GetImageBuffer();			//窗口缓冲区指针
	DWORD* psrc = GetImageBuffer(src);		//图片缓冲区指针
	int win_w = getwidth();				//窗口宽高
	int win_h = getheight();
	int src_w = src->getwidth();				//图片宽高
	int src_h = src->getheight();


	// 计算贴图的实际长宽
	int real_w = (x + dstW > win_w) ? win_w - x : dstW;			// 处理超出右边界
	int real_h = (y + dstH > win_h) ? win_h - y : dstH;			// 处理超出下边界
	if (x < 0) { psrc += -x;			real_w -= -x;	x = 0; }	// 处理超出左边界
	if (y < 0) { psrc += (dstW * -y);	real_h -= -y;	y = 0; }	// 处理超出上边界

	//printf("realw,h(%d,%d)\n", real_w, real_h);
	// 修正贴图起始位置
	pwin += (win_w * y + x);

	// 实现透明贴图
	for (int iy = 0; iy < real_h; iy++)
	{
		for (int ix = 0; ix < real_w; ix++)
		{
			byte a = (byte)(psrc[ix + srcX + srcY * src_w] >> 24);//计算透明通道的值[0,256) 0为完全透明 255为完全不透明
			if (a > 100)
			{
				pwin[ix] = psrc[ix + srcX + srcY * src_w];
			}
		}
		//换到下一行
		pwin += win_w;
		psrc += src_w;
	}
}


//https://tieba.baidu.com/p/2578675430?red_tag=0281760964   //easyx指定颜色透明

