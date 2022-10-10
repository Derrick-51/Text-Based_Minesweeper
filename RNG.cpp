#include <random>

static std::random_device rd;
static std::seed_seq ss{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };

extern std::mt19937 mt{ ss };
extern std::uniform_int_distribution randPercent{ 1, 100 };

int generateRandInt(int min, int max)
{
	std::uniform_int_distribution randInt{ min, max };

	return randInt(mt);
}