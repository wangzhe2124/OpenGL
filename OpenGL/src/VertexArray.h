#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
class VertexArray
{
private:
	unsigned int m_RendererId;
	unsigned int Number;
	unsigned int j;
public:
	VertexArray();
	VertexArray(VertexArray& va);
	~VertexArray();
	inline unsigned int GetNumber() const { return Number; }
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	
	void Bind() const;
	void Unbind() const;
	unsigned int GetId() { return m_RendererId; }
	unsigned int GetNum() { return Number; }
	unsigned int GetJ() { return j; }
};

#include "Debugger.h"
VertexArray::VertexArray() :Number(0), j(0)
{
	glGenVertexArrays(1, &m_RendererId);
	glBindVertexArray(m_RendererId);

}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_RendererId);
}

VertexArray::VertexArray(VertexArray& va)
{
	m_RendererId = va.GetId();
	Number = va.GetNum();
	j = va.GetJ();
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto &element = elements[i];
		glEnableVertexAttribArray(i+j);
		glVertexAttribPointer(i + j, element.count, element.type, element.normalize, layout.GetStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	j += (unsigned int)elements.size();
	Number += vb.Get_Number()/layout.GetStride();
	vb.Unbind();
	Unbind();
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererId);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);

}

class VertexArrays
{
public:
	VertexArray quadVa;
	VertexArray cubeVa;
	VertexArray skyboxVa;
	VertexArray textVa;
	VertexArray particleVa;
};