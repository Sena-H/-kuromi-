
/*编写一个计算器的页面
第一面 选择计算器计算形状的种类 在点击不同的形状后跳转到不同的页面
第二页计算正方形的面积 并画出正方形
第三页计算长方形的面积 并画出长方形
第四页计算圆的面积 并画出圆
存在的问题：
1没有具体的计算公式
2存在没有使用的代码，后续需要删除保持精确简洁
3没有丰富的交互
4文字排版是乱排的，需要后续重新排版
5还没有实现计算完后按空格或者其他键回到第一面
6还没有计算按键的位置，没有设置捕捉鼠标点击按键进行跳转即isinrect函数
7具体的跳转操作是没有调试的，只是提供设计ui的思路，需要在完成调试后重新debug
8没有调试一定存在大量bug---汪家怡第一版


*/

#include <iostream>
#include <iostream>
#include<easyx.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#include<graphics.h>
struct Resourse{IMAGE img_start[2];}res;//这里存放要使用当作背景的图片
struct square{
    float d;
    float s;
}squ;
struct rectangle {
    float d1;
    float d2;
    float s;
}rec;
struct triangle {
    float h;
    float d;
    float s;
}tri;
struct circle {
    float r;
    float s;

}cir;//导入公式
void calculatesquare(struct square)
{}
void calculaterectangle(struct rectangle)
{}
void calculatetriangle(struct triangle)
{}
void calculatecircle(struct circle)
{}
void res_init(struct Resourse*res){
    loadimage(res->img_start + 0, "resourse/image/01.png", getwidth(), getheight());

    loadimage(res->img_start + 1, "resourse/image/02.png", getwidth(), getheight());
   

}//导入图片,记得改属性高级字符集，不然会报错

int which = 0;
void isinrect(ExMessage* msg){
    




}//判断鼠标的位置是否在所需要的区域 

    

int main()
{
   
    initgraph(800, 550, EX_SHOWCONSOLE);
    res_init(&res);
    //第一个画面 选择要计算的类型
    putimage(0, 0, res.img_start + 0); 
    while (true) {
        ExMessage msg;//不停的获取信息
     
        while (which == 0) { 
       isinrect(&msg);//判断which
        }

        while (which != 0&&peekmessage(&msg,EM_MOUSE))//鼠标点击
        {
            putimage(0, 0, res.img_start + 1);
         while(which==1){
             settextstyle(36, 0, "楷体");
             char str[50];
             
             
             outtextxy(226, 309, str);
             sprintf_s(str, "正方形的边长是:%d");
             outtextxy(226,425, str);
             sprintf_s(str, "正方形的面积是:%d");
             std::cin >> squ.d;
             outtextxy(250, 309, str);
             sprintf_s(str, "/f",squ.d);
             void calculatesquare();//这个地方要通过前面定义的函数squ.d计算出squ.s
             outtextxy(250, 425, str);
             sprintf_s(str, "/f", squ.s);
             setcolor(RGB(219, 207, 124));
             solidrectangle(100, 100, 100 + squ.d, 100 + squ.d);//这个地方是绘制出你计算的正方形的面积

         }
         while(which==2){
             settextstyle(36, 0, "楷体");
             char str[50];


             outtextxy(226, 309, str);
             sprintf_s(str, "长方形的长是:%d");
             outtextxy(500, 309, str);
             sprintf_s(str, "长方形的宽是:%d");
             outtextxy(226, 425, str);
             sprintf_s(str, "长方形的面积是:%d");
             std::cin >> rec.d1;
             outtextxy(300, 309, str);
             sprintf_s(str, "/f", rec.d1);
             std::cin >> rec.d2;
             outtextxy(574,309, str);
             sprintf_s(str, "/f", rec.d2);

             void calculaterectangle();//这个地方要通过前面定义的函数squ.d计算出squ.s
             outtextxy(300, 425, str);
             sprintf_s(str, "/f", rec.s);
             setfillcolor(RGB(121, 72, 200));
             solidrectangle(100, 100, 100 + rec.d1, 100 + rec.d2);//这个地方是绘制出你计算的正方形的面积


         }
         while(which==3){
             settextstyle(36, 0, "楷体");
             char str[50];


             outtextxy(226, 309, str);
             sprintf_s(str, "圆的半径是:%d");
             outtextxy(500, 309, str);
             
             sprintf_s(str, "圆的面积是:%d");
             std::cin >> cir.r;
             outtextxy(300, 309, str);
             sprintf_s(str, "/f", cir.r);
            

             void calculatecircle();
             outtextxy(300, 425, str);
             sprintf_s(str, "/f", cir.s);
             setfillcolor(RGB(200, 105, 72));
             solidcircle(200, 200, cir.r);
         
         }
         while(which==4){
             settextstyle(36, 0, "楷体");
             char str[50];


             outtextxy(226, 309, str);
             sprintf_s(str, "三角形的底是:%d");
             outtextxy(500, 309, str);
             sprintf_s(str, "三角形的高是:%d");
             outtextxy(226, 425, str);
             sprintf_s(str, "三角形的面积是:%d");
             std::cin >> tri.d;
             outtextxy(300, 309, str);
             sprintf_s(str, "/f", tri.d);
             std::cin >> tri.h;
             outtextxy(574, 309, str);
             sprintf_s(str, "/f", tri.h);

             void calculatetriangle();//这个地方要通过前面定义的函数squ.d计算出squ.s
             outtextxy(300, 425, str);
             sprintf_s(str, "/f", tri.s);
             setfillcolor(RGB(126, 200, 72));
             POINT pts[] = { {200, 200}, {200+tri.d, 200}, {200+tri.d/2, 200-tri.h} };
             fillpolygon(pts, 3);

             
         }
      
        }
        
    }return 0;
}

