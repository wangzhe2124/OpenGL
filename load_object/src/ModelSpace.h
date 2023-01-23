#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class ModelSpace
{
private:
	glm::mat4 modelspace;
public:
	ModelSpace();
	ModelSpace operator=(const ModelSpace& ms) { this->modelspace = ms.modelspace; }
	void SetModel(glm::mat4& mo);
	void Translate(glm::vec3 tr);
	void Rotate(float angle, const glm::vec3& axis);
	void Scale(const glm::vec3& sc);
	inline glm::mat4 GetModelSpace() { return modelspace; }

};

ModelSpace::ModelSpace() :modelspace(glm::mat4(1.0f))
{
}

void ModelSpace::SetModel(glm::mat4& mo)
{
	modelspace = mo;
}

void ModelSpace::Translate(glm::vec3 tr)
{
	modelspace = glm::translate(modelspace, tr);
}

void ModelSpace::Rotate(float angle, const glm::vec3& axis)
{
	modelspace = glm::rotate(modelspace, glm::radians(angle), axis);
}

void ModelSpace::Scale(const glm::vec3& sc)
{
	modelspace = glm::scale(modelspace, sc);
}
