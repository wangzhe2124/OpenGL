#pragma once
#include <iostream>
#include <GL/glew.h>
#include "vender/stb_image/stb_image.h"
#include <vector>
class Texture
{
private:
	unsigned int RenderId;
	unsigned char* Data;	
public:
	Texture(const std::string path, int inChannel) :RenderId(0), Data(nullptr)
	{
		glGenTextures(1, &RenderId);
		glBindTexture(GL_TEXTURE_2D, RenderId);
		BindData(path, inChannel);
	};
	void BindData(const std::string path, int inChannel)
	{	stbi_set_flip_vertically_on_load(1);//旋转
		int width, height, outChannel;
		Data = stbi_load(path.c_str(), &width, &height, &outChannel, inChannel);
		if (Data)
		{
			GLenum outformat;
			if (outChannel == 1)
				outformat = GL_RED;
			else if (outChannel == 3)
				outformat = GL_SRGB;
			else if (outChannel == 4)
				outformat = GL_SRGB_ALPHA;
			GLenum informat;
			if (inChannel == 1)
				informat = GL_RED;
			else if (inChannel == 3)
				informat = GL_RGB;
			else if (inChannel == 4)
				informat = GL_RGBA;
			glBindTexture(GL_TEXTURE_2D, RenderId);
			glTexImage2D(GL_TEXTURE_2D, 0, outformat, width, height, 0, informat, GL_UNSIGNED_BYTE, Data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR,GL_NEAREST_MIPMAP_NEAREST,GL_LINEAR_MIPMAP_NEAREST,GL_NEAREST_MIPMAP_LINEAR,GL_LINEAR_MIPMAP_LINEAR
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_NEAREST
			GLfloat max_TexAni;    //查询允许的各向异性数量
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_TexAni);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_TexAni);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			stbi_image_free(Data);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
			stbi_image_free(Data);
		}
		stbi_set_flip_vertically_on_load(0);//旋转

	};
	~Texture()
	{
		glDeleteTextures(1, &RenderId);
	};

	void Bind(unsigned int slot = 0) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);//激活纹理单元
		glBindTexture(GL_TEXTURE_2D, RenderId);
	};
	void UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	};
	void SetRepeat()
	{
		glBindTexture(GL_TEXTURE_2D, RenderId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	};
};
class HDRTexture
{
private:
	unsigned int RenderId;
	unsigned char* Data;
public:
	HDRTexture(const std::string path, int inChannel) :RenderId(0), Data(nullptr)
	{
		glGenTextures(1, &RenderId);
		glBindTexture(GL_TEXTURE_2D, RenderId);
		BindData(path, inChannel);
	};
	void BindData(const std::string path, int inChannel)
	{	stbi_set_flip_vertically_on_load(1);//旋转
		int width, height, outChannel;
		Data = stbi_load(path.c_str(), &width, &height, &outChannel, inChannel);
		if (Data)
		{
			GLenum outformat;
			if (outChannel == 1)
				outformat = GL_RED;
			else if (outChannel == 3)
				outformat = GL_SRGB;
			else if (outChannel == 4)
				outformat = GL_SRGB_ALPHA;
			GLenum informat;
			if (inChannel == 1)
				informat = GL_RED;
			else if (inChannel == 3)
				informat = GL_RGB;
			else if (inChannel == 4)
				informat = GL_RGBA;
			glBindTexture(GL_TEXTURE_2D, RenderId);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, informat, GL_UNSIGNED_BYTE, Data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR,GL_NEAREST_MIPMAP_NEAREST,GL_LINEAR_MIPMAP_NEAREST,GL_NEAREST_MIPMAP_LINEAR,GL_LINEAR_MIPMAP_LINEAR
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//GL_NEAREST
			GLfloat max_TexAni;    //查询允许的各向异性数量
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_TexAni);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_TexAni);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
			stbi_image_free(Data);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
			stbi_image_free(Data);
		}
		stbi_set_flip_vertically_on_load(0);//旋转
	};
	~HDRTexture()
	{
		glDeleteTextures(1, &RenderId);
	};

	void Bind(unsigned int slot = 0) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);//激活纹理单元
		glBindTexture(GL_TEXTURE_2D, RenderId);
	};
	void UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	};
	void SetRepeat()
	{
		glBindTexture(GL_TEXTURE_2D, RenderId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	};
};

class CubeMapTexture
{
private:
	unsigned int CubeMapId;
	unsigned char* Data;
public:
	CubeMapTexture(std::vector<std::string> faces) : CubeMapId(0), Data(nullptr)
	{
		glGenTextures(1, &CubeMapId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapId);
		int width, height, nrChannel;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			Data = stbi_load(faces[i].c_str(), &width, &height, &nrChannel, 0);
			if (Data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
				stbi_image_free(Data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(Data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	};
	void Bind(unsigned int slot = 0) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);//激活纹理单元
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapId);
	};
	void UnBind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	};
	~CubeMapTexture()
	{
		glDeleteTextures(1, &CubeMapId);
	};
};

#pragma endregion