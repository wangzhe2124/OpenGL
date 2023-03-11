#include "Tools.h"

float generateHaltonNum(int index, int base)
{
	float f = 1;
	float r = 0;
	int current = index;
	while (current > 0)
	{
		f = f / base;
		r = r + f * (current % base);
		current = glm::floor(current / base);
	}
	return r;
}

std::vector<glm::vec2> generateHaltonSquence(int base1, int base2, int num)
{
	std::vector<glm::vec2> t;
	for (int i = 1; i <= num; i++)
	{
		t.push_back(glm::vec2(generateHaltonNum(i, base1), generateHaltonNum(i, base2)));
	}
	return t;
}

