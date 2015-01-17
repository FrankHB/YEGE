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
	std::mutex mtx;
	T _queue[QUEUE_LEN];
	T _last;
	int _r = 0, _w = 0;

public:
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

	int
	pop(T& d_)
	{
		std::lock_guard<std::mutex> lck(mtx);

		if(_w == _r)
			return 0;
		d_ = _queue[_r];
		_last = d_;
		_r = (_r + 1) % QUEUE_LEN;
		return 1;
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

	T
	last()
	{
		return _last;
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

	bool
	empty()
	{
		std::lock_guard<std::mutex> lck(mtx);

		return _r == _w;
	}
};

}

#endif

