#include "ege/random.h"
#include <random>
#include <memory>

namespace ege
{

namespace
{

static std::random_device rd;

std::unique_ptr<std::mt19937>
rand_p()
{
	return std::unique_ptr<std::mt19937>(new std::mt19937(rd()));
}

static std::unique_ptr<std::mt19937> p_mt(rand_p());

}

void
randomize()
{
	p_mt = rand_p();
}

unsigned
random(unsigned n)
{
	std::uniform_int_distribution<unsigned> d(0, n);

	return d(*p_mt);
}

double
randomf()
{
	std::uniform_real_distribution<float> d;

	return d(*p_mt);
}

} // namespace ege

