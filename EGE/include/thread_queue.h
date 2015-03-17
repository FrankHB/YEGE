#ifndef INC_thread_queue_h_
#define INC_thread_queue_h_

#include <mutex>
#include <deque>

#define QUEUE_LEN 1024

namespace ege
{

template<typename T>
class thread_queue
{
private:
	mutable std::mutex mtx;
	std::deque<T> _queue;

public:
	void
	clear()
	{
		std::lock_guard<std::mutex> lck(mtx);

		_queue.clear();
	}

	bool
	empty() const
	{
		std::lock_guard<std::mutex> lck(mtx);

		return _queue.empty();
	}

	void
	pop()
	{
		std::lock_guard<std::mutex> lck(mtx);

		assert(!_queue.empty());
		_queue.pop_front();
	}

	void
	push(const T& d_)
	{
		std::lock_guard<std::mutex> lck(mtx);

		_queue.push_back(d_);
		if(_queue.size() > QUEUE_LEN)
			_queue.pop_front();
	}

	T&
	top()
	{
		std::lock_guard<std::mutex> lck(mtx);

		assert(!_queue.empty());
		return _queue.front();
	}
	T&
	top() const
	{
		std::lock_guard<std::mutex> lck(mtx);

		assert(!_queue.empty());
		return _queue.front();
	}

	template<typename F>
	void
	process(F f)
	{
		std::lock_guard<std::mutex> lck(mtx);

		for(auto& e : _queue)
			f(e);
	}
};

}

#endif

