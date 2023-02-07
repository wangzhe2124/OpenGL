#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <random>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Debugger.h"
#include "Shader.h"
#include "KeyInput.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Model.h"
#include "ModelSpace.h"
#include "Camera.h"
#include "Renderer.h"
#include "FrameBuffer.h"
#include "CSM.h"
#include "vender/imgui/imgui.h"
#include "vender/imgui/imgui_impl_glfw.h"
#include "vender/imgui/imgui_impl_opengl3.h"
#include "Config.h"
#include "ft2build.h"
#include "Text.h"
#include FT_FREETYPE_H 
constexpr auto CSM_SPLIT_NUM = 4;
constexpr auto POINT_LIGHTS_NUM = 4;
constexpr auto PI = 3.14159265359;
struct Sun_DATA
{
    glm::vec3 Sun_Position;
    glm::vec3 Sun_Direction;
    Sun_DATA():Sun_Position(0.0f),Sun_Direction(0.0f)
    {};
    Sun_DATA(glm::vec3 sp, glm::vec3 sd) :Sun_Position(sp), Sun_Direction(sd)
    {};
};


struct SpotLight_DATA
{
    float spotlight_near_plane = 0.1f;
    float spotlight_far_plane = 20.0f;
    glm::vec3 bias_direction;
    glm::vec3 position;
    glm::vec3 direction;
    glm::mat4 SpotlightSpaceMatrix;
};

struct PointLight_DATA
{
    float lightintensity = 0.0f;
    float near_plane = 0.1f;
    float far_plane = 5.0f;
    glm::vec3 position[POINT_LIGHTS_NUM];
    ModelSpace space[POINT_LIGHTS_NUM];
    glm::vec3 color[POINT_LIGHTS_NUM];
    PointLight_DATA() : lightintensity(0.0f), near_plane(0.1f), far_plane(5.0f),position(),space(), color()
    {};
};
class Game
{
private:
    unsigned int screenWidth = 960;
    unsigned int screenHeight = 640;

public:
    KeyInput keyinput;
    Camera camera = Camera(glm::vec3(0.0f, 1.4f, 3.0f));
    CSM_Dirlight csm_dirlight = CSM_Dirlight(camera, CSM_SPLIT_NUM);
    Shaders* shaders;
    Textures* textures;
    Models* models;
    FrameBuffers* framebuffers;
    VertexArrays* vertex_arrays;
    VertexBuffers* vertex_buffers;
    Renderer renderer;
    Sun_DATA sun;
    SpotLight_DATA spotlight;
    PointLight_DATA pointlight;
    bool Last_CSM_update = true;
    glm::mat4 Last_CSM_update_matrix;

    std::string strConfigFileName = std::string("src/config.ini");
    std::fstream out = std::fstream(strConfigFileName);
    Config config = Config(strConfigFileName);
    Game() :shaders(nullptr), textures(nullptr), models(nullptr), framebuffers(nullptr), vertex_arrays(nullptr), vertex_buffers(nullptr) {};
    ~Game() {
        delete shaders;
        delete textures;
        delete models;
        delete framebuffers;
        delete vertex_arrays;
        delete vertex_buffers;
    }
    void ready_render()
    {
        shaders = new Shaders;
        textures = new Textures;
        models = new Models;
        framebuffers = new FrameBuffers(screenWidth, screenHeight);
        vertex_arrays = new VertexArrays;
        vertex_buffers = new VertexBuffers;
        Debug();
        read_config();
        Update_Pointlight();
        Initialize_Models_Positions();     
        Initialize_Vertex_Arrays();
        TextFBO::Init();
        Generate_CubeTexture(shaders->EnvCubeMapShader, framebuffers, textures, vertex_arrays, renderer);
        Generate_EnvLight_Diffuse(shaders->EnvCubeMap_ConvolutionShader, framebuffers,
            renderer, vertex_arrays, textures);
        Generate_EnvLight_Specular(shaders->EnvCubeMap_spec_ConvolutionShader, framebuffers,
            renderer, vertex_arrays, textures);
        Generate_EnvLight_Specular_BRDF(shaders->EnvCubeMap_spec_BRDF_Shader, framebuffers,
            renderer, vertex_arrays);
        glEnable(GL_BLEND);
        RenderText(shaders->text_shader, "welcome to the demo", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), vertex_arrays, vertex_buffers);
        glDisable(GL_BLEND);
    }
    void start_render()
    {
        camera.Set_third_view(keyinput.third_view);
        glEnable(GL_DEPTH_TEST);
        Update_Models_Positions(models, camera);
        sun = GetSunPosition(camera);
        csm_dirlight.Get_light_projection(camera, sun.Sun_Position);
        Generate_Dir_CSM(shaders->DirLightShadowshader, csm_dirlight,
            framebuffers, models, renderer, vertex_arrays);
        //点光阴影贴图
        Generate_Point_SM(shaders->PointLightShadowshader, framebuffers,
            models, renderer, vertex_arrays, pointlight);

        //聚光阴影贴图
        Update_Spotlight(spotlight, camera);
        Generate_Spot_SM(shaders->SpotLightShadowshader, framebuffers,
            models, renderer, vertex_arrays, spotlight);

        //defered shading 绘制基本信息    
        Generate_Defered_basicDATA(shaders->DeferedShader, camera, framebuffers,
            models, renderer, vertex_arrays, textures);

        framebuffers->gbuffer.Read();
        framebuffers->cameradepthFBO.Write();
        framebuffers->gbuffer.BlitDepthBuffer();
        framebuffers->gbuffer.UnBind();

        if (keyinput.useSSAO)
        {
            Generate_SSAO(shaders->SSAOShader, camera, framebuffers, framebuffers->ssaoKernel,
                renderer, vertex_arrays, keyinput);
            //blur
            Generate_SSAO_blur(shaders->SSAOBlurShader, framebuffers, renderer, vertex_arrays);
        }
        Generate_PreShadow(shaders->DeferedPreShadowShader, camera, csm_dirlight, framebuffers,
            sun, pointlight, spotlight, renderer, vertex_arrays);

        //对preshdow模糊处理
        for (int j = 0; j < 1; j++)
        {
            Gaussian_Blured_Texture(j, 4, shaders->shadow_blurshader, framebuffers->PreShadowFBO, framebuffers->shadow_blur_horizontalFBO, framebuffers->shadow_blur_verticalFBO, renderer, vertex_arrays->quadVa);
            framebuffers->shadow_blur_verticalFBO.Bind();
            framebuffers->shadow_blur_verticalFBO.ReadTexture();
            framebuffers->PreShadowFBO.WriteTexture(j);
            framebuffers->shadow_blur_verticalFBO.UnBind();
        }

        //设置uniform      着色
        Generate_Origin_Screen(shaders->DeferedLighting_shader, keyinput, framebuffers,
            sun, pointlight, spotlight, renderer, vertex_arrays);
        //点光源模型 
        //读取gbuffer的深度信息以结合正向渲染    
        Generate_SkyBox(shaders->basic_shader, shaders->skyboxShader, framebuffers, camera, keyinput,
            vertex_arrays, renderer, textures, models);

        //传入原画
        Generate_PostProcess(shaders->screenShader, shaders->blooming_highlightshader, shaders->blooming_blurshader, shaders->basicscreen_shader,
            framebuffers, renderer, vertex_arrays, keyinput);

        glEnable(GL_BLEND);
        std::string text_render = Collision_detection(models, camera);
        RenderText(shaders->text_shader, text_render, 0.0f, 600.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), vertex_arrays, vertex_buffers);
        glDisable(GL_BLEND);
    }
    void read_config()
    {
        out.open(strConfigFileName, std::ios::app);
        if (out.is_open()) {
            //读取config.ini
            out >> config;
            out.close();
        }
    }
    template <typename T1, typename T2, typename T3>
    void Gaussian_Blured_Texture(int j, unsigned int times, Shader& blooming_blurshader, T1& PreShadowFBO, T2& shadow_blur_horizontalFBO,
        T3& shadow_blur_verticalFBO, Renderer& renderer, VertexArray& quadVa);

    Sun_DATA GetSunPosition(Camera& camera);

    void Update_Spotlight(SpotLight_DATA& spotlight, Camera& camera);

    void Update_Pointlight();

    void Generate_Dir_CSM(Shader& dircsmshader, CSM_Dirlight& csm_dirlight, FrameBuffers* framebuffers,
        Models* models, Renderer& renderer, VertexArrays* vertex_arrays);

    void Generate_Point_SM(D3Shader& Point_sm_shader, FrameBuffers* framebuffers,
        Models* models, Renderer& renderer, VertexArrays* vertex_arrays, PointLight_DATA& pointlight);

    void Generate_Spot_SM(Shader& Spot_sm_shader, FrameBuffers* framebuffers,
        Models* models, Renderer& renderer, VertexArrays* vertex_arrays, SpotLight_DATA& spoltlight);

    void Generate_Defered_basicDATA(Shader& DeferedShader, Camera& camera, FrameBuffers* framebuffers,
        Models* models, Renderer& renderer, VertexArrays* vertex_arrays, Textures* textures);

    void Generate_SSAO(Shader& ssaoshader, Camera& camera, FrameBuffers* framebuffers,
        std::vector<glm::vec3>& ssaoKernel, Renderer& renderer, VertexArrays* vertex_arrays, KeyInput& keyinput);

    void Generate_SSAO_blur(Shader& SSAOBlurShader, FrameBuffers* framebuffers, Renderer& renderer, VertexArrays* vertex_arrays);

    void Generate_PreShadow(Shader& DeferedPreShadowShader, Camera& camera, CSM_Dirlight& csm_dirlight, FrameBuffers* framebuffers,
        Sun_DATA& sun, PointLight_DATA& pointlight, SpotLight_DATA& spotlight, Renderer& renderer, VertexArrays* vertex_arrays);

    void Generate_Origin_Screen(Shader& DeferedLighting_shader, KeyInput& keyinput, FrameBuffers* framebuffers,
        Sun_DATA& sun, PointLight_DATA& pointlight, SpotLight_DATA& spotlight, Renderer& renderer, VertexArrays* vertex_arrays);

    void Generate_SkyBox(Shader& basic_shader, Shader& skyboxShader, FrameBuffers* framebuffers, Camera& camera, KeyInput& keyinput,
        VertexArrays* vertex_arrays, Renderer& renderer, Textures* textures, Models* models);

    void Generate_PostProcess(Shader& screenShader, Shader& blooming_highlightshader, Shader& blooming_blurshader, Shader& basic_shader,
        FrameBuffers* framebuffers, Renderer& renderer, VertexArrays* vertex_arrays, KeyInput& keyinput);

    void Generate_CubeTexture(Shader& EnvCubeMapShader, FrameBuffers* framebuffers, Textures* textures, VertexArrays* vertex_arrays, Renderer& renderer);

    void Generate_EnvLight_Diffuse(Shader& EnvCubeMap_ConvolutionShader, FrameBuffers* framebuffers,
        Renderer& renderer, VertexArrays* vertex_arrays, Textures* textures);

    void Generate_EnvLight_Specular(Shader& EnvCubeMap_spec_ConvolutionShader, FrameBuffers* framebuffers,
        Renderer& renderer, VertexArrays* vertex_arrays, Textures* textures);

    void Generate_EnvLight_Specular_BRDF(Shader& EnvCubeMap_spec_BRDF_Shader, FrameBuffers* framebuffers,
        Renderer& renderer, VertexArrays* vertex_arrays);

    void Initialize_Models_Positions();

    void Update_Models_Positions(Models* models, Camera& camera);

    void GUI_Initialize(GLFWwindow* window);

    void GUI_Process(GLFWwindow* window, KeyInput& keyinput);

    void Initialize_Vertex_Arrays();

    void RenderText(Shader& text_shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, VertexArrays* vertex_arrays,
        VertexBuffers* vertex_buffers);

    void Debug();

    std::string Collision_detection(Models* models, Camera& camera);
};
template <typename T1, typename T2, typename T3>
void Game::Gaussian_Blured_Texture(int j, unsigned int times, Shader& blooming_blurshader, T1& PreShadowFBO, T2& shadow_blur_horizontalFBO,
    T3& shadow_blur_verticalFBO, Renderer& renderer, VertexArray& quadVa)
{
    bool horizontal = true, first_iteration = true;
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    blooming_blurshader.Bind();
    for (unsigned int i = 0; i < times; i++)
    {
        blooming_blurshader.SetUniform1i("horizontal", horizontal);
        if (first_iteration)
        {
            shadow_blur_horizontalFBO.Bind();
            first_iteration = false;
            PreShadowFBO.BindTexture(0, j);
        }
        else
            if (horizontal)
            {
                shadow_blur_horizontalFBO.Bind();
                shadow_blur_verticalFBO.BindTexture();
            }
            else
            {
                shadow_blur_verticalFBO.Bind();
                shadow_blur_horizontalFBO.BindTexture();
            }
        renderer.DrawArray(quadVa, blooming_blurshader);
        horizontal = !horizontal;
    }
    shadow_blur_verticalFBO.UnBind();
}

Sun_DATA Game::GetSunPosition(Camera& camera)
{
    float sun_height = 20;
    float east_x = static_cast<float>(sin(glm::radians(45.0f + glfwGetTime() / 100.0f))) * sun_height;
    float west_y = powf(sun_height * sun_height - powf(east_x, 2), 0.5);
    glm::vec3 SunPosition = glm::vec3(east_x, west_y, 1.0f) + camera.Position;
    glm::vec3 SunDireciton = -glm::vec3(east_x, west_y, 1.0f);
    Sun_DATA sundata = { SunPosition, SunDireciton };
    return sundata;
}
void Game::Generate_Dir_CSM(Shader& dircsmshader, CSM_Dirlight& csm_dirlight, FrameBuffers* framebuffers,
    Models* models, Renderer& renderer, VertexArrays* vertex_arrays)
{
    int update_CSM_SPLIT_NUM = CSM_SPLIT_NUM - 1;//每两帧更新一次3级csm
    if (Last_CSM_update)
    {
        update_CSM_SPLIT_NUM = CSM_SPLIT_NUM;
        Last_CSM_update_matrix = csm_dirlight.light_projection_matrix[3];
    }
    Last_CSM_update = !Last_CSM_update;
    for (int i = 0; i < update_CSM_SPLIT_NUM; i++)
    {
        dircsmshader.Bind();
        dircsmshader.SetUniformmatri4fv("LightSpace", csm_dirlight.light_projection_matrix[i]);

        framebuffers->csm_mapFBO[i].Bind();//帧缓冲存储贴图
        framebuffers->csm_mapFBO[i].SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //绘制       
        std::map<std::string, Model*>::iterator iter;
        iter = models->models_map.begin();
        for (iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
        {
            dircsmshader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
            iter->second->DrawShadow(dircsmshader);
        }
        dircsmshader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
        renderer.DrawArray(*models->Floor.va, dircsmshader);

        dircsmshader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
        renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, dircsmshader);
        framebuffers->csm_mapFBO[i].UnBind();

    }
    glViewport(0, 0, screenWidth, screenHeight);

}
void Game::Generate_Point_SM(D3Shader& Point_sm_shader, FrameBuffers* framebuffers,
    Models* models, Renderer& renderer, VertexArrays* vertex_arrays, PointLight_DATA& pointlight)
{
    glm::mat4 PointlightProjection = glm::perspective(glm::radians(90.0f), 1.0f, pointlight.near_plane, pointlight.far_plane);//aspect应为阴影宽高比
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        std::vector<glm::mat4> shadowTransforms;
        glm::vec3 position = pointlight.position[i];
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));//右
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));//左
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));//上
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));//下
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));//前
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));//后
        Point_sm_shader.Bind();
        for (int face = 0; face < 6; face++)
        {
            Point_sm_shader.SetUniformmatri4fv("shadowMatrices[" + std::to_string(face) + "]", shadowTransforms[face]);
        }
        Point_sm_shader.SetUniform3f("lightPos", position);
        Point_sm_shader.SetUniform1f("far_plane", pointlight.far_plane);
        framebuffers->PointlightMapfbo[i].Bind();//帧缓冲存储贴图
        framebuffers->PointlightMapfbo[i].SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //绘制
        Point_sm_shader.SetUniformmatri4fv("model", models->Nano.position.GetModelSpace());
        models->Nano.DrawShadow(Point_sm_shader);

        Point_sm_shader.SetUniformmatri4fv("model", models->Marry.position.GetModelSpace());
        models->Marry.DrawShadow(Point_sm_shader);

        Point_sm_shader.SetUniformmatri4fv("model", models->Planet.position.GetModelSpace());
        models->Planet.DrawShadow(Point_sm_shader);

        Point_sm_shader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
        renderer.DrawArray(*models->Floor.va, Point_sm_shader);

        Point_sm_shader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
        renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, Point_sm_shader);
        framebuffers->PointlightMapfbo[i].UnBind();
    }
    glViewport(0, 0, screenWidth, screenHeight);

}
void Game::Generate_Spot_SM(Shader& Spot_sm_shader, FrameBuffers* framebuffers,
    Models* models, Renderer& renderer, VertexArrays* vertex_arrays, SpotLight_DATA& spoltlight)
{
    float spotlight_near_plane = 0.1f;
    float spotlight_far_plane = 20.0f;
    glm::mat4 SpotlightProjection = glm::perspective(glm::radians(45.0f), 1.0f, spotlight_near_plane, spotlight_far_plane);
    glm::mat4 SpotlightView = glm::lookAt(spotlight.position, spotlight.position + spotlight.direction, glm::vec3(0.0f, 1.0f, 0.0f));//方向为负的光照方向
    glm::mat4 SpotlightSpaceMatrix = SpotlightProjection * SpotlightView;
    Spot_sm_shader.Bind();
    Spot_sm_shader.SetUniformmatri4fv("LightSpace", SpotlightSpaceMatrix);
    framebuffers->SpotlightMapfbo.Bind();//帧缓冲存储贴图
    framebuffers->SpotlightMapfbo.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //绘制
    Spot_sm_shader.SetUniformmatri4fv("model", models->Nano.position.GetModelSpace());
    models->Nano.DrawShadow(Spot_sm_shader);

    Spot_sm_shader.SetUniformmatri4fv("model", models->Marry.position.GetModelSpace());
    models->Marry.DrawShadow(Spot_sm_shader);

    Spot_sm_shader.SetUniformmatri4fv("model", models->Planet.position.GetModelSpace());
    models->Planet.DrawShadow(Spot_sm_shader);

    Spot_sm_shader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
    renderer.DrawArray(*models->Floor.va, Spot_sm_shader);

    Spot_sm_shader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
    renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, Spot_sm_shader);
    framebuffers->SpotlightMapfbo.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_Defered_basicDATA(Shader& DeferedShader, Camera& camera, FrameBuffers* framebuffers,
    Models* models, Renderer& renderer, VertexArrays* vertex_arrays, Textures* textures)
{
    DeferedShader.Bind();
    DeferedShader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    DeferedShader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    if (camera.third_view)
        DeferedShader.SetUniform3f("viewPos", camera.character_pos - glm::vec3(3.0f) * camera.Front);
    else
        DeferedShader.SetUniform3f("viewPos", camera.Position);
    framebuffers->gbuffer.Bind();
    framebuffers->gbuffer.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DeferedShader.SetUniform1i("use_NormalMap", keyinput.NormalMap);
    DeferedShader.SetUniform1i("use_HeightMap", keyinput.useheight);
    DeferedShader.SetUniformmatri4fv("model", models->Nano.position.GetModelSpace());
    models->Nano.Draw(DeferedShader);
    DeferedShader.SetUniform1i("use_NormalMap", 0);
    DeferedShader.SetUniform1i("use_HeightMap", 0);
    DeferedShader.SetUniformmatri4fv("model", models->Marry.position.GetModelSpace());
    models->Marry.Draw(DeferedShader);
    DeferedShader.SetUniformmatri4fv("model", models->Planet.position.GetModelSpace());
    models->Planet.Draw(DeferedShader);
    DeferedShader.SetUniformmatri4fv("model", models->Main_character.position.GetModelSpace());
    models->Main_character.Draw(DeferedShader);
    //地板  
    DeferedShader.SetUniform1i("use_NormalMap", keyinput.NormalMap);
    DeferedShader.SetUniform1i("use_HeightMap", keyinput.useheight);
    textures->floor_diffuse.Bind(0);
    DeferedShader.SetUniform1i("material.texture_diffuse1", 0);
    textures->floor_specular.Bind(1);
    DeferedShader.SetUniform1i("material.texture_specular1", 1);
    textures->floor_normal.Bind(2);
    DeferedShader.SetUniform1i("material.texture_normal1", 2);
    textures->floor_height.Bind(3);
    DeferedShader.SetUniform1i("material.texture_height1", 3);
    DeferedShader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
    renderer.DrawArray(*models->Floor.va, DeferedShader);
    //球体
    DeferedShader.SetUniform1i("use_NormalMap", 0);
    DeferedShader.SetUniform1i("use_HeightMap", 0);
    DeferedShader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
    renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, DeferedShader);

    framebuffers->gbuffer.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_SSAO(Shader& SSAOShader, Camera& camera, FrameBuffers* framebuffers, std::vector<glm::vec3>& ssaoKernel,
    Renderer& renderer, VertexArrays* vertex_arrays, KeyInput& keyinput)
{
    framebuffers->ssaoFBO.Bind();
    framebuffers->ssaoFBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SSAOShader.Bind();
    SSAOShader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    SSAOShader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    SSAOShader.SetUniform1f("camera.far_plane", camera.far_plane);
    SSAOShader.SetUniform1f("bias", keyinput.SSAO_bias);
    SSAOShader.SetUniform1f("radius", keyinput.SSAO_radius);
    SSAOShader.SetUniform1f("rangecheck", keyinput.SSAO_rangecheck);

    framebuffers->cameradepthFBO.BindTexture(0);
    SSAOShader.SetUniform1i("camera.Depth", 0);
    framebuffers->gbuffer.BindTexture(1, 0);
    SSAOShader.SetUniform1i("gPosition", 1);
    framebuffers->gbuffer.BindTexture(2, 1);
    SSAOShader.SetUniform1i("gNormal", 2);
    framebuffers->ssaoFBO.BindNoiseTexture(3);
    SSAOShader.SetUniform1i("Noise", 3);
    for (int i = 0; i < 64; i++)
    {
        SSAOShader.SetUniform3f("samples[" + std::to_string(i) + "]", framebuffers->ssaoKernel[i]);
    }
    renderer.DrawArray(vertex_arrays->quadVa, SSAOShader);
    framebuffers->ssaoFBO.UnBind();
}
void Game::Generate_SSAO_blur(Shader& SSAOBlurShader, FrameBuffers* framebuffers, Renderer& renderer, VertexArrays* vertex_arrays)
{
    framebuffers->ssaoblurFBO.Bind();
    framebuffers->ssaoblurFBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SSAOBlurShader.Bind();
    framebuffers->ssaoFBO.BindTexture(0);
    SSAOBlurShader.SetUniform1i("ssaoInput", 0);
    renderer.DrawArray(vertex_arrays->quadVa, SSAOBlurShader);
    framebuffers->ssaoblurFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_PreShadow(Shader& DeferedPreShadowShader, Camera& camera, CSM_Dirlight& csm_dirlight, FrameBuffers* framebuffers,
    Sun_DATA& sun, PointLight_DATA& pointlight, SpotLight_DATA& spotlight, Renderer& renderer, VertexArrays* vertex_arrays)
{
    DeferedPreShadowShader.Bind();
    DeferedPreShadowShader.SetUniform3f("camera.viewPos", camera.Position);
    DeferedPreShadowShader.SetUniform1f("camera.far_plane", camera.far_plane);
    DeferedPreShadowShader.SetUniform1f("camera.near_plane", camera.near_plane);
    DeferedPreShadowShader.SetUniform3f("dirlight.direction", sun.Sun_Direction);
    for (int i = 0; i < 4; i++)
    {
        DeferedPreShadowShader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointlight.position[i]);
        DeferedPreShadowShader.SetUniform1f("pointlight[" + std::to_string(i) + "].far_plane", pointlight.far_plane);
    }
    framebuffers->cameradepthFBO.BindTexture(0);
    DeferedPreShadowShader.SetUniform1i("camera.Depth", 0);
    framebuffers->gbuffer.BindTexture(1, 0);
    DeferedPreShadowShader.SetUniform1i("gPosition", 1);
    framebuffers->gbuffer.BindTexture(2, 1);
    DeferedPreShadowShader.SetUniform1i("gNormal", 2);
    framebuffers->gbuffer.BindTexture(3, 2);

    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        framebuffers->PointlightMapfbo[i].BindTexture(i + 4);
        DeferedPreShadowShader.SetUniform1i("shadowMap.PointShadow[" + std::to_string(i) + "]", i + 4);
    }
    for (int i = 0; i < CSM_SPLIT_NUM; i++)
    {
        DeferedPreShadowShader.SetUniform1f("split_distance[" + std::to_string(i) + "]", csm_dirlight.Get_frustum_far(i));
        DeferedPreShadowShader.SetUniform1f("z_distance[" + std::to_string(i) + "]", csm_dirlight.Get_z_distance(i));
        DeferedPreShadowShader.SetUniform1f("xy_distance[" + std::to_string(i) + "]", csm_dirlight.Get_xy_distance(i));
        framebuffers->csm_mapFBO[i].BindTexture(i + 8);
        DeferedPreShadowShader.SetUniform1i("shadowMap.csm_map[" + std::to_string(i) + "]", i + 8);
        if (i == CSM_SPLIT_NUM - 1)
            DeferedPreShadowShader.SetUniformmatri4fv("DirlightCSMMatrix[" + std::to_string(i) + "]", Last_CSM_update_matrix);
        else
            DeferedPreShadowShader.SetUniformmatri4fv("DirlightCSMMatrix[" + std::to_string(i) + "]", csm_dirlight.light_projection_matrix[i]);

    }

    framebuffers->SpotlightMapfbo.BindTexture(12);
    DeferedPreShadowShader.SetUniform1i("shadowMap.SpotShadow", 12);
    DeferedPreShadowShader.SetUniformmatri4fv("SpotlightSpaceMatrix", spotlight.SpotlightSpaceMatrix);

    framebuffers->PreShadowFBO.Bind();//
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays->quadVa, DeferedPreShadowShader);
    glEnable(GL_DEPTH_TEST);
    framebuffers->PreShadowFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_Origin_Screen(Shader& DeferedLighting_shader, KeyInput& keyinput, FrameBuffers* framebuffers,
    Sun_DATA& sun, PointLight_DATA& pointlight, SpotLight_DATA& spotlight, Renderer& renderer, VertexArrays* vertex_arrays)
{
    DeferedLighting_shader.Bind();
    DeferedLighting_shader.SetUniform3f("objectColor", glm::vec3(keyinput.objectColor));
    DeferedLighting_shader.SetUniform1f("material.metallic", keyinput.metallic);
    DeferedLighting_shader.SetUniform1f("material.roughness", keyinput.roughness);
    if (camera.third_view)
        DeferedLighting_shader.SetUniform3f("camera.viewPos", camera.character_pos - glm::vec3(3.0f) * camera.Front);
    else
        DeferedLighting_shader.SetUniform3f("camera.viewPos", camera.Position);
    framebuffers->gbuffer.BindTexture(1, 0);
    DeferedLighting_shader.SetUniform1i("gPosition", 1);
    framebuffers->gbuffer.BindTexture(2, 1);
    DeferedLighting_shader.SetUniform1i("gNormal", 2);
    framebuffers->gbuffer.BindTexture(3, 2);
    DeferedLighting_shader.SetUniform1i("gAlbedoSpec", 3);
    framebuffers->envcubemap_convolutionFBO.BindTexture(4);
    DeferedLighting_shader.SetUniform1i("EnvLight", 4);
    framebuffers->envcubemap_spec_convolutionFBO.BindTexture(5);
    DeferedLighting_shader.SetUniform1i("EnvLight_spec", 5);
    framebuffers->envcubemap_spec_BRDF_FBO.BindTexture(6);
    DeferedLighting_shader.SetUniform1i("EnvLight_spec_BRDF", 6);
    DeferedLighting_shader.SetUniform1i("use_EnvLight_spec", keyinput.EnvLight_spec);

    framebuffers->PreShadowFBO.BindTexture(8, 0);
    DeferedLighting_shader.SetUniform1i("predirshadow", 8);//平行光

    framebuffers->PreShadowFBO.BindTexture(9, 1);
    DeferedLighting_shader.SetUniform1i("prepointshadow", 9);//点光
    framebuffers->PreShadowFBO.BindTexture(10, 2);
    DeferedLighting_shader.SetUniform1i("prespotshadow", 10);//聚光
    DeferedLighting_shader.SetUniform1i("useSSAO", keyinput.useSSAO);
    if (keyinput.useSSAO)
    {
        framebuffers->ssaoblurFBO.BindTexture(13);
        DeferedLighting_shader.SetUniform1i("ssao", 13);
    }

    //平行光
    DeferedLighting_shader.SetUniform3f("dirlight.color", keyinput.SunColor);
    DeferedLighting_shader.SetUniform1f("dirlight.LightIntensity", keyinput.SunIntensity);
    DeferedLighting_shader.SetUniform3f("dirlight.direction", sun.Sun_Direction);
    //点光
    for (int i = 0; i < 4; i++)
    {
        DeferedLighting_shader.SetUniform3f("pointlight[" + std::to_string(i) + "].color", pointlight.color[i] * pointlight.lightintensity);
        DeferedLighting_shader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointlight.position[i]);
        DeferedLighting_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].LightIntensity", pointlight.lightintensity);
        DeferedLighting_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].far_plane", pointlight.far_plane);
        DeferedLighting_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].near_plane", pointlight.near_plane);
    }
    //聚光手电
    DeferedLighting_shader.SetUniform3f("spotlight.color", glm::vec3(30.0f));
    DeferedLighting_shader.SetUniform3f("spotlight.direction", spotlight.direction);
    DeferedLighting_shader.SetUniform3f("spotlight.position", spotlight.position);
    DeferedLighting_shader.SetUniform1f("spotlight.LightIntensity", 1.0f * keyinput.TorchOn);
    DeferedLighting_shader.SetUniform1f("spotlight.inner_CutOff", glm::cos(glm::radians(10.5f)));//spotlight范围
    DeferedLighting_shader.SetUniform1f("spotlight.outer_CutOff", glm::cos(glm::radians(12.5f)));//spotlight范围

    framebuffers->hdrfbo.Bind();//使用非默认帧缓冲来提供HDR
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays->quadVa, DeferedLighting_shader);
    glEnable(GL_DEPTH_TEST);
    framebuffers->hdrfbo.UnBind();
}
void Game::Generate_SkyBox(Shader& basic_shader, Shader& skyboxShader, FrameBuffers* framebuffers, Camera& camera, KeyInput& keyinput,
    VertexArrays* vertex_arrays, Renderer& renderer, Textures* textures, Models* models)
{
    //读取深度信息
    framebuffers->hdrfbo.Bind();
    framebuffers->gbuffer.Read();
    framebuffers->hdrfbo.Write();
    framebuffers->gbuffer.BlitDepthBuffer();

    basic_shader.Bind();
    basic_shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    basic_shader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    for (unsigned int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        basic_shader.SetUniform3f("material.color", pointlight.color[i]);
        basic_shader.SetUniformmatri4fv("model", pointlight.space[i].GetModelSpace());
        renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, basic_shader);
    }
    basic_shader.SetUniform3f("material.color", keyinput.SunColor * keyinput.SunIntensity);
    ModelSpace dirlightspace;//太阳位置会变化，所以动态改变位置       
    dirlightspace.Translate(sun.Sun_Position);
    //dirlightspace.Scale(glm::vec3(0.3f));
    basic_shader.SetUniformmatri4fv("model", dirlightspace.GetModelSpace());
    renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, basic_shader);
    //天空盒       
    skyboxShader.Bind();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));//降维移除第四列的位移
    skyboxShader.SetUniformmatri4fv("view", view);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), camera.near_plane, camera.far_plane);
    skyboxShader.SetUniformmatri4fv("projection", projection);
    textures->skybox.Bind();
    //envcubemapFBO.BindTexture();
    //envcubemap_spec_convolutionFBO.BindTexture();
    renderer.DrawArray(vertex_arrays->skyboxVa, skyboxShader);
    framebuffers->hdrfbo.UnBind();
}
void Game::Generate_PostProcess(Shader& screenShader, Shader& blooming_highlightshader, Shader& blooming_blurshader, Shader& basicscreen_shader,
    FrameBuffers* framebuffers, Renderer& renderer, VertexArrays* vertex_arrays, KeyInput& keyinput)
{
    framebuffers->blooming_hightlightFBO.Bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //将原画分出两份，将原画(ambient + color * shadow * blocker_distance),另一份是高亮highlight
    blooming_highlightshader.Bind();
    framebuffers->hdrfbo.BindTexture(0, 0);;//原画
    blooming_highlightshader.SetUniform1i("screenTexture", 0);
    renderer.DrawArray(vertex_arrays->quadVa, blooming_highlightshader);
    framebuffers->blooming_hightlightFBO.UnBind();
    //将高亮贴图进行高斯模糊
    Gaussian_Blured_Texture(1, 10, blooming_blurshader, framebuffers->blooming_hightlightFBO, framebuffers->blooming_blur_horizontalFBO, framebuffers->blooming_blur_verticalFBO, renderer, vertex_arrays->quadVa);
    //结合原画和模糊后的图片形成泛光
    screenShader.Bind();
    screenShader.SetUniform1i("gamma", keyinput.gamma);
    screenShader.SetUniform1f("exposure", keyinput.exposure);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    framebuffers->blooming_hightlightFBO.BindTexture(0);
    screenShader.SetUniform1i("screenTexture", 0);
    framebuffers->blooming_blur_verticalFBO.BindTexture(1);
    screenShader.SetUniform1i("blooming_screenTexture", 1);
    framebuffers->ssaoFBO.BindTexture(2);
    screenShader.SetUniform1i("assistTexture", 2);
    renderer.DrawArray(vertex_arrays->quadVa, screenShader);
    if (keyinput.assist_screen)
    {
        glViewport(0, 0, int(screenWidth * 0.4), int(screenHeight * 0.4));
        basicscreen_shader.Bind();
        framebuffers->ssaoFBO.BindTexture(0);
        renderer.DrawArray(vertex_arrays->quadVa, basicscreen_shader);
        glViewport(0, 0, screenWidth, screenHeight);
    }

}

void Game::Generate_CubeTexture(Shader& EnvCubeMapShader, FrameBuffers* framebuffers, Textures* textures, VertexArrays* vertex_arrays, Renderer& renderer)
{
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    EnvCubeMapShader.Bind();
    EnvCubeMapShader.SetUniformmatri4fv("projection", captureProjection);
    framebuffers->envcubemapFBO.SetViewPort();
    textures->equirectangularMap.Bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        framebuffers->envcubemapFBO.Bind(i);
        EnvCubeMapShader.SetUniformmatri4fv("view", captureViews[i]);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.DrawArray(vertex_arrays->cubeVa, EnvCubeMapShader);
    }
    framebuffers->envcubemapFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);

}

void Game::Generate_EnvLight_Diffuse(Shader& EnvCubeMap_ConvolutionShader, FrameBuffers* framebuffers,
    Renderer& renderer, VertexArrays* vertex_arrays, Textures* textures)
{
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    EnvCubeMap_ConvolutionShader.Bind();
    EnvCubeMap_ConvolutionShader.SetUniformmatri4fv("projection", captureProjection);
    framebuffers->envcubemap_convolutionFBO.SetViewPort();
    textures->skybox.Bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        framebuffers->envcubemap_convolutionFBO.Bind(i);
        EnvCubeMap_ConvolutionShader.SetUniformmatri4fv("view", captureViews[i]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.DrawArray(vertex_arrays->cubeVa, EnvCubeMap_ConvolutionShader);
    }
    framebuffers->envcubemap_convolutionFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}

void Game::Generate_EnvLight_Specular(Shader& EnvCubeMap_spec_ConvolutionShader, FrameBuffers* framebuffers,
    Renderer& renderer, VertexArrays* vertex_arrays, Textures* textures)
{
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    EnvCubeMap_spec_ConvolutionShader.Bind();
    EnvCubeMap_spec_ConvolutionShader.SetUniformmatri4fv("projection", captureProjection);
    unsigned int maxMipLevels = 5;
    textures->skybox.Bind();
    for (unsigned int i = 0; i < maxMipLevels; ++i)
    {
        unsigned int mipWidth = 256 * static_cast<unsigned int>(std::pow(0.5, i));
        unsigned int mipHeight = 256 * static_cast<unsigned int>(std::pow(0.5, i));
        framebuffers->envcubemap_spec_convolutionFBO.Bindmip_Renderbuffer(mipWidth, mipHeight);
        framebuffers->envcubemap_spec_convolutionFBO.SetViewPort(mipWidth, mipHeight);
        float roughness = (float)i / (float)(maxMipLevels - 1);
        EnvCubeMap_spec_ConvolutionShader.SetUniform1f("roughness", roughness);
        for (int j = 0; j < 6; j++)
        {
            framebuffers->envcubemap_spec_convolutionFBO.Bind(j, i);
            EnvCubeMap_spec_ConvolutionShader.SetUniformmatri4fv("view", captureViews[j]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderer.DrawArray(vertex_arrays->cubeVa, EnvCubeMap_spec_ConvolutionShader);
        }
    }
    textures->skybox.UnBind();
    framebuffers->envcubemap_spec_convolutionFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}

void Game::Generate_EnvLight_Specular_BRDF(Shader& EnvCubeMap_spec_BRDF_Shader, FrameBuffers* framebuffers,
    Renderer& renderer, VertexArrays* vertex_arrays)
{
    EnvCubeMap_spec_BRDF_Shader.Bind();
    framebuffers->envcubemap_spec_BRDF_FBO.Bind();
    framebuffers->envcubemap_spec_BRDF_FBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays->quadVa, EnvCubeMap_spec_BRDF_Shader);
    glEnable(GL_DEPTH_TEST);
    framebuffers->envcubemap_spec_BRDF_FBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}

void Game::Initialize_Models_Positions()
{
    models->Nano.position.Scale(glm::vec3(0.1f));
    models->Nano.position.Translate(glm::vec3(0.0f, 1.0f, 0.0f));
    models->Nano.position.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    models->Marry.position.Translate(glm::vec3(5.0f, 0.0f, 0.0f));
    models->Marry.position.Scale(0.5f);
    models->Planet.position.Translate(glm::vec3(10.0f, 0, 0));
    models->Floor.position.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    models->Sphere.position.Translate(glm::vec3(-2.0f, 1.0f, 0.0));
    models->Main_character.position.Scale(glm::vec3(0.1f));
    models->Main_character.position.Translate(camera.Position);
    models->Main_character.position.Rotate(-180.0, glm::vec3(0.0, 1.0, 0.0));
    std::map<std::string, Model*>::iterator iter;
    iter = models->models_map.begin();
    for (iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        iter->second->Get_AABB();
    }
}

void Game::Update_Models_Positions(Models* models, Camera& camera)
{
    glm::mat4 mod;
    float theta = acos(glm::dot(glm::normalize(glm::vec3(camera.Front.x, 0.0, camera.Front.z)), glm::vec3(0.0f, 0.0f, -1.0f)));
    theta = camera.Front.x > 0 ? theta : static_cast<float>((2 * PI - theta));
    float character_view = static_cast<float>(theta * 180.0f / PI);
    ModelSpace modd;
    modd.Scale(glm::vec3(0.1f));
    modd.Rotate(180.0f + character_view, glm::vec3(0.0, -1.0, 0.0));
    if (!camera.third_view)
    {
        mod = models->Main_character.position.GetModelSpace();
    }
    else
    {
        float rotation = 0;
        switch (camera.character_Front)
        {
        case FORWARD:
            rotation = 0;
            break;
        case LEFT:
            rotation = -90.0f;
            break;
        case RIGHT:
            rotation = 90.0f;
            break;
        case BACKWARD:
            rotation = 180.0f;
            break;
        case FORWARD_LEFT:
            rotation = -45.0f;
            break;
        case FORWARD_RIGHT:
            rotation = 45.0f;
            break;
        case BACKWARD_LEFT:
            rotation = 225.0f;
            break;
        case BACKWARD_RIGHT:
            rotation = 135.0f;
            break;
        }
        if (camera.is_move)
        {
            modd.Rotate(rotation, glm::vec3(0.0, -1.0, 0.0));
            mod = modd.GetModelSpace();
        }
        else
            mod = models->Main_character.position.GetModelSpace();
    }
    mod[3][0] = camera.character_pos.x;
    mod[3][1] = 0.0;
    mod[3][2] = camera.character_pos.z;
    models->Main_character.position.SetModel(mod);
    models->Main_character.Get_AABB();
}

void Game::GUI_Initialize(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void Game::Initialize_Vertex_Arrays()
{
    std::string floor_vertex_attr = "floor_vertex_attr";
    std::vector<float> floor_vertex = config.ReadVector(floor_vertex_attr);
    VertexBuffer floorVb(&floor_vertex[0], static_cast<unsigned int>(floor_vertex.size()) * sizeof(float));
    VertexBufferLayout floorLayout;
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(2);
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(3);
    models->Floor.va->AddBuffer(floorVb, floorLayout);
    //sphere
    sphere_data sphere = SphereData();
    IndexBuffer sphereIb(&sphere.index[0], sphere.index.size());
    VertexBuffer sphereVb(&sphere.vertex[0], sizeof(float) * static_cast<unsigned int>(sphere.vertex.size()));
    VertexBufferLayout sphereLayout;
    sphereLayout.Push<float>(3);//position
    sphereLayout.Push<float>(3);//normal
    sphereLayout.Push<float>(2);//texcoord
    models->Sphere.va->AddBuffer(sphereVb, sphereLayout);
    //quad
    std::string quad_vertex_attr = "quad_vertex_attr";
    std::vector<float> quad_vertex = config.ReadVector(quad_vertex_attr);
    VertexBuffer quadVb(&quad_vertex[0], static_cast<unsigned int>(quad_vertex.size()) * sizeof(float));
    VertexBufferLayout quadLayout;
    quadLayout.Push<float>(3);
    quadLayout.Push<float>(2);
    vertex_arrays->quadVa.AddBuffer(quadVb, quadLayout);
    //天空盒
    std::string skybox_vertex_attr = "skybox_vertex_attr";
    std::vector<float> skybox_vertex = config.ReadVector(skybox_vertex_attr);
    VertexBuffer skyboxVb(&skybox_vertex[0], static_cast<unsigned int>(skybox_vertex.size()) * sizeof(float));
    VertexBufferLayout skyboxLayout;
    skyboxLayout.Push<float>(3);
    vertex_arrays->skyboxVa.AddBuffer(skyboxVb, skyboxLayout);
    //点光源属性
    std::string cube_vertex_attr = "cube_vertex_attr";
    std::vector<float> cube_vertex = config.ReadVector(cube_vertex_attr);
    VertexBuffer cubeVb(&cube_vertex[0], static_cast<unsigned int>(cube_vertex.size()) * sizeof(float));
    VertexBufferLayout cubeLayout;
    cubeLayout.Push<float>(3);
    cubeLayout.Push<float>(3);
    cubeLayout.Push<float>(2);
    vertex_arrays->cubeVa.AddBuffer(cubeVb, cubeLayout);
    //textVa
    VertexBufferLayout textLayout;
    textLayout.Push<float>(2);
    textLayout.Push<float>(2);
    vertex_arrays->textVa.AddBuffer(vertex_buffers->textVb, textLayout);

}

void Game::RenderText(Shader& text_shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, VertexArrays* vertex_arrays,
    VertexBuffers* vertex_buffers)
{
    // 激活对应的渲染状态
    text_shader.Bind();
    text_shader.SetUniform3f("textColor", color);
    glm::mat4 projection = glm::ortho(0.0f, float(screenWidth), 0.0f, float(screenHeight));
    text_shader.SetUniformmatri4fv("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    vertex_arrays->textVa.Bind();
    // 遍历文本中所有的字符
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // 对每个字符更新VBO
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // 在四边形上绘制字形纹理
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // 更新VBO内存的内容
        vertex_buffers->textVb.Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        vertex_buffers->textVb.Unbind();
        // 绘制四边形
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glCheckError();
        // 更新位置到下一个字形的原点，注意单位是1/64像素
        x += (ch.Advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Game::Debug()
{
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0,
        GL_DEBUG_SEVERITY_MEDIUM, -1, "error message here");

}

std::string Game::Collision_detection(Models* models, Camera& camera)
{
    std::string text("");
    bool x_collision;
    bool y_collision;
    bool z_collision;

    std::map<std::string, Model*>::iterator iter;
    iter = models->models_map.begin();
    for (iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        x_collision = (camera.Position.x < iter->second->aabb[1] && camera.Position.x > iter->second->aabb[0]);
        y_collision = (camera.Position.y < iter->second->aabb[3] && camera.Position.y > iter->second->aabb[2]);
        z_collision = (camera.Position.z < iter->second->aabb[5] && camera.Position.z > iter->second->aabb[4]);
        if (x_collision && y_collision && z_collision)
        {
            text += std::string(iter->first);
        }
    }
    return text;
}

void Game::Update_Spotlight(SpotLight_DATA& spotlight, Camera& camera)
{
    spotlight.bias_direction = glm::vec3(0.15f) * (glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f)) - camera.Front);
    spotlight.position = camera.Position + spotlight.bias_direction;
    spotlight.direction = camera.Front - spotlight.bias_direction;
    glm::mat4 SpotlightProjection = glm::perspective(glm::radians(45.0f), 1.0f, spotlight.spotlight_near_plane, spotlight.spotlight_far_plane);
    glm::mat4 SpotlightView = glm::lookAt(spotlight.position, spotlight.position + spotlight.direction, glm::vec3(0.0f, 1.0f, 0.0f));//方向为负的光照方向
    spotlight.SpotlightSpaceMatrix = SpotlightProjection * SpotlightView;
}

void Game::Update_Pointlight()
{
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointlight.position[i] = glm::vec3(i * 3, 2, 0);
        pointlight.space[i].Translate(pointlight.position[i]);
        pointlight.space[i].Scale(glm::vec3(0.3f));
    }
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointlight.color[i] = glm::vec3(50.0f);
    }
}