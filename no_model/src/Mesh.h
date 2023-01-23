#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

class Mesh 
{
private:

public:
    VertexArray va;
    VertexBuffer vb;
    IndexBuffer ib;
    VertexBufferLayout layout;
    /*  º¯Êý  */
    Mesh();
    ~Mesh();
    void BindTexture(Texture& texture, const std::string path, const std::string type, Shader& shader, int i );
    void BindVertex(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void BindSlot(Texture& texture, Shader& shader, const unsigned int i);
    void BindTextureVector(std::vector<Texture>& t);

};