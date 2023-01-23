#pragma once
class VertexBuffer
{
private:
	unsigned int m_RendererId;
	unsigned int Number;
public:
	VertexBuffer();
    VertexBuffer(const void* data, unsigned int size);
	void BindData(const void* data, unsigned int size);
	VertexBuffer operator=(const VertexBuffer& vb);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
	inline unsigned int Get_Number() const { return Number; }
};

#include "Debugger.h"


VertexBuffer::VertexBuffer() :Number(0)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint));
    glGenBuffers(1, &m_RendererId);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);

}
VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint));
    glGenBuffers(1, &m_RendererId);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
    BindData(data, size);

}
VertexBuffer VertexBuffer::operator=(const VertexBuffer& vb)
{
    this->m_RendererId = vb.m_RendererId;
    this->Number = vb.Number;
    return *this;
}
VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_RendererId);
}

void VertexBuffer::BindData(const void* data, unsigned int size)
{
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    Number = size / sizeof(GL_FLOAT);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
