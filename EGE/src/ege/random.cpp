#include "ege/random.h"
#include <random>
#include <memory>

namespace ege
{

namespace
{

std::random_device rd;

std::mt19937&
get_mt()
{
	static std::mt19937 mt(rd());

	return mt;
}

} // unnamed namespace;

void
randomize()
{
	get_mt() = std::mt19937(rd());
}

unsigned
random(unsigned n)
{
	std::uniform_int_distribution<unsigned> d(0, n);

	return d(get_mt());
}

double
randomf()
{
	std::uniform_real_distribution<float> d;

	return d(get_mt());
}

} // namespace ege;

