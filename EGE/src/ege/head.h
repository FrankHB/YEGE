#ifndef Inc_ege_head_h_
#define Inc_ege_head_h_

#include "ege.h"
#include "thread_queue.h"
#include <cassert>

#ifndef DEFAULT_CHARSET
#define DEFAULT_CHARSET ANSI_CHARSET
#endif

namespace ege
{

template<typename T>
struct count_ptr
{
	explicit count_ptr(T* p)
	{
		m_cnt = new long(1);
		m_p = p;
	}
	~count_ptr()
	{
		--*m_cnt;
		if(*m_cnt == 0)
		{
			delete m_p;
			m_p = static_cast<T*>(0);
			delete m_cnt;
			m_cnt = static_cast<long*>(0);
		}
	}
	count_ptr(const count_ptr<T>& count_ptr_)
	{
		m_p = count_ptr_.m_p;
		m_cnt = count_ptr_.m_cnt;
		++*m_cnt;
	}
	count_ptr<T>& operator= (const count_ptr<T>& count_ptr_)
	{
		--*m_cnt;
		if(*m_cnt == 0)
		{
			delete m_p;
			m_p = static_cast<T*>(0);
			delete m_cnt;
			m_cnt = static_cast<long*>(0);
		}
		m_p = count_ptr_.m_p;
		m_cnt = static_cast<long*>(count_ptr_.m_cnt);
		++*m_cnt;
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
};

} // namespace ege

#endif

