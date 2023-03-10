#pragma once
class IndexBuffer
{
private:
	unsigned int m_RendererId;
	unsigned int m_Count;
public:
	IndexBuffer();
	IndexBuffer(const unsigned int* data, unsigned int count);
	void BindData(const unsigned int* data, unsigned int count);
	IndexBuffer operator=(const IndexBuffer& vb);

	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
	inline unsigned int GetCount() const { return m_Count; }
};

#include "Debugger.h"

IndexBuffer::IndexBuffer() :m_Count(0)
{
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));
	glGenBuffers(1, &m_RendererId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);

}
IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) :m_Count(0)
{
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));
	glGenBuffers(1, &m_RendererId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
	BindData(data, count);
}
IndexBuffer IndexBuffer::operator=(const IndexBuffer& ib)
{
	this->m_RendererId = ib.m_RendererId;
	this->m_Count = ib.m_Count;
	return *this;
}
IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_RendererId);
}
void IndexBuffer::BindData(const unsigned int* data, unsigned int count)
{
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	m_Count = count;
}

void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
}

void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
