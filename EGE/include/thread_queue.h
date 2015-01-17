#ifndef INC_thread_queue_h_
#define INC_thread_queue_h_

#include <windows.h>
#include <mutex>

#define QUEUE_LEN 1024

namespace ege
{

template<typename T>
class thread_queue
{
private:
	mutable std::mutex mtx;
	T _queue[QUEUE_LEN];
	int _r = 0, _w = 0;

public:
	void
	clear()
	{
		std::lock_guard<std::mutex> lck(mtx);

		_r = _w = 0;
	}

	bool
	empty() const
	{
		std::lock_guard<std::mutex> lck(mtx);

		return _r == _w;
	}

	void
	pop()
	{
		std::lock_guard<std::mutex> lck(mtx);

		assert(_w != _r);
		_r = (_r + 1) % QUEUE_LEN;
	}

	void
	push(const T& d_)
	{
		std::lock_guard<std::mutex> lck(mtx);

		int w = (_w + 1) % QUEUE_LEN;
		_queue[w] = d_;
		if(w == _r)
			_r = (_r + 1) % QUEUE_LEN;
		_w = w;
	}

	T&
	top()
	{
		std::lock_guard<std::mutex> lck(mtx);

		assert(_w != _r);
		return _queue[_r];
	}
	T&
	top() const
	{
		std::lock_guard<std::mutex> lck(mtx);

		assert(_w != _r);
		return _queue[_r];
	}

	int
	unpop()
	{
		std::lock_guard<std::mutex> lck(mtx);

		if(_r == (_w + 1) % QUEUE_LEN)
			return 0;
		_r = (_r + QUEUE_LEN - 1) % QUEUE_LEN;
		return 1;
	}

	template<typename F>
	void
	process(F&& process_func)
	{
		std::lock_guard<std::mutex> lck(mtx);
		int r = _r;
		int w = _w;

		if(r != w)
		{
			if(w < r) w += QUEUE_LEN;

			for(; r <= w; r++)
			{
				int pos = r % QUEUE_LEN;
				process_func(_queue[pos]);
			}
		}
	}
};

}

#endif

