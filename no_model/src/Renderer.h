
#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Renderer
{
public:
	void Clear(unsigned int cl) const;
	void ClearColor(float c1, float c2, float c3, float c4) const;
	void DrawElement(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	//void DrawMeshElement(Mesh& mesh, const Shader& shader) const;

	void DrawArray(const VertexArray& va, const Shader& shader) const;
	//void DrawMeshArray(Mesh& mesh,  const Shader& shader) const;
	
};

void Renderer::Clear(unsigned int cl) const
{
    switch (cl)
    {
    case 0:glClear(GL_COLOR_BUFFER_BIT);
    case 1:glClear(GL_DEPTH_BUFFER_BIT);
    case 2:glClear(GL_STENCIL_BUFFER_BIT);
    }

}

void Renderer::ClearColor(float c1, float c2, float c3, float c4) const
{
    glClearColor(c1, c2, c3, c4);
}

void Renderer::DrawElement(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    va.Bind();
    ib.Bind();
    shader.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}


void Renderer::DrawArray(const VertexArray& va, const Shader& shader) const
{
    va.Bind();
    shader.Bind();
    glDrawArrays(GL_TRIANGLES, 0, va.GetNumber());
}
