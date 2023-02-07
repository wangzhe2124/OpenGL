#pragma once
#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
   
#include<fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};
class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererId;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind () const;
	void UnBind() const;
    void SetUniform1i(const std::string& name, int value);
    void SetUniform1b(const std::string& name, bool value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float f3, float f4);
	void SetUniformmatri4fv(const std::string& name, const glm::mat4& ptr);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform3f(const std::string& name, glm::vec3 vc);
    unsigned int GetId(){return m_RendererId;}
private:
	ShaderProgramSource Parseshader();
	unsigned int CompileShader(unsigned int type,const std::string& source);
	unsigned int CreateShader(const std::string& vertexshader, const std::string& fragmentshader);
	int GetUniformLocation(const std::string& name);

};


Shader::Shader(const std::string& filepath)
    :m_FilePath(filepath), m_RendererId(0)
{
    ShaderProgramSource source = Parseshader();
    m_RendererId = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    glDeleteShader(m_RendererId);
}

void Shader::Bind() const
{
    glUseProgram(m_RendererId);
}

void Shader::UnBind() const
{
    glUseProgram(0);
}


ShaderProgramSource Shader::Parseshader()
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    std::ifstream stream(m_FilePath);
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}
unsigned int Shader::CompileShader(unsigned int type,
    const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();//&source[0]
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        std::cout << "failed." << std::endl;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "failed to compile shader!"
            << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}
unsigned int Shader::CreateShader(const std::string& vertexshader,
    const std::string& fragmentshader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexshader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentshader);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}
int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }
    int location = glGetUniformLocation(m_RendererId, name.c_str());
    if (location == -1)
        std::cout << "warning: uniform '" << name << "' doesn't exist!" << std::endl;
    m_UniformLocationCache[name] = location;
    return location;

}
void Shader::SetUniform1i(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}
void Shader::SetUniform1b(const std::string& name, bool value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformmatri4fv(const std::string& name, const glm::mat4& ptr)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &ptr[0][0]);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}
void Shader::SetUniform3f(const std::string& name, glm::vec3 vc)
{
    glUniform3fv(GetUniformLocation(name), 1, &vc[0]);
}


struct D3ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
    std::string GeometrySource;
};
class D3Shader
{
private:
    std::string m_FilePath;
    unsigned int m_RendererId;
    std::unordered_map<std::string, int> m_UniformLocationCache;
public:
    D3Shader(const std::string& filepath);
    ~D3Shader();

    void Bind() const;
    void UnBind() const;
    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform4f(const std::string& name, float v0, float v1, float f3, float f4);
    void SetUniformmatri4fv(const std::string& name, const glm::mat4& ptr);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniform3f(const std::string& name, glm::vec3 vc);
    unsigned int GetId() { return m_RendererId; }
private:
    D3ShaderProgramSource Parseshader();
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string& vertexshader, const std::string& fragmentshader, const std::string& geometryshader);
    int GetUniformLocation(const std::string& name);

};


D3Shader::D3Shader(const std::string& filepath)
    :m_FilePath(filepath), m_RendererId(0)
{
    D3ShaderProgramSource source = Parseshader();
    m_RendererId = CreateShader(source.VertexSource, source.FragmentSource, source.GeometrySource);
}

D3Shader::~D3Shader()
{
    glDeleteShader(m_RendererId);
}

void D3Shader::Bind() const
{
    glUseProgram(m_RendererId);
}

void D3Shader::UnBind() const
{
    glUseProgram(0);
}


D3ShaderProgramSource D3Shader::Parseshader()
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
    };
    std::ifstream stream(m_FilePath);
    std::string line;
    std::stringstream ss[3];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
            else if (line.find("geometry") != std::string::npos)
                type = ShaderType::GEOMETRY;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str(), ss[2].str() };
}
unsigned int D3Shader::CompileShader(unsigned int type,
    const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();//&source[0]
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        std::cout << "failed." << std::endl;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "failed to compile shader!"
            << (type == GL_VERTEX_SHADER ? "vertex" : (type == GL_FRAGMENT_SHADER ? "fragment" : " geometry"))
            << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}
unsigned int D3Shader::CreateShader(const std::string& vertexshader,
    const std::string& fragmentshader, const std::string& geometryshader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexshader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentshader);
    unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, geometryshader);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glAttachShader(program, gs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteShader(gs);
    return program;
}
int D3Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }
    int location = glGetUniformLocation(m_RendererId, name.c_str());
    if (location == -1)
        std::cout << "warning: uniform '" << name << "' doesn't exist!" << std::endl;
    m_UniformLocationCache[name] = location;
    return location;

}
void D3Shader::SetUniform1i(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void D3Shader::SetUniform1f(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}
void D3Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void D3Shader::SetUniformmatri4fv(const std::string& name, const glm::mat4& ptr)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &ptr[0][0]);
}

void D3Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}
void D3Shader::SetUniform3f(const std::string& name, glm::vec3 vc)
{
    glUniform3fv(GetUniformLocation(name), 1, &vc[0]);
}

class Shaders
{
public:
    Shader screenShader = Shader("res/shaders/screen.shader");
    Shader blooming_highlightshader = Shader("res/shaders/blooming_highlight.shader");
    Shader blooming_blurshader = Shader("res/shaders/blooming_blur.shader");
    Shader shadow_blurshader = Shader("res/shaders/shadow_blur.shader");
    Shader DeferedShader = Shader("res/shaders/Defered.shader");
    Shader DeferedPreShadowShader = Shader("res/shaders/DeferedPreShadow.shader");
    Shader SSAOShader = Shader("res/shaders/SSAO.shader");
    Shader SSAOBlurShader = Shader("res/shaders/SSAOblur.shader");
    Shader basic_shader = Shader("res/shaders/basic.shader");
    Shader basicscreen_shader = Shader("res/shaders/basic_screen.shader");
    Shader EnvCubeMapShader = Shader("res/shaders/EnvCubeMap.shader");
    Shader EnvCubeMap_ConvolutionShader = Shader("res/shaders/EnvCubeMap_Convolution.shader");
    Shader EnvCubeMap_spec_ConvolutionShader = Shader("res/shaders/EnvCubeMap_spec_Convolution.shader");
    Shader EnvCubeMap_spec_BRDF_Shader = Shader("res/shaders/EnvCubeMap_spec_BRDF.shader");
    Shader DeferedLighting_shader = Shader("res/shaders/DeferedLighting.shader");
    //D3Shader model_geometry_shader("res/shaders/model_geomrtry.shader");
    //Shader model_instance_shader = Shader("res/shaders/model_instance.shader");
    //Shader tencil = Shader("res/shaders/single.shader");
    Shader skyboxShader = Shader("res/shaders/SkyBox.shader");
    Shader DirLightShadowshader = Shader("res/shaders/DirLightShadow.shader");
    D3Shader PointLightShadowshader = D3Shader("res/shaders/PointLightShadow.shader");
    Shader SpotLightShadowshader = Shader("res/shaders/SpotLightShadow.shader");
    Shader text_shader = Shader("res/shaders/Text.shader");
};