#ifndef DK_GUI_H_
#define DK_GUI_H_


#ifdef UNICODE
#undef UNICODE
#endif // UNICODE


#include<easyx.h>

#ifdef _START_ID_
#define GUID __LINE__ + _START_ID_
#else
#define GUID __LINE__
#endif // !_START_ID_

typedef IMAGE Texture;

/**
 * 矩形.
 */
struct Rect
{
	int x;
	int y;
	int w;
	int h;
};

/**
 * 点.
 */
struct Point
{
	int x;
	int y;
};

struct Size
{
	int w;
	int h;
};

enum Alignment
{
	AlignTop = 1,
	AlignBottom = 2,
	AlignLeft = 4,
	AlignRight = 8,
	AlignHCenter = 16,
	AlignVCenter = 32,
	AlignCenter = AlignHCenter | AlignVCenter,
};

/*设置文本字体*/
void setTextFont(int size, const char* family);

/*按钮*/
bool GUI_button(int id, Rect rect, const char* text);
bool GUI_textureButton(int id, Point pos, Texture* normalImg, Texture* hoverImg);
bool GUI_lucencyButton(int id, Rect rect,bool test = false);

/*输入框*/
bool GUI_edit(int id,Rect rect,char* buffer,size_t len);

/*文本框*/
void GUI_label(Rect rect, const char* text,COLORREF color = BLACK,int aliginment = AlignCenter);

/*对话框*/
bool GUI_dialog(const char* text, Rect rect, Rect* clientRect);
#define GUI_DialogEx(title,width,height) \
	Rect rect;\
	if(GUI_dialog(title,{ (getwidth() - (width)) / 2,(getheight() - (height)) / 2,(width),(height) },&rect))

#define GUI_Dialog(title) GUI_DialogEx(title,350,250)


/*helper*/
bool pointInRect(Point pos, Rect rect);
void drawTexture(int x, int y, Texture* src);
void drawTexture(int x, int y, int dstW, int dstH, Texture* src, int srcX, int srcY);
void drawRoundRect(Rect* rect);
void drawRoundRect(Rect* rect, int ellipsewidth, int ellipseheight);		

/*加载图片*/
bool loadTexture(Texture* img,const char* filenmae,int w = 0,int h = 0);
Texture* loadTexture(const char* filename, int w = 0, int h = 0);
Texture* cacheTexture(const char* name, const char* filename,int w = 0,int h =0);
Texture* getTexture(const char* name);
Size getTextureSize(Texture* tex);

/*UIState*/
void GUI_init();
void GUI_clean();
void GUI_update(ExMessage* msg);
void GUI_prepare();
void GUI_finish();

/* 帧率控制 */
void startFrame();
void endFrame();
void setFps(float fps);	/*!> 帧率设置 ，example 1000/60.0 */

/*GUIStyle*/
typedef struct GUI_Color
{
	COLORREF color;
	COLORREF hoverColor;
}GUI_Color;

typedef struct GUI_Border
{
	COLORREF color;
	COLORREF hoverColor;
	int width;		//边框宽度
	int radius;		//圆角半径
}GUI_Border;

typedef struct GUI_Text
{
	COLORREF color;
	COLORREF hoverColor;
	int size;				//文字大小
	const char* font;		//字体
}GUI_Text;

typedef struct GUIStyle
{
	GUI_Color  background;
	GUI_Text   text;
	GUI_Border border;
	GUI_Color  focus;
}GUIStyle;

extern  GUIStyle ButtonStyle;	//! 按钮样式
extern  GUIStyle EditStyle;		//! 编辑框样式

void saveStyle();			//保存样式
void restoreStyle();		//恢复样式


#endif // !DK_GUI_H_


