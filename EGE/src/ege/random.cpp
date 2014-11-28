#include "ege/random.h"
#include <random>

namespace ege
{

namespace
{

std::random_device rd;
std::mt19937 mt(rd());

} // unnamed namespace;

void
randomize()
{
	mt = std::mt19937(rd());
}

unsigned
random(unsigned n)
{
	std::uniform_int_distribution<unsigned> d(0, n);

	return d(mt);
}

double
randomf()
{
	std::uniform_real_distribution<float> d;

	return d(mt);
}

} // namespace ege;

