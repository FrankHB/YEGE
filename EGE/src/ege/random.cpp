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
	return std::uniform_int_distribution<unsigned>(0, n)(mt);
}

double
randomf()
{
	return std::uniform_real_distribution<float>()(mt);
}

} // namespace ege;

