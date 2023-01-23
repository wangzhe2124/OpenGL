#pragma once
#include <iostream>
#include <GL/glew.h>
class Texture
{
private:
	unsigned int m_RenderId;
	std::string m_FilePath;
	unsigned char* Data;
	int m_Width, m_Height, m_Channel;

	std::string m_type;
public:

	Texture(const std::string path, const std::string type);
	Texture();
	void BindData(const std::string path, const std::string  type);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void UnBind() const;
	Texture(const Texture& t);
	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline std::string GetType() const { return m_type; }
};

#include "vender/stb_image/stb_image.h"
Texture::Texture(const std::string path, const std::string type) :m_RenderId(0), m_FilePath("null"), Data(nullptr),
m_Width(0), m_Height(0), m_Channel(0), m_type("null")
{
	glGenTextures(1, &m_RenderId);
	glBindTexture(GL_TEXTURE_2D, m_RenderId);
	BindData(path, type);
}
void Texture::BindData(const std::string path, const std::string type)
{
	m_type = type;
	stbi_set_flip_vertically_on_load(1);//旋转
	Data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channel, 4);

	glBindTexture(GL_TEXTURE_2D, m_RenderId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_LINEAR,GL_NEAREST_MIPMAP_NEAREST,GL_LINEAR_MIPMAP_NEAREST,GL_NEAREST_MIPMAP_LINEAR,GL_LINEAR_MIPMAP_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_NEAREST
	glBindTexture(GL_TEXTURE_2D, 0);
	if (Data)
	{
		stbi_image_free(Data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
}
Texture::Texture() :m_RenderId(0), m_FilePath("null"), Data(nullptr),
m_Width(0), m_Height(0), m_Channel(0), m_type("null")
{
	glGenTextures(1, &m_RenderId);
	glBindTexture(GL_TEXTURE_2D, m_RenderId);
}
Texture::Texture(const Texture& t)
{
	m_RenderId = t.m_RenderId;
	m_FilePath = t.m_FilePath;
	Data = t.Data;
	m_Width = t.m_Width;
	m_Height = t.m_Height;
	m_Channel = t.m_Channel;
	m_type = t.m_type;
}
Texture::~Texture()
{
	glDeleteTextures(1, &m_RenderId);
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);//激活纹理单元
	glBindTexture(GL_TEXTURE_2D, m_RenderId);
}

void Texture::UnBind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);

}
