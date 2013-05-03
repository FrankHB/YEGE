#include "graphics.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ege/head.h"

class SceneBase
{
public:
	virtual SceneBase* Update()
	{
		return nullptr;
	}
	virtual ~SceneBase()
	{};
};

class SceneHelloWorld6 : public SceneBase
{
public:
	SceneHelloWorld6(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		setcolor(RED);
		arc(100, 100, 0, 180, 50);
		arc(200, 100, 0, 180, 50);
		line(50, 100, 150, 200);
		line(250, 100, 150, 200);
	}
	SceneBase* Update()
	{
		char str[] = "#include \"graphics.h\"\n\nint main()\n{\n	initgraph(640, 480);\n\n	setcolor(RED);\n	//�����ߣ���(100,100)ΪԲ�ģ�0�ȵ�180�ȣ��뾶50\n	arc(100, 100, 0, 180, 50);\n	//ͬ�����ߣ�ֻ��λ�ò�ͬ\n	arc(200, 100, 0, 180, 50);\n	//��(50,100)��(150,200)����\n	line(50, 100, 150, 200);\n	//��(250,100)��(150,200)����\n	line(250, 100, 150, 200);\n	getch(); //�ȴ��û��������൱����ͣ\n	return 0;\n}";
		setbkcolor_f(BLACK);
		cleardevice();

		smain();
		{
			setfont(12, 0, "����");
			setcolor(0x808080);
			line(320, 0, 320, 480);
			setcolor(0xFFFFFF);
			outtextrect(320, 100, 320, 380, str);
			outtextrect(320, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������");
		}
		getch();
		return m_parent;
	}
private:
	SceneBase* m_parent;
};

class SceneHelloWorld5 : public SceneBase
{
public:
	SceneHelloWorld5(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		setcolor(YELLOW);
		setfillstyle(SOLID_FILL, MAGENTA);
		fillellipse(150, 200, 50, 100);
	}
	SceneBase* Update()
	{
		char str[] = "#include \"graphics.h\"\n\nint main()\n{\n	//ͼ�δ��ڳ�ʼ��Ϊ640*480��С\n	initgraph(640, 480);\n\n	//������ɫΪ��ɫ\n	setcolor(YELLOW);\n	//���������ɫΪ�Ϻ�ɫ\n	setfillstyle(MAGENTA);\n	//��(150,200)ΪԲ�ģ�x�뾶Ϊ50��y�뾶Ϊ100����һ��ʵ����Բ\n	fillellipse(150, 200, 50, 100);\n\n	getch(); //�ȴ��û��������൱����ͣ\n	return 0;\n}";
		setbkcolor_f(BLACK);
		cleardevice();

		smain();
		{
			setfont(12, 0, "����");
			setcolor(0x808080);
			line(320, 0, 320, 480);
			setcolor(0xFFFFFF);
			outtextrect(320, 100, 320, 380, str);
			outtextrect(320, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������");
		}
		getch();
		return new SceneHelloWorld6(m_parent);
	}
private:
	SceneBase* m_parent;
};

class SceneHelloWorld4 : public SceneBase
{
public:
	SceneHelloWorld4(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		//setfillstyle(GREEN);
		bar(100, 100, 200, 400);
	}
	SceneBase* Update()
	{
		char str[] = "#include \"graphics.h\"\n\nint main()\n{\n	//ͼ�δ��ڳ�ʼ��Ϊ640*480��С\n	initgraph(640, 480);\n\n	//���������ɫΪ��ɫ��ע�������������ɫ\n	setfillstyle(GREEN);\n	//��(100,100)��(200,400)��һ��ʵ�ľ��Σ�ʹ�������ɫ\n	bar(100, 100, 200, 400);\n\n	getch(); //�ȴ��û��������൱����ͣ\n	return 0;\n}";
		setbkcolor_f(BLACK);
		cleardevice();

		smain();
		{
			setfont(12, 0, "����");
			setcolor(0x808080);
			line(320, 0, 320, 480);
			setcolor(0xFFFFFF);
			outtextrect(320, 100, 320, 380, str);
			outtextrect(320, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������");
		}
		getch();
		return new SceneHelloWorld5(m_parent);
	}
private:
	SceneBase* m_parent;
};

class SceneHelloWorld3 : public SceneBase
{
public:
	SceneHelloWorld3(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		setcolor(GREEN);
		circle(200, 100, 80);
	}
	SceneBase* Update()
	{
		char str[] = "#include \"graphics.h\"\n\nint main()\n{\n	//ͼ�δ��ڳ�ʼ��Ϊ640*480��С\n	initgraph(640, 480);\n\n	//������ɫΪ��ɫ\n	setcolor(GREEN);\n	//��x=200,y=100�ĵط�����һ���뾶80��Բ\n	circle(200, 100, 80);\n\n	getch(); //�ȴ��û��������൱����ͣ\n	return 0;\n}";
		setbkcolor_f(BLACK);
		cleardevice();

		smain();
		{
			setfont(12, 0, "����");
			setcolor(0x808080);
			line(320, 0, 320, 480);
			setcolor(0xFFFFFF);
			outtextrect(320, 100, 320, 380, str);
			outtextrect(320, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������");
		}
		getch();
		return new SceneHelloWorld4(m_parent);
	}
private:
	SceneBase* m_parent;
};

class SceneHelloWorld2 : public SceneBase
{
public:
	SceneHelloWorld2(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		circle(200, 100, 80);
	}
	SceneBase* Update()
	{
		char str[] = "#include \"graphics.h\"\n\nint main()\n{\n	//ͼ�δ��ڳ�ʼ��Ϊ640*480��С\n	initgraph(640, 480);\n\n	//��x=200,y=100�ĵط�����һ���뾶80��Բ\n	circle(200, 100, 80);\n\n	getch(); //�ȴ��û��������൱����ͣ\n	return 0;\n}";
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);

		smain();
		{
			setfont(12, 0, "����");
			setcolor(0x808080);
			line(320, 0, 320, 480);
			setcolor(0xFFFFFF);
			outtextrect(320, 100, 320, 380, str);
			outtextrect(320, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������");
		}
		getch();
		return new SceneHelloWorld3(m_parent);
	}
private:
	SceneBase* m_parent;
};

class SceneHelloWorld : public SceneBase
{
public:
	SceneHelloWorld(SceneBase* parent)
	{
		m_parent = parent;
	}
	SceneBase* Update()
	{
		char str[] = "//������б��\'//\'��ʼ���������Ϊע�ͣ���Ӱ�����\n//���������PowerEasyXͼ�ο��ͷ�ļ���������TCͼ�ε�ͷ�ļ�����ע��\n//Ҫ��ȷ���뱾��������Ϊ���VC����C-Free��װ��PEX\n���˰������ͷ�ļ��󣬾Ϳ���ʹ��ͼ�κ�����\n#include \"graphics.h\"\n\nint main() //��ʹ��int����main����Ϊ�淶\n{\n	//ͼ�δ��ڳ�ʼ��Ϊ640*480��С\n	initgraph(640, 480);\n\n	//��������߶�Ϊ20�����ΪĬ��ֵ��������\n	setfont(20, 0, \"����\");\n\n	//��x=100,y=0�ĵط���ʼ����ʾһ������\n	outtextxy(100, 0, \"Hello World\");\n\n	//�ȴ��û��������൱����ͣ��ע������ͼ�ο�ĺ���\n	getch();\n	return 0;\n}";
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		setfont(20, 0, "����");
		outtextxy(100, 0, "Hello World");
		{
			setfont(12, 0, "����");
			setcolor(0x808080);
			line(320, 0, 320, 480);
			setcolor(0xFFFFFF);
			outtextrect(320, 100, 320, 380, str);
			outtextrect(320, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������");
		}
		getch();
		return new SceneHelloWorld2(m_parent);
	}
private:
	SceneBase* m_parent;
};

class SceneForLoop9 : public SceneBase
{
public:
	SceneForLoop9(SceneBase* parent)
	{
		m_parent = parent;
		img = newimage();
	}
	~SceneForLoop9()
	{
		delimage(img);
	}
	void smain()
	{
		int y, x;
		setbkcolor(DARKGRAY);
		for(y = 0; y < 8; ++y)
		{
			for(x = 0; x < 8; ++x)
			{
				setfillcolor(((x ^ y) & 1) ? BLACK : WHITE);
				bar(x * 30, y * 30, (x + 1) * 30, (y + 1) * 30);
			}
		}
		info();
	}
	void info()
	{
		if(img->getwidth() <= 1)
		{
			char str[] = "#include \"graphics.h\"\nint main()\n{\
\n	initgraph(640, 480);\
\n	int y, x;\
\n	setbkcolor(DARKGRAY);\
\n	for(y = 0; y < 8; ++y)\
\n	{\
\n		for(x = 0; x < 8; ++x)\
\n		{\
\n			setfillcolor(((x^y)&1) ? BLACK : WHITE );\
\n			bar(x * 30, y * 30,\
\n			(x+1) * 30, (y+1) * 30));\
\n		}\
\n	}\
\n	getch();\n	return 0;\n}";
			img->createimage(320, 480);
			setfont(12, 0, "����", img);
			setbkmode(TRANSPARENT, img);
			setcolor(0x808080, img);
			line(0, 0, 0, 480, img);
			setcolor(0xFFFFFF, img);
			outtextrect(0, 50, 320, 480, str, img);
			outtextrect(0, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������", img);
		}
		putimage(320, 0, img);
	}
	SceneBase* Update()
	{
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		while(kbhit())
		{
			getch();
		}
		smain();
		getch();
		return m_parent;
	}
private:
	SceneBase* m_parent;
	IMAGE* img;
};

class SceneForLoop8 : public SceneBase
{
public:
	SceneForLoop8(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		int y, x;
		setbkcolor(DARKGRAY);
		for(y = 0; y < 8; ++y)
		{
			for(x = 0; x < 8; ++x)
			{
			//	setfillstyle(((x^y)&1) ? BLACK : WHITE );
				bar(x * 30, y * 30, (x + 1) * 30, (y + 1) * 30);
			}
		}
		info();
	}
	void info()
	{
		if(img.getwidth() <= 1)
		{
			char str[] = "#include \"graphics.h\"\nint main()\n{\
\n	initgraph(640, 480);\
\n	int y, x;\
\n	setbkcolor(DARKGRAY);\
\n	for(y = 0; y < 8; ++y)\
\n	{\
\n		for(x = 0; x < 8; ++x)\
\n		{\
\n			setfillstyle(((x^y)&1) ? BLACK : WHITE );\
\n			bar(x * 30, y * 30,\
\n				(x+1) * 30, (y+1) * 30));\
\n		}\
\n	}\
\n	getch();\n	return 0;\n}";
			img.createimage(320, 480);
			setfont(12, 0, "����", &img);
			setbkmode(TRANSPARENT, &img);
			setcolor(0x808080, &img);
			line(0, 0, 0, 480, &img);
			setcolor(0xFFFFFF, &img);
			outtextrect(0, 50, 320, 480, str, &img);
			outtextrect(0, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������", &img);
		}
		putimage(320, 0, &img);
	}
	SceneBase* Update()
	{
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		while(kbhit())
			getch();
		smain();
		getch();
		return m_parent;
	}
private:
	SceneBase* m_parent;
	IMAGE img;
};

class SceneForLoop7 : public SceneBase
{
public:
	SceneForLoop7(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		int y;
		for(y = 0; y < 360; ++y)
		{
			setcolor(HSVtoRGB((float)y, 1.0f, 1.0f));
			line(0, y, 200, y);
		}
		info();
	}
	void info()
	{
		if(img.getwidth() <= 1)
		{
			char str[] = "#include \"graphics.h\"\nint main()\n{\
\n	initgraph(640, 480);\
\n	{\
\n		int y;\
\n		for(y = 0; y < 360; ++y)\
\n		{\
\n			setcolor(HSVtoRGB((float)y, 1.0f, 1.0f));\
\n			line(0, y, 200, y);\
\n		}\
\n	}\n	getch();\n	return 0;\n}";
			img.createimage(320, 480);
			setfont(12, 0, "����", &img);
			setbkmode(TRANSPARENT, &img);
			setcolor(0x808080, &img);
			line(0, 0, 0, 480, &img);
			setcolor(0xFFFFFF, &img);
			outtextrect(0, 50, 320, 480, str, &img);
			outtextrect(0, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������", &img);
		}
		putimage(320, 0, &img);
	}
	SceneBase* Update()
	{
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		while(kbhit())
			getch();
		smain();
		getch();
		return new SceneForLoop8(m_parent);
	}
private:
	SceneBase* m_parent;
	IMAGE img;
};

class SceneForLoop6 : public SceneBase
{
public:
	SceneForLoop6(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		int x = 0, dx = 1, color = 0;
		for(; kbhit() == 0; delay_fps(60))
		{
			cleardevice();
			if(x >= 320)
				dx = -1;
			else if(x <= 0)
				dx = 1;
			x += dx;
			color += 1;
			if(color >= 360)
				color = 0;
			setcolor(HSVtoRGB((float)color, 1.0f, 1.0f));
			circle(x, 100, 100);
			info();
		}
	}
	void info()
	{
		if(img.getwidth() <= 1)
		{
			char str[] = "#include \"graphics.h\"\nint main()\n{\
\n	initgraph(640, 480);\
\n	int x = 0, dx = 1, color = 0; //x��ʾԲ�ĺ����꣬dx��ʾ�ٶȷ���\
\n	//������ѭ����kbhit()��⵱ǰ��û�а������о��˳�\
\n	//delay_fps(60)�������ѭ��ÿ��ѭ��60��\
\n	for(; kbhit() == 0; delay_fps(60))\
\n	{\
\n		cleardevice();\
\n		//����x������dx�ķ���\
\n		if(x >= 320)\
\n			dx = -1;\
\n		else if(x <= 0)\
\n			dx = 1;\
\n		//ͨ����dx�Ŀ��ƣ���ӿ���x����������\
\n		x += dx;\
\n		color += 1;\
\n		if(color >= 360)\
\n			color = 0;\
\n		//ʹ��HSV��ʽָ����ɫ\
\n		//����HSV�Ľ��ܼ�ͼ�ο��ĵ�����Google\
\n		setcolor(HSVtoRGB((float)color, 1.0f, 1.0f));\
\n		circle(x, 100, 100);\n	}\n	getch();\n	return 0;\n}";
			img.createimage(320, 480);
			setfont(12, 0, "����", &img);
			setbkmode(TRANSPARENT, &img);
			setcolor(0x808080, &img);
			line(0, 0, 0, 480, &img);
			setcolor(0xFFFFFF, &img);
			outtextrect(0, 50, 320, 480, str, &img);
			outtextrect(0, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������", &img);
		}
		putimage(320, 0, &img);
	}
	SceneBase* Update()
	{
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		while(kbhit())
			getch();
		smain();
		getch();
		return new SceneForLoop7(m_parent);
	}
private:
	SceneBase* m_parent;
	IMAGE img;
};

class SceneForLoop5 : public SceneBase
{
public:
	SceneForLoop5(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		int x = 0, dx = 1;
		for(; kbhit() == 0; delay_fps(60))
		{
			cleardevice();
			if(x >= 320)
				dx = -1;
			else if(x <= 0)
				dx = 1;
			x += dx;
			setcolor(0xFF0080);
			circle(x, 100, 100);
			info();
		}
	}
	void info()
	{
		if(img.getwidth() <= 1)
		{
			char str[] = "#include \"graphics.h\"\n\nint main()\n{\
\n	initgraph(640, 480);\
\n	int x = 0, dx = 1; //x��ʾԲ�ĺ����꣬dx��ʾ�ٶȷ���\
\n	//������ѭ����kbhit()��⵱ǰ��û�а������о��˳�\
\n	//delay_fps(60)�������ѭ��ÿ��ѭ��60��\
\n	for(; kbhit() == 0; delay_fps(60))\
\n	{\
\n		//����\
\n		cleardevice();\
\n		//����x������dx�ķ���\
\n		if(x >= 320)\
\n			dx = -1;\
\n		else if(x <= 0)\
\n			dx = 1;\
\n		//ͨ����dx�Ŀ��ƣ���ӿ���x����������\
\n		x += dx;\
\n		//ʹ��RGB������ʽָ����ɫ\
\n		//��ɫΪ80����Ϊ0����ΪFF\
\n		setcolor(0xFF0080);\
\n		circle(x, 100, 100);\n	}\n	getch();\n	return 0;\n}";
			img.createimage(320, 480);
			setfont(12, 0, "����", &img);
			setbkmode(TRANSPARENT, &img);
			setcolor(0x808080, &img);
			line(0, 0, 0, 480, &img);
			setcolor(0xFFFFFF, &img);
			outtextrect(0, 50, 320, 480, str, &img);
			outtextrect(0, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������", &img);
		}
		putimage(320, 0, &img);
	}
	SceneBase* Update()
	{
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		while(kbhit())
			getch();
		smain();
		getch();
		return new SceneForLoop6(m_parent);
	}
private:
	SceneBase* m_parent;
	IMAGE img;
};

class SceneForLoop4 : public SceneBase
{
public:
	SceneForLoop4(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		for(int n = 0; n < 320; n++)
		{
			double x = ((double)n - 160) / 20;
			double y = sin(x);
			y = -y * 80 + 240;
			putpixel(n, (int)y, WHITE);
		}
		line(0, 240, 320, 240);
		line(160, 0, 160, 480);
	}
	SceneBase* Update()
	{
		char str[] = "#include \"graphics.h\"\n\nint main()\n{\n	initgraph(640, 480);\n	int n; //��������x\n	//����x��0��320��ȡ��ÿ��������\
\n	for(int n = 0; n < 320; n++)\
\n	{\
\n		//ӳ�䵽-8��8�ĸ�������Χ\
\n		double x = ((double)n - 160) / 20;\
\n		//�����Ӧ��y\
\n		double y = sin(x);\
\n		//��yӳ�����Ļ����\
\n		y = -y * 80 + 240;\
\n		//���������\
\n		putpixel(n, (int)y, WHITE);\
\n	}\
\n	//��������\
\n	line(0, 240, 320, 240);\
\n	line(160, 0, 160, 480);\
\n	getch();\n	return 0;\n}";
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		smain();
		{
			setfont(12, 0, "����");
			setcolor(0x808080);
			line(320, 0, 320, 480);
			setcolor(0xFFFFFF);
			outtextrect(320, 100, 320, 380, str);
			outtextrect(320, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������");
		}
		getch();
		return new SceneForLoop5(m_parent);
	}
private:
	SceneBase* m_parent;
};

class SceneForLoop3 : public SceneBase
{
public:
	SceneForLoop3(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		for(int n = 0; n < 320; n++)
		{
			double x = ((double)n - 160) / 80;
			double y = x * x;
			y = -y * 80 + 240;
			putpixel(n, (int)y, WHITE);
		}
		line(0, 240, 320, 240);
		line(160, 0, 160, 480);
	}
	SceneBase* Update()
	{
		char str[] = "#include \"graphics.h\"\n\nint main()\n{\n	initgraph(640, 480);\n	int n; //��������x\n	//����x��0��320��ȡ��ÿ��������\
\n	for(int n = 0; n < 320; n++)\
\n	{\
\n		//ӳ�䵽-2��2�ĸ�������Χ\
\n		double x = ((double)n - 160) / 80;\
\n		//�����Ӧ��y\
\n		double y = x * x;\
\n		//��yӳ�����Ļ����\
\n		y = -y * 80 + 240;\
\n		//���������\
\n		putpixel(n, (int)y, WHITE);\
\n	}\
\n	//��������\
\n	line(0, 240, 320, 240);\
\n	line(160, 0, 160, 480);\
\n	getch();\n	return 0;\n}";
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		smain();
		{
			setfont(12, 0, "����");
			setcolor(0x808080);
			line(320, 0, 320, 480);
			setcolor(0xFFFFFF);
			outtextrect(320, 100, 320, 380, str);
			outtextrect(320, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������");
		}
		getch();
		return new SceneForLoop4(m_parent);
	}
private:
	SceneBase* m_parent;
};

class SceneForLoop2 : public SceneBase
{
public:
	SceneForLoop2(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		for(int x = 100; x < 300; x += 3)
			putpixel(x, 100, GREEN);
	}
	SceneBase* Update()
	{
		char str[] = "#include \"graphics.h\"\n\nint main()\n{"
		"\n	initgraph(640, 480);\n	int x; //��������x\n	//����x��100��300������Ϊ3��������������\n	for(x = 100; x < 300; x += 3)\n	{\n		//��y=100�ĵط����̵㣬��������㹹����\n		putpixel(x, 100, GREEN);\n	}\n	getch();\n	return 0;\n}";
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		smain();
		setfont(12, 0, "����");
		setcolor(0x808080);
		line(320, 0, 320, 480);
		setcolor(0xFFFFFF);
		outtextrect(320, 100, 320, 380, str);
		outtextrect(320, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������");
		getch();
		return new SceneForLoop3(m_parent);
	}
private:
	SceneBase* m_parent;
};

class SceneForLoop : public SceneBase
{
public:
	SceneForLoop(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		for(int x = 100; x < 300; x++)
			putpixel(x, 100, RED);
	}
	SceneBase* Update()
	{
		char str[] = "#include \"graphics.h\"\n\nint main()\n{\n	initgraph(640, 480);\n	int x; //��������x\n	//����x��100��300\n	for(x = 100; x < 300; x++)\n	{\n		//��y=100�ĵط�����㣬��������㹹����\n		putpixel(x, 100, RED);\n	}\n	getch();\n	return 0;\n}";
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		smain();
		setfont(12, 0, "����");
		setcolor(0x808080);
		line(320, 0, 320, 480);
		setcolor(0xFFFFFF);
		outtextrect(320, 100, 320, 380, str);
		outtextrect(320, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������");
		getch();
		return new SceneForLoop2(m_parent);
	}
private:
	SceneBase* m_parent;
};

class SceneArray2 : public SceneBase
{
public:
	SceneArray2(SceneBase* parent)
	{
		m_parent = parent;
		m_dline = 0;
		m_resettext = 1;
	}
	void mydelay(int ms)
	{
		int nms = 0;
		for(; nms < ms; delay_ms(50), nms += 50)
		{
			while(kbhit())
			{
				int key = getch();
				if((key & KEYMSG_DOWN) == 0) continue;
				key &= 0xFFFF;
				if(key == 'W' || key == VK_UP)
				{
					m_dline += 1;
					m_resettext = 1;
				}
				else if((key == 'S' || key == VK_DOWN) && m_dline > 0)
				{
					m_dline -= 1;
					m_resettext = 1;
				}
			}
			info();
		}
	}
	void display(int[], int n, int i)
	{
		int a;
		cleardevice();
		for(a = 0; a < n; ++a)
		{
			setcolor(WHITE);
		//	setfillstyle(HSLtoRGB(120.0f, 1.0f, (float)(arr[a] / 32.0)));
			fillellipse(100, 20 * a + 30, 9, 9);
		}
		if(i >= 0)
		{
		//	setfillstyle(HSLtoRGB(120.0f, 1.0f, 1.0f));
			fillellipse(80, 20 * i + 30, 9, 9);
			fillellipse(80, 20 * (i + 1) + 30, 9, 9);
		}
		mydelay(500);
	}
	void smain()
	{
		int arr[20];
		int a, b;
		randomize();
		for(a = 0; a < 20; ++a)
		{
			arr[a] = random(32);
		}
		display(arr, 20, -1);
		setfont(12, 0, "����");
		outtextxy(0, 0, "�밴�������ʼ��ʾð������");
		info();
		getch();
		cleardevice();
		for(b = 20; b > 0; --b)
		{
			for(a = 1; a < b; ++a)
			{
				if(arr[a] < arr[a - 1])
				{
					int t = arr[a];
					arr[a] = arr[a - 1];
					arr[a - 1] = t;
				}
				display(arr, 20, a - 1);
			}
		}
		outtextxy(0, 0, "�������");
	}
	void info()
	{
		if(m_resettext)
		{
			char str[] = "#include \"graphics.h\"\n#include <stdio.h>\n#include <time.h>\n#include <string.h>\
\nvoid display(int arr[], int n)\
\n{\
\n	int a;\
\n	cleardevice();\
\n	for(a = 0; a < n; ++a)\
\n	{\
\n		setcolor(WHITE);\
\n		setfillstyle(HSLtoRGB(120.0f, 1.0f, (float)(arr[a] / 32.0)));\
\n		fillellipse(100, 20 * a, 9, 9);\
\n	}\
\n	if(i >= 0)\
\n	{\
\n		setfillstyle(HSLtoRGB(120.0f, 1.0f, 1.0f)));\
\n		fillellipse(80, 20 * i + 30, 9, 9);\
\n		fillellipse(80, 20 * (i + 1) + 30, 9, 9);\
\n	}\
\n	delay(500);\
\n}\
\nint main()\
\n{\
\n	int arr[20];\
\n	int a, b;\
\n	initgraph(640, 480);\
\n	randomize();\
\n	for(a = 0; a < 20; ++a)\
\n	{\
\n		arr[a] = random(32);\
\n	}\
\n	display(arr, 20);\
\n	setfont(12, 0, \"����\");\
\n	outtextxy(0, 0, \"�밴�������ʼ��ʾ\");\
\n	getch();\
\n	cleardevice();\
\n	for(b = 20; b > 0; --b)\
\n	{\
\n		for(a = 1; a < b; ++a)\
\n		{\
\n			if( arr[a] < arr[a-1])\
\n			{\
\n				int t = arr[a];\
\n				arr[a] = arr[b];\
\n				arr[b] = t;\
\n			}\
\n			display(arr, 20, a-1);\
\n		}\
\n	}\
\n	outtextxy(0, 0, \"�������\");\
\n	return 0;\
\n}\
";
			m_resettext = 0;
			img.createimage(320, 480);
			setfont(12, 0, "����", &img);
			setbkmode(TRANSPARENT, &img);
			setcolor(0x808080, &img);
			line(0, 0, 0, 480, &img);
			setcolor(0xFFFFFF, &img);
			outtextrect(0, 50 - m_dline * 12, 320, 2048, str, &img);
			outtextrect(0, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������", &img);
		}
		putimage(320, 0, &img);
	}
	SceneBase* Update()
	{
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		while(kbhit())
			getch();
		smain();
		getch();
		return m_parent;
	}
private:
	SceneBase* m_parent;
	IMAGE img;
	int m_dline;
	int m_resettext;
};

class SceneArray : public SceneBase
{
public:
	SceneArray(SceneBase* parent)
	{
		m_parent = parent;
	}
	void smain()
	{
		int t = clock();
		char str[100];
		for(; kbhit() == 0; delay_fps(60))
		{
			cleardevice();
			sprintf(str, "����ʱ��%d", int(clock() - t));
			setfont(36, 0, "��Բ");
			outtextxy(0, 0, str);
			info();
		}
	}
	void info()
	{
		if(img.getwidth() <= 1)
		{
			char str[] = "#include \"graphics.h\"\n#include <stdio.h>\n#include <time.h>\n#include <string.h>\nint main()\n{\
\n	initgraph(640, 480);\
\n	{\
\n		int t = clock(); //��¼��ʼʱ��\
\n		char str[100];\
\n		for(; kbhit() == 0; delay_fps(60))\
\n		{\
\n			cleardevice();\
\n			//��clock()-t�Ľ��������ַ���str\
\n			//ʵ�ּ򵥵ļ�ʱ������չ�����\
\n			sprintf(str, \"����ʱ��%d\", clock() - t;\
\n			setfont(36, 0, \"��Բ\");\
\n			outtextxy(0, 0, str);\
\n		}\
\n	}\
\n	getch();\n	return 0;\n}";
			img.createimage(320, 480);
			setfont(12, 0, "����", &img);
			setbkmode(TRANSPARENT, &img);
			setcolor(0x808080, &img);
			line(0, 0, 0, 480, &img);
			setcolor(0xFFFFFF, &img);
			outtextrect(0, 50, 320, 480, str, &img);
			outtextrect(0, 0, 320, 400, "�����ǳ������н������������Ӧ��Դ����\n��������鿴��һ������", &img);
		}
		putimage(320, 0, &img);
	}
	SceneBase* Update()
	{
		setbkcolor_f(BLACK);
		cleardevice();
		setcolor(LIGHTGRAY);
		while(kbhit())
			getch();
		smain();
		getch();
		return new SceneArray2(m_parent);
	}
private:
	SceneBase* m_parent;
	IMAGE img;
};

class SceneMenu : public SceneBase
{
public:
	SceneMenu()
	{
		memset(m_strlist, 0, sizeof(m_strlist));
		strcpy(m_strlist[0], "1.����Ҹ�ѧ��Hello World");
	}
	SceneBase* Update()
	{
		setbkcolor_f(0x808080);
		cleardevice();
		setcolor(0xFFFFFF);
		setfont(24, 0, "����");
		outtextrect(100, 200, 500, 500, "1.����Ҹո�ѧ��Hello World\n2.����Ҹո�ѧ��ѭ���ͷ�֧\n3.����Ҹո�ѧ��������ַ���\n�����������д���ӣ�\n");
		outtextxy(100, 100, "�밴���ּ�ѡ��Ҫ��������");
		int k;
		while(1)
		{
			k = getch();
			if(k == '1')
				return new SceneHelloWorld(new SceneMenu);
			else if(k == '2')
				return new SceneForLoop(new SceneMenu);
			else if(k == '3')
				return new SceneArray(new SceneMenu);
		}
	}
private:
	int m_x, m_y;
	int m_highlight;
	char m_strlist[100][32];
};

class SceneIntroduce : public SceneBase
{
public:
	SceneIntroduce()
	{
		memset(m_str, 0, sizeof(m_str));
		strcpy(m_str, "���Ǹո�ѧϰ�����Ե����������ǲ��Ǿ��õ������ַ�����е����ģ�������ֻ������Щ���ܲ���������Ȥ�ģ�����д��Ϸ��\r\n����ʾ�������Ϊ�˸���⿪����ɻ󣬱����򽫴�����뾫�ʵģ�����ͼ�����磡���������ڵ�C�Ǹոտ�ʼѧ������ѧ��һ��ʱ�䣬ֻҪ����VC����C-Free���������������ͼ�ε�ʢ�硣����\r\n����ʽ��ʼǰ������ٶȡ�PowerEasyX�������ز��������˵���ĵ���װ�á������װʱ����ʲô���ѣ����Լ�QQȺ46612969˵����������������Э�������ġ�\r\n���밴�����������\r\n");
	}
	SceneBase* Update()
	{
		char str[1024]{0};
		int len = 0;
		setfont(20, 0, "����");
		for(len = 0 ; len <= 0x80; delay_fps(60))
		{
			setbkcolor_f(RGB(len, len, len));
			cleardevice();
			++len;
		}

		for(len = 0 ; m_str[len]; delay_fps(30))
		{
			strncpy(str, m_str, len);
			len += 2;
			cleardevice();
			setcolor(0x0);
			outtextrect(102, 101, 440, 480, str);
			setcolor(0xFFFFFF);
			outtextrect(100, 100, 440, 480, str);
		}
		getch();
		IMAGE img;
		getimage(&img, 0, 0, 640, 480);
		for(len = 255 ; len >= 0; delay_fps(60))
		{
			cleardevice();
			putimage_alphablend(nullptr, &img, 0, 0, len);
			len -= 3;
		}
		return new SceneMenu;
	}
private:
	char m_str[1024];
};

int main()
{
	initgraph(640, 480);
	SceneBase* scene = new SceneIntroduce; //SceneIntroduce; SceneMenu
	setbkmode(TRANSPARENT);
//    BeginBatchDraw();
	for(SceneBase* newscene = scene; newscene; delay_fps(60))
	{
		newscene = scene->Update();
		if(newscene != scene)
		{
			delete scene;
			scene = newscene;
		}
	}
//    EndBatchDraw();
	closegraph();
	return 0;
}

