#include <graphics.h>
#include <direct.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define X 1
#define Y 2
#define TH_SIZE 10

struct str{
	int num;
	char *title;
	char *thing;
};
struct str th[10];

struct info{
	int page;//标签数
	int box_y;//分割线位置
	int color_top;//上部颜色
	int color_btm;//下部颜色
	int screen_x;//界面大小X
	int screen_y;//界面大小Y
	int CS_x;
	int CS_y;
	int mouse_speed;
}inf;
time_t rawtime;
struct tm * timeinfo;
int loop();

char * intThing();//修改标签
char * intTitle();//修改标题
void putTitle(char *str,int color);//输出标题
void putThing(int screen_x,int screen_y,int box_y,char *str,int color);//输出标签
void drawMouse(int x,int y,int color);//绘制鼠标
void SetTransparent(HWND hwnd, UINT alpha);
void startInf();//初始化配置
bool isLButton();
int mouse(int type);//取出鼠标的坐标信息，type=X或者Y,输入其他参数返回全为0
int moveWindow(HWND hwnd);//鼠标左键点击屏幕，可以直接拖动当前窗口,传入当前窗口句柄，无论点击当前窗口任何地方都可以拖动，返回移动后的窗口位置:格式X*10000+Y
int windowAction(int w,int h);//改变当前窗口大小
char * openStringBox(const char *title);//打开输入字符串窗口
void doubleThing(int page);//处理双击事件
void view();//box_y为中间分线，color_top上部分颜色，color_btm为下部分颜色，screen_x,，screen_y界面大小
bool isDoubleLbutton();//可以手动调节速度，最小速度必须大于2*1000/fps
void getTime();
void putTime();

void
SetTransparent(HWND hwnd, UINT alpha)
{
	::SetWindowLong(hwnd, GWL_EXSTYLE,
		::GetWindowLong(hwnd,GWL_EXSTYLE) | 0x80000L);

	if(alpha == 0)
	{
		setbkcolor(BLACK);
		::SetLayeredWindowAttributes(hwnd, BLACK, 255, 0x3);
		cleardevice();
	}
	else
		::SetLayeredWindowAttributes(hwnd, 0, alpha, 0x2);
}

void drawMouse(int x,int y,int color)
{
	setcolor(color);
	line(x+5,y,x+5,y+9);
	line(x,y+5,x+9,y+5);
	setfillcolor(color);
	if(isLButton())
		setfillcolor(RED);
	int pt[]={
		x,y,
		x+5,y,
		x+5,y+5,
		x,y+5
	};
	fillpoly(4,pt);
}
void startInf()
{
	inf.screen_x=200;
	inf.screen_y=150;
	inf.CS_x = GetSystemMetrics(SM_CXSCREEN)-300;
	inf.CS_y = GetSystemMetrics(SM_CYSCREEN);
	setinitmode(INIT_TOPMOST|INIT_NOBORDER,inf.CS_x,0);
	initgraph(inf.screen_x,inf.screen_y);
	setcaption("easyNote");
	setbkmode(1);
	setbkcolor(YELLOW);
	setfont(20,0,"宋体");
	showmouse(0);
	th[0].num=0;
	inf.page=1;
	inf.box_y=20;
	inf.color_top=YELLOW;
	inf.color_btm=RGB(199,233,131);
	inf.mouse_speed=200;
}
void putTitle(char *str,int color)
{
	static int x=1.0,state=1;
	static int pos=1;
	int len=0;
	setcolor(color);
	setfont(inf.box_y-2,0,"黑体");
	xyprintf(x,2,"%s",str);
	if(str)
	{
		len=strlen(str);
		x==0?pos=-pos,x++,state++:(x==inf.screen_x-(inf.box_y/2)*len?pos=-pos,x--,state++:(state/5==1?x=x+pos,state=0:state++));
	}
}
void putThing(int screen_x,int screen_y,int box_y,char *str,int color)
{
	setcolor(color);
	setfont(20,0,"宋体");
	outtextrect(0,box_y,screen_x,screen_y,str);
}
void usedDoubleThing()
{

}
void doubleThing(int page)
{
	int x=mouse(X),y=mouse(Y);
	if (isDoubleLbutton())
	{
		if (x>0&&y>0&&x<inf.screen_x&&y<inf.box_y)
			th[page].title=intTitle();
		if (x>0&&y>inf.box_y&&x<inf.screen_x&&y<inf.screen_y)
			th[page].thing=intThing();
	}
}
char * intTitle()
{
	char *str=(char *)malloc(256 * sizeof(char));
	str=openStringBox("请输入标题");
	return str;
}
char * intThing()
{
	char *str=(char *)malloc(256 * sizeof(char));
	str=openStringBox("请输入你需要执行的事件");
	return str;
}
int mouse(int type)
{
	int x=0,y=0;
	mousepos(&x,&y);
	if(type==X)
		return x;
	if(type==Y)
		return y;
	return 0;
}
int moveWindow(HWND hwnd)
{
	static int posx,posy;
	static int state1=1;
	int pleft,ptop,pright,pbottom;
	getviewport(&pleft,&ptop,&pright,&pbottom);
	if(isLButton())
	{
		if(state1==1)
		{
			posx=mouse(X);
			posy=mouse(Y);
			state1=0;
		}
		inf.CS_x=inf.CS_x+mouse(X)-posx;
		inf.CS_y=inf.CS_y+mouse(Y)-posy;
		MoveWindow(hwnd,inf.CS_x,inf.CS_y,pright,pbottom,false);
	}
	else
		state1=1;
	return 0;
}
int windowAction(int w,int h)
{
	cleardevice();
	initgraph(w,h);
	return 0;
}
char * openStringBox(const char *title)
{
	char *str = (char *)malloc(256 * sizeof(char));
	int x0,y0;
	int x,y;
	getviewport(&x0,&y0,&x,&y);
	cleardevice();
	initgraph(400,300);
	inputbox_getline(title,"输入文字，限制100字",str,100);
	cleardevice();
	initgraph(x,y);
	return str;
}
void view()
{
	setbkcolor(inf.color_top);
	setcolor(inf.color_btm);
	setfillcolor(inf.color_btm);
	int pt[]={
		0,inf.box_y,
		inf.screen_x,inf.box_y,
		inf.screen_x,inf.screen_y,
		0,inf.screen_y
	};
	fillpoly(4,pt);
}
bool isDoubleLbutton()//鼠标双击算法
{
	static int x,y;
	static int state=0;
	static int dTimeB=0;
	if (isLButton()&&state==0)
	{
		state=1;
		x=mouse(X);
		y=mouse(Y);
	}
	if ((!isLButton())&&state==1)
	{
		state=2;
		dTimeB=dTimeB+1000/getfps();
	}
	if ((!isLButton())&&state==2)
	{
		dTimeB=dTimeB+1000/getfps();
	}
	if (state==2&&isLButton())
	{
		x=mouse(X);
		y=mouse(Y);
		state=3;
		dTimeB=dTimeB+1000/getfps();
	}
	if (dTimeB>inf.mouse_speed&&state==2)
	{
		dTimeB=0;
		state=0;
	}
	if (dTimeB<=inf.mouse_speed&&state==3)
	{
		if(abs(mouse(X)-x)<4&&abs(mouse(Y)-y)<4)
		{
			state=0;
			dTimeB=0;
			return true;
		}
	}
	return false;
}
bool isLButton()
{
	 return keystate(VK_LBUTTON)?true:false;
}
void addPage()
{
	th[inf.page].num=inf.page-1;
	++inf.page;
}
void deletePage(int pageNum)
{
	int i=pageNum;
	for (;i<inf.page;i++)
	{
		th[i-1].num=th[i].num;
		th[i-1].thing=th[i].thing;
		th[i-1].title=th[i].title;
	}
	th[i-1].thing = {};
	th[i-1].title = {};
}
void getTime()
{
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
}
void putTime()
{
	static int x=1.0,state=1;
	static int pos=1;
	getTime();
	setfont(18,0,"宋体");
	xyprintf(x,inf.screen_y-20,"%d月%2d日%2d:%2d:%2d",timeinfo->tm_mon,timeinfo->tm_mday,
			timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
	x==0?pos=-pos,x++,state++:(x==70?pos=-pos,x--,state++:(state/5==1?x=x+pos,state=0:state++));
}

int main()
{
	startInf();
	HWND hwnd = ::FindWindowA(NULL,"easyNote");
	SetTransparent(hwnd,215);

	while(true)
	{
		view();
		setcolor(RED);
		moveWindow(hwnd);
		doubleThing(inf.page-1);
		putTitle(th[0].title,BLUE);
		putThing(inf.screen_x,inf.screen_y,inf.box_y,th[0].thing,RED);
		putTime();
		drawMouse(mouse(X),mouse(Y),WHITE);
		if(keystate(VK_ESCAPE)&&keystate(VK_LBUTTON))
			break;
		delay_fps(100);
		cleardevice();
	}
}

