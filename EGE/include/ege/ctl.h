﻿#ifndef Inc_ege_ctl_h_
#define Inc_ege_ctl_h_

#include "ege/base.h"
#include "ege/mac.h"
#include "ege/pushtarget.h"
#include "ege/windows.h"
#include "ege/img.h"

namespace ege
{

class EGEAPI egeControlBase
{
public:
	enum ROP
	{
		COPY    = SRCCOPY,
		XOR     = SRCINVERT,
		AND     = SRCAND,
		OR      = SRCPAINT,
	};
	enum blendmode_e
	{
		SOLIDCOPY = 0,
		ALPHABLEND = 1,
	};
	enum inherit_e
	{
		inherit_level_e = 0,
	};
	// 构造函数可以自定义，但要按需要选择使不使用宏，详见前面代码或者文档示例代码
	egeControlBase();
	egeControlBase(int inherit, egeControlBase* pParent);
	~egeControlBase();

	// 以下虚函数都不要直接相互调用
	virtual LRESULT onMessage(::UINT, ::WPARAM, ::LPARAM)
	{
		return 0;
	}
	// 以下函数如果返回非0则不向子控件传递键盘鼠标消息
	virtual int onMouse(int, int, int)
	{
		return 0;
	}
	virtual int onKeyDown(int, int)
	{
		return 0;
	}
	virtual int onKeyUp(int, int)
	{
		return 0;
	}
	virtual int onKeyChar(int, int)
	{
		return 0;
	}
	// 屏幕更新后会被调用，用于更新逻辑
	virtual int onUpdate()
	{
		return 0;
	}
	// 以下GetFocus在要获得焦点时调用，返回值一般返回0表示获取键盘输入焦点，返回非0放弃获得输入焦点
	virtual int onGetFocus()
	{
		return 0;
	}
	// 失去输入焦点时调用
	virtual void onLostFocus(){}
	// 设置尺寸前调用，自定义修正函数
	virtual void onSizing(int*, int*){}
	// 响应尺寸变化函数
	virtual void onSize(int, int){}
	// 重绘函数，尽量请画到pimg上，以便能控制绘画目标
	virtual void onDraw(IMAGE*) const{}
	// 尺寸变化时调用，用于重画过滤缓冲区内容
	virtual void onResetFilter()
	{
		setbkcolor(BLACK, m_mainFilter);
		cleardevice(m_mainFilter);
	}
	virtual void onAddChild(egeControlBase*){}
	virtual void onDelChild(egeControlBase*){}
	virtual void onIdle(){} // 保留接口，未用
	// 这里以上的函数可以自行定义（注意声明要一致，不要漏掉OnDraw里的const）
	// 这里以下的public函数可以调用，不可自定义，任何预定义变量都不要直接访问，请使用预定义函数来控制
public:
	IMAGE* buf()
	{
		return m_mainbuf;
	}
	IMAGE* filter()
	{
		return m_mainFilter;
	}
	egeControlBase* parent()
	{
		return m_parent;
	}
	IMAGE* buf() const
	{
		return m_mainbuf;
	}
	IMAGE* filter() const
	{
		return m_mainFilter;
	}
	const egeControlBase* parent() const
	{
		return m_parent;
	}

	void blendmode(int mode)
	{
		m_AlphablendMode = mode;
	}
	void setrop(int rop)
	{
		m_rop = rop;    // 请用枚举类型ROP里所定义的
	}

	void directdraw(bool bdraw)
	{
		m_bDirectDraw = (bdraw ? 1 : 0);
	}
	bool isdirectdraw() const
	{
		return (m_bDirectDraw != 0);
	}
	void autoredraw(bool)
	{
		m_bAutoDraw = (m_bAutoDraw ? 1 : 0);
	}
	bool isautoredraw() const
	{
		return (m_bAutoDraw != 0);
	}
	void visable(bool bvisable)
	{
		m_bVisable = (bvisable ? 1 : 0);
	}
	bool isvisable() const
	{
		return (m_bVisable != 0);
	}
	void enable(bool benable)
	{
		m_bEnable = (benable ? 1 : 0);
	}
	bool isenable() const
	{
		return (m_bEnable != 0);
	}
	void capture(bool bcapture)
	{
		m_bCapture = (bcapture ? 1 : 0);
	}
	bool iscapture() const
	{
		return (m_bCapture != 0);
	}
	void capmouse(bool bcapmouse)
	{
		m_bCapMouse = (bcapmouse ? 1 : 0);
	}
	bool iscapmouse() const
	{
		return (m_bCapMouse != 0);
	}
	bool isfocus() const
	{
		return (m_bInputFocus != 0);
	}
	void move(int x, int y)
	{
		m_x = x;
		m_y = y;
	}
	void size(int w, int h)
	{
		onSizing(&w, &h);
		m_w = w;
		m_h = h;
		resize(m_mainbuf, w, h);
		resize(m_mainFilter, w, h);
		onSize(w, h);
		onResetFilter();
	}
	void zorderup();
	void zorderdown();
	void zorderset(int z);

	int getx() const
	{
		return m_x;
	}
	int gety() const
	{
		return m_y;
	}
	int getw() const
	{
		return m_w;
	}
	int geth() const
	{
		return m_h;
	}
	int width() const
	{
		return m_w;
	}
	int height() const
	{
		return m_h;
	}

	int addchild(egeControlBase* pChild);
	int delchild(egeControlBase* pChild);
	void draw(IMAGE* pimg);
	void update();
	void mouse(int x, int y, int flag);
	void keymsgdown(unsigned key, int flag);
	void keymsgup(unsigned key, int flag);
	void keymsgchar(unsigned key, int flag);
	bool operator<(const egeControlBase& pbase) const
	{
		if(m_zOrderLayer != pbase.m_zOrderLayer)
			return m_zOrderLayer < pbase.m_zOrderLayer;
		if(m_zOrder == pbase.m_zOrder)
			return this < &pbase;
		else
			return m_zOrder < pbase.m_zOrder;
	}
protected:
	int allocId();
	int allocZorder();
	class EGEAPI InitObject
	{
	public:
		InitObject(egeControlBase* pThis, int inherit_level);
		~InitObject();
	private:
		egeControlBase* m_this;
		int m_inherit_level;
	};
	void(*m_preinit_func)(egeControlBase*);
private:
	void init(egeControlBase* parent);
	void fixzorder();
	void sortzorder();
#if _MSC_VER <= 1200
public:
#endif
	void initok();
private:
	IMAGE*   m_mainbuf;      // 主缓冲
	IMAGE*   m_mainFilter;   // 过滤器

private:
	int m_bVisable;     // 是否可见
	int m_bEnable;      // 是否可获得输入（键盘和鼠标）
	int m_bAutoDraw;    // 是否自动绘画到窗口上
	int m_bCapture;     // 是否可获得键盘输入焦点
	int m_bInputFocus;  // 是否已经获得输入焦点
	int m_bCapMouse;    // 是否捕捉鼠标（即使不在所在区域内）
	int m_zOrderLayer;  // Z次序层（值较大者在前，值较小者会被其它控件遮挡）
	int m_zOrder;       // Z次序（值较大者在前，值较小者会被其它控件遮挡）
	int m_allocId;      // 分配id
	int m_allocZorder;  // 分配Z次序

	egeControlBase* m_parent;
	static int s_maxchildid;   // 下一次子控件分配ID值

private:
	void* m_childmap;       // 子控件
	void* m_childzorder;    // 子控件排序

protected:
	int m_x, m_y;       // 左上角坐标
	int m_w, m_h;       // 宽高

protected:
	::DWORD   m_rop;              // 混合方式
	int     m_AlphablendMode;   // 绘画混合过滤方式
	int     m_bDirectDraw;      // 启用直接绘画
#if _MSC_VER <= 1200
public:
#endif
	int     m_inheritlevel;     // 继承层次
};

}

#endif

