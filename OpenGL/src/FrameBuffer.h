#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class CameraDepthFBO
{
private:
	unsigned int frameBufferId;
	unsigned int DepthTextureId;
	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	CameraDepthFBO(unsigned int scr_width, unsigned int scr_height) :screenWidth(scr_width), screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &DepthTextureId);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//只需要深度
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTextureId, 0);
	}
	~CameraDepthFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &DepthTextureId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};
class SSAOFBO
{
private:
	unsigned int frameBufferId;
	unsigned int ssaoTextureId;
	unsigned int noiseTextureId;
	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	SSAOFBO(unsigned int scr_width, unsigned int scr_height, std::vector<glm::vec3>& ssaoNoise) :screenWidth(scr_width), screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindNoiseBuffer(ssaoNoise);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &ssaoTextureId);
		glBindTexture(GL_TEXTURE_2D, ssaoTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTextureId, 0);
	}
	void BindNoiseBuffer(std::vector<glm::vec3>& ssaoNoise)
	{

		glGenTextures(1, &noiseTextureId);
		glBindTexture(GL_TEXTURE_2D, noiseTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, noiseTextureId, 0);
	}

	~SSAOFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &ssaoTextureId);
		glDeleteFramebuffers(1, &noiseTextureId);

	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, ssaoTextureId);
	};
	void BindNoiseTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, noiseTextureId);
	};

	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};
class SSAOBlurFBO
{
private:
	unsigned int frameBufferId;
	unsigned int ssaoBlurTextureId;
	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	SSAOBlurFBO(unsigned int scr_width, unsigned int scr_height) :screenWidth(scr_width), screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &ssaoBlurTextureId);
		glBindTexture(GL_TEXTURE_2D, ssaoBlurTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);//只需要深度
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurTextureId, 0);
	}


	~SSAOBlurFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &ssaoBlurTextureId);

	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, ssaoBlurTextureId);
	};

	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};

class Blooming_HighlightFBO
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId[2];
	unsigned int RenderBufferId;
	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	Blooming_HighlightFBO(unsigned int scr_width, unsigned int scr_height) :screenWidth(scr_width), screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		//BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(2, textureColorId);
		for (int i = 0; i < 2; i++)
		{

			glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);//使用高精度32F颜色缓冲HDR
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureColorId[i], 0);
		}
		GLuint colorattachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, colorattachments);
	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~Blooming_HighlightFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, textureColorId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};

class Blooming_Blur_HorizontalFBO
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId;
	unsigned int RenderBufferId;
	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	Blooming_Blur_HorizontalFBO(unsigned int scr_width, unsigned int scr_height) :screenWidth(scr_width), screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &textureColorId);
		glBindTexture(GL_TEXTURE_2D, textureColorId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);//使用高精度32F颜色缓冲HDR
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorId, 0);	

	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~Blooming_Blur_HorizontalFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &textureColorId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};
class Blooming_Blur_VerticalFBO
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId;
	unsigned int RenderBufferId;
	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	Blooming_Blur_VerticalFBO(unsigned int scr_width, unsigned int scr_height) :screenWidth(scr_width), screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &textureColorId);
		glBindTexture(GL_TEXTURE_2D, textureColorId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);//使用高精度32F颜色缓冲HDR
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorId, 0);

	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~Blooming_Blur_VerticalFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &textureColorId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};
class Shadow_Blur_HorizontalFBO
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId;
	unsigned int RenderBufferId;
	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	Shadow_Blur_HorizontalFBO(unsigned int scr_width, unsigned int scr_height) :screenWidth(scr_width), screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &textureColorId);
		glBindTexture(GL_TEXTURE_2D, textureColorId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);//使用高精度32F颜色缓冲HDR
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorId, 0);

	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~Shadow_Blur_HorizontalFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &textureColorId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};
class Shadow_Blur_VerticalFBO
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId;
	unsigned int RenderBufferId;
	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	Shadow_Blur_VerticalFBO(unsigned int scr_width, unsigned int scr_height) :screenWidth(scr_width), screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &textureColorId);
		glBindTexture(GL_TEXTURE_2D, textureColorId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);//使用高精度32F颜色缓冲HDR
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorId, 0);

	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~Shadow_Blur_VerticalFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &textureColorId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_BUFFER, frameBufferId);
	};
	void ReadTexture()
	{
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorId, 0);
	};
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};
class GBuffer
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId[3];
	unsigned int RenderBufferId;

	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	GBuffer(unsigned int scr_width, unsigned int scr_height) :screenWidth(scr_width), screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(3, textureColorId);
		for (int i = 0; i < 3; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);//使用高精度32F颜色缓冲HDR
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			GLfloat max_TexAni;    //查询允许的各向异性数量
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_TexAni);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_TexAni);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureColorId[i], 0);
		}
		GLuint colorattachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, colorattachments);
	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~GBuffer()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, textureColorId);
		glDeleteFramebuffers(1, &RenderBufferId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
	};
	void BindDepth(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, RenderBufferId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void WriteTexture(int i = 0)
	{
		glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, screenWidth, screenHeight);

	};
	void unWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
	}
	void unRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
	void BlitDepthBuffer()
	{
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	};

	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};
class MSAAFrameBuffer
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId;
	unsigned int ambientId;
	unsigned int renderBufferId;
	unsigned int screenWidth;
	unsigned int screenHeight;
	unsigned int SampleNumber;
public:
	MSAAFrameBuffer(unsigned int scr_width, unsigned int scr_height, unsigned int SN) : screenWidth(scr_width), screenHeight(scr_height), SampleNumber(SN)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, &textureColorId);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorId);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SampleNumber, GL_RGB32F, screenWidth, screenHeight, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorId, 0);

		glGenTextures(1, &ambientId);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ambientId);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SampleNumber, GL_RGB32F, screenWidth, screenHeight, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, ambientId, 0);
		GLuint colorattachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, colorattachments);
	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &renderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, SampleNumber, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId);
	};
	~MSAAFrameBuffer()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &textureColorId);
		glDeleteFramebuffers(1, &renderBufferId);
		glDeleteFramebuffers(1, &ambientId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture()
	{
		glBindTexture(GL_TEXTURE_2D, textureColorId);
	};
	void BindambientTexture()
	{
		glBindTexture(GL_TEXTURE_2D, ambientId);
	};
	void BlitBuffer()
	{
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
};
class HDRFBO
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId[1];
	unsigned int RenderBufferId;

	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	HDRFBO(unsigned int scr_width, unsigned int scr_height) :screenWidth(scr_width), screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, textureColorId);
		for (int i = 0; i < 1; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);//使用高精度32F颜色缓冲HDR
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			GLfloat max_TexAni;    //查询允许的各向异性数量
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_TexAni);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_TexAni);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureColorId[i], 0);
		}
		GLuint colorattachments[1] = { GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, colorattachments);
	};
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~HDRFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, textureColorId);
		glDeleteFramebuffers(1, &RenderBufferId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
	};
	void BindDepth(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, RenderBufferId);
	};
	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void unWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
	}
	void unRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
	void BlitDepthBuffer()
	{
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	};

	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};


class DirLightDepthMapFBO
{
private:
	unsigned int frameBufferId;
	unsigned int DepthTextureId;
	unsigned int RenderBufferId;
	unsigned int screenWidth;//分辨率
	unsigned int screenHeight;
public:
	DirLightDepthMapFBO(unsigned int ScreenWidth, unsigned int ScreenHeight) :screenWidth(ScreenWidth), screenHeight(ScreenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		//glDrawBuffer(GL_NONE);
		//glReadBuffer(GL_NONE);
		BindRenderBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void BindTextureBuffer()
	{
		glGenTextures(1, &DepthTextureId);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);//只需要深度
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, DepthTextureId, 0);
	}
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~DirLightDepthMapFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &DepthTextureId);
	}
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
	}
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};


class PointLightDepthMapFBO
{
private:
	unsigned int frameBufferId;
	unsigned int DepthTextureId;
	unsigned int screenWidth;//分辨率
	unsigned int screenHeight;
public:
	PointLightDepthMapFBO(unsigned int ScreenWidth, unsigned int ScreenHeight) :screenWidth(ScreenWidth), screenHeight(ScreenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
	}
	void BindTextureBuffer()
	{
		glGenTextures(1, &DepthTextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, DepthTextureId);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//只需要深度
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthTextureId, 0);
	}
	~PointLightDepthMapFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &DepthTextureId);
	}
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, DepthTextureId);
	}
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};

class SpotLightDepthMapFBO
{
private:
	unsigned int frameBufferId;
	unsigned int DepthTextureId;
	unsigned int RenderBufferId;
	unsigned int screenWidth;//分辨率
	unsigned int screenHeight;
public:
	SpotLightDepthMapFBO(unsigned int ScreenWidth, unsigned int ScreenHeight) :screenWidth(ScreenWidth), screenHeight(ScreenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	}
	void BindTextureBuffer()
	{
		glGenTextures(1, &DepthTextureId);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);//只需要深度
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTextureId, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 0.0, 0.0, 0.0, 0.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}
	void BindRenderBuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId);
	};
	~SpotLightDepthMapFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &DepthTextureId);
	}
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, DepthTextureId);
	}
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};

class EnvCubeMapFBO
{
private:
	unsigned int frameBufferId;
	unsigned int TextureId;
	unsigned int RenderBufferId;
	unsigned int screenWidth;//分辨率
	unsigned int screenHeight;
public:
	EnvCubeMapFBO(unsigned int ScreenWidth, unsigned int ScreenHeight) :screenWidth(ScreenWidth), screenHeight(ScreenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderbuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void BindTextureBuffer()
	{
		glGenTextures(1, &TextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);//只需要深度

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X , TextureId, 0);

		//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureId, 0);
	}
	void BindRenderbuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBufferId);
	}
	~EnvCubeMapFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &TextureId);
		glDeleteFramebuffers(1, &RenderBufferId);
	}
	void Bind(int i = 0)
	{	if(i == 0)
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, TextureId, 0);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
	}
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};
class EnvCubeMap_ConvolutionFBO
{
private:
	unsigned int frameBufferId;
	unsigned int TextureId;
	unsigned int RenderBufferId;
	unsigned int screenWidth;//分辨率
	unsigned int screenHeight;
public:
	EnvCubeMap_ConvolutionFBO(unsigned int ScreenWidth, unsigned int ScreenHeight) :screenWidth(ScreenWidth), screenHeight(ScreenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		BindRenderbuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void BindTextureBuffer()
	{
		glGenTextures(1, &TextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);//只需要深度

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, TextureId, 0);

		//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureId, 0);
	}
	void BindRenderbuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBufferId);
	}
	~EnvCubeMap_ConvolutionFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &TextureId);
		glDeleteFramebuffers(1, &RenderBufferId);
	}
	void Bind(int i = 0)
	{
		if (i == 0)
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, TextureId, 0);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
	}
	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};
class EnvCubeMap_spec_ConvolutionFBO
{
private:
	unsigned int frameBufferId;
	unsigned int TextureId;
	unsigned int RenderBufferId;
	unsigned int screenWidth;//分辨率 128
	unsigned int screenHeight;
public:
	EnvCubeMap_spec_ConvolutionFBO(unsigned int ScreenWidth, unsigned int ScreenHeight) :screenWidth(ScreenWidth), screenHeight(ScreenHeight)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		//BindRenderbuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void BindTextureBuffer()
	{
		glGenTextures(1, &TextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);//只需要深度

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, TextureId, 0);//先绑定一个防止报错
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureId, 0);
	}
	void BindRenderbuffer()
	{
		glGenRenderbuffers(1, &RenderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBufferId);
	}
	void Bindmip_Renderbuffer(unsigned int width, unsigned int height)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
	}
	~EnvCubeMap_spec_ConvolutionFBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, &TextureId);
		glDeleteFramebuffers(1, &RenderBufferId);
	}
	void Bind(int i = 0, int j =0)
	{
		if (i == 0)
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, TextureId, j);

	}
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void BindTexture(unsigned int slot = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TextureId);
	}
	void SetViewPort(unsigned int width, unsigned int height)
	{
		glViewport(0, 0, width, height);
	}
};
class EnvCubeMap_spec_BRDF_FBO
{
private:
	unsigned int frameBufferId;
	unsigned int textureColorId[1];

	unsigned int screenWidth;
	unsigned int screenHeight;
public:
	EnvCubeMap_spec_BRDF_FBO(unsigned int scr_width, unsigned int scr_height) :screenWidth(scr_width), screenHeight(scr_height)
	{
		glGenFramebuffers(1, &frameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
		BindTextureBuffer();
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTextureBuffer()
	{
		glGenTextures(1, textureColorId);
		for (int i = 0; i < 1; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, screenWidth, screenHeight, 0, GL_RG, GL_FLOAT, NULL);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureColorId[i], 0);
		}
		GLuint colorattachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, colorattachments);
	};
	~EnvCubeMap_spec_BRDF_FBO()
	{
		glDeleteFramebuffers(1, &frameBufferId);
		glDeleteFramebuffers(1, textureColorId);
	};
	void Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
	};
	void UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};
	void BindTexture(unsigned int slot = 0, int i = 0)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureColorId[i]);
	};

	void Write()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferId);
	};
	void unWrite()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	};
	void Read()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId);
	}
	void unRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
	void BlitDepthBuffer()
	{
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	};

	void SetViewPort()
	{
		glViewport(0, 0, screenWidth, screenHeight);
	}
};
