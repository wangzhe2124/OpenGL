#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
float generateHaltonNum(int index, int base);

std::vector<glm::vec2> generateHaltonSquence(int base1, int base2, int num);
#define Halton_Num 1024
static const std::vector<glm::vec2> HaltonSequence = generateHaltonSquence(2, 3, Halton_Num);