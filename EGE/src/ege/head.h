﻿#ifndef Inc_ege_head_h_
#define Inc_ege_head_h_

#include "ege.h"
#include "thread_queue.h"

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS 0
#endif

#ifndef ASSERT_TRUE
#ifdef _DEBUG
#include <cassert>
#define ASSERT_TRUE(e) assert((e) != MUSIC_ERROR)
#else
#define ASSERT_TRUE(e) (void(0))
#endif
#endif

#include <wtypes.h> // for ::PROPID required by <gdiplus.h>;
#include <gdiplus.h>

#define QUEUE_LEN           1024
#define UPDATE_MAX_CALL     0xFF
#define RENDER_TIMER_ID     916
#define IMAGE_INIT_FLAG     0x20100916
#define FLOAT_EPS           1e-3f

#define IFATODOB(A, B)  ( (A) && (B, 0) )
#define IFNATODOB(A, B) ( (A) || (B, 0) )

#ifndef DEFAULT_CHARSET
#define DEFAULT_CHARSET ANSI_CHARSET
#endif

#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif
#endif

#ifndef __int3264
#if defined(_WIN64)
typedef __int64 ::LONG_PTR, *PLONG_PTR;
typedef unsigned __int64 ::ULONG_PTR, *PULONG_PTR;

#define __int3264   __int64

#else
typedef _W64 long LONG_PTR, *PLONG_PTR;
typedef _W64 unsigned long ULONG_PTR, *PULONG_PTR;

#define __int3264   __int32

#endif
#endif

typedef ::ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef unsigned int uint32;
typedef intptr_t POINTER_SIZE;

#ifndef _MSC_VER
#define GRADIENT_FILL_RECT_H 0x00000000
#define GRADIENT_FILL_RECT_V 0x00000001
#define GRADIENT_FILL_TRIANGLE 0x00000002
#define GRADIENT_FILL_OP_FLAG 0x000000ff

extern "C"
{
	WINGDIAPI WINAPI BOOL
	GradientFill(::HDC hdc, PTRIVERTEX pVertex, ::ULONG nVertex, PVOID pMesh,
		::ULONG nMesh, ::ULONG ulMode);
}
#endif

namespace ege
{

enum dealmessage_update
{
	NORMAL_UPDATE = false,
	FORCE_UPDATE = true,
};


template<typename T>
struct count_ptr
{
	explicit count_ptr(T* p)
	{
		//m_mutex = new Mutex;
		m_cnt = new long(1);
		m_p = p;
	}
	~count_ptr()
	{
		//m_mutex->Lock();
		--*m_cnt;
		if(*m_cnt == 0)
		{
			delete m_p;
			m_p = static_cast<T*>(0);
			delete m_cnt;
			m_cnt = static_cast<long*>(0);
		}
		//Mutex* mutex = m_mutex;
		//m_mutex = static_cast<Mutex*> ( 0 );
		//mutex->UnLock();
	}
	count_ptr(const count_ptr<T>& count_ptr_)
	{
		//m_mutex = count_ptr_.m_mutex;
		//m_mutex->Lock();
		m_p = count_ptr_.m_p;
		m_cnt = count_ptr_.m_cnt;
		++*m_cnt;
		//m_mutex->UnLock();
	}
	count_ptr<T>& operator= (const count_ptr<T>& count_ptr_)
	{
		//m_mutex->Lock();
		--*m_cnt;
		if(*m_cnt == 0)
		{
			delete m_p;
			m_p = static_cast<T*>(0);
			delete m_cnt;
			m_cnt = static_cast<long*>(0);
		}
		//Mutex* mutex = m_mutex;
		//m_mutex = count_ptr_.m_mutex;
		//mutex->UnLock();

		//m_mutex->Lock();
		m_p = count_ptr_.m_p;
		m_cnt = static_cast<long*>(count_ptr_.m_cnt);
		++*m_cnt;
		//m_mutex->UnLock();
		return *this;
	}
	operator T* () const
	{
		return m_p;
	}
	T& operator * () const
	{
		return *m_p;
	}
	T* operator -> () const
	{
		return m_p;
	}
private:
	T* m_p;
	long* m_cnt;
	//Mutex* m_mutex;
};

} // namespace ege

#endif /*Inc_ege_head_h_*/
