#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
class VertexArray
{
private:
	unsigned int m_RendererId;
	unsigned int Number;
public:
	VertexArray();
	~VertexArray();
	inline unsigned int GetNumber() const { return Number; }
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	
	void Bind() const;
	void Unbind() const;
};

#include "Debugger.h"
VertexArray::VertexArray() :Number(0)
{
	glGenVertexArrays(1, &m_RendererId);
	glBindVertexArray(m_RendererId);

}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_RendererId);
}



void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{


	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalize, layout.GetStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	Number = vb.Get_Number();

}

void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererId);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);

}
