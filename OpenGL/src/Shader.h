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
    std::string GeometrySource;
    std::string TessContSource;
    std::string TessEvalSource;
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
    void SetUniform2f(const std::string& name, float value1, float value2);
    void SetUniform2f(const std::string& name, glm::vec2 value);
	void SetUniform4f(const std::string& name, float v0, float v1, float f3, float f4);
    void SetUniform4f(const std::string& name, glm::vec4& value);
	void SetUniformmatri4fv(const std::string& name, const glm::mat4& ptr);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform3f(const std::string& name, glm::vec3 vc);
    unsigned int GetId(){return m_RendererId;}
private:
	ShaderProgramSource Parseshader();
	unsigned int CompileShader(unsigned int type,const std::string& source);
	unsigned int CreateShader(const ShaderProgramSource& source);
	int GetUniformLocation(const std::string& name);

};


Shader::Shader(const std::string& filepath)
    :m_FilePath(filepath), m_RendererId(0)
{
    ShaderProgramSource source = Parseshader();
    m_RendererId = CreateShader(source);
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
        NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2, TessCont = 3, TessEval = 4
    };
    std::ifstream stream(m_FilePath);
    std::string line;
    std::stringstream ss[5];
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
            else if (line.find("tess_cont") != std::string::npos)
                type = ShaderType::TessCont;
            else if (line.find("tess_eval") != std::string::npos)
                type = ShaderType::TessEval;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str(), ss[2].str(), ss[3].str(), ss[4].str() };
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
        std::cout << "failed to compile shader!" << std::endl;
        std::string erro_message = "";
        switch (type)
        {
        case GL_VERTEX_SHADER: erro_message += "vertex ";
        case GL_FRAGMENT_SHADER:erro_message += "fragment ";
        case GL_GEOMETRY_SHADER:erro_message += "geometry ";
        case GL_TESS_CONTROL_SHADER:erro_message += "tess_cont ";
        case GL_TESS_EVALUATION_SHADER:erro_message += "tess_eval ";
            break;
        }
        std::cout << erro_message << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}
unsigned int Shader::CreateShader(const ShaderProgramSource& source)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = 0;
    if (!source.VertexSource.empty())
    {
        vs = CompileShader(GL_VERTEX_SHADER, source.VertexSource);
        glAttachShader(program, vs);
    }
    unsigned int fs = 0;
    if (!source.FragmentSource.empty())
    {
        fs = CompileShader(GL_FRAGMENT_SHADER, source.FragmentSource);
        glAttachShader(program, fs);
    }
    unsigned int gs = 0;
    if (!source.GeometrySource.empty())
    {
        gs = CompileShader(GL_GEOMETRY_SHADER, source.GeometrySource);
        glAttachShader(program, gs);
    }
    unsigned int tcs = 0;
    if (!source.TessContSource.empty())
    {
        tcs = CompileShader(GL_TESS_CONTROL_SHADER, source.TessContSource);
        glAttachShader(program, tcs);
    }
    unsigned int tes = 0;
    if (!source.TessEvalSource.empty())
    {
        tes = CompileShader(GL_TESS_EVALUATION_SHADER, source.TessEvalSource);
        glAttachShader(program, tes);
    }
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteShader(gs);
    glDeleteShader(tcs);
    glDeleteShader(tes);
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
void Shader::SetUniform2f(const std::string& name, float value1, float value2)
{
    glUniform2f(GetUniformLocation(name), value1, value2);
}
void Shader::SetUniform2f(const std::string& name, glm::vec2 value)
{
    glUniform2fv(GetUniformLocation(name),1, &value[0]);
}
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}
void Shader::SetUniform4f(const std::string& name, glm::vec4& value)
{
    glUniform4fv(GetUniformLocation(name), 1, &value[0]);
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
    Shader PointLightShadowshader = Shader("res/shaders/PointLightShadow.shader");
    Shader SpotLightShadowshader = Shader("res/shaders/SpotLightShadow.shader");
    Shader text_shader = Shader("res/shaders/Text.shader");
    Shader Particle_shader = Shader("res/shaders/Particle.shader");
    Shader D3Particle_shader = Shader("res/shaders/D3Particle.shader");
    Shader FXAA_shader = Shader("res/shaders/FXAA.shader");
    Shader Terrain_cpu_shader = Shader("res/shaders/Terrain_cpu.shader");
    Shader Terrain_gpu_shader = Shader("res/shaders/Terrain_gpu.shader");
    Shader Health_bar_shader = Shader("res/shaders/Health_bar.shader");
    Shader Health_bar_enemy_shader = Shader("res/shaders/Health_bar_enemy.shader");
};