#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <random>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <functional>

#include <irrKlang.h>
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
#include "Particle.h"
#include "Animator.h"
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
    float lightintensity;
    float near_plane;
    float far_plane;
    glm::vec3 position[POINT_LIGHTS_NUM];
    ModelSpace space[POINT_LIGHTS_NUM];
    glm::vec3 color[POINT_LIGHTS_NUM];
    PointLight_DATA() : lightintensity(2.0f), near_plane(0.1f), far_plane(25.0f),position(),space(), color()
    {};
};
struct Character_state
{
    float max_life;
    float current_life;
    float max_magic;
    float current_magic;
    float max_energy;
    float current_energy;
    Character_state() :max_life(200.0f), current_life(150.0f) ,
        max_magic(100.0f), current_magic(100.0f),
        max_energy(100.0f), current_energy(100.0f)
    {}
};
class Game
{
private:
    unsigned int screenWidth = 960;
    unsigned int screenHeight = 640;
    TerrainData terrain;
    float deltaTime;
    Animator* animator;
    Animations* animations;
public:
    Character_state my_state;
    KeyInput keyinput;
    Camera camera = Camera(glm::vec3(0.0f, 1.4f, 3.0f));
    CSM_Dirlight csm_dirlight = CSM_Dirlight(camera, CSM_SPLIT_NUM);
    Shaders* shaders;
    Textures* textures;
    Models* models;
    Pre_FrameBuffers* pre_framebuffers;
    FrameBuffers* framebuffers;
    VertexArrays* vertex_arrays;
    VertexBuffers* vertex_buffers;
    Renderer renderer;
    Sun_DATA sun;
    SpotLight_DATA spotlight;
    PointLight_DATA pointlight;
    bool Last_CSM_update = true;
    glm::mat4 Last_CSM_update_matrix;
    Particle_Generator particle_generator = Particle_Generator(500, 1, 0.002f);
    D3Particle_Generator d3particle_generator = D3Particle_Generator(500, 1, 0.002f);
    std::string strConfigFileName = std::string("src/config.ini");
    std::fstream out = std::fstream(strConfigFileName);
    Config config = Config(strConfigFileName);
    Game() :shaders(nullptr), textures(nullptr), models(nullptr), framebuffers(nullptr), pre_framebuffers(nullptr), vertex_arrays(nullptr), vertex_buffers(nullptr) {};
    ~Game() {
        delete shaders;
        delete textures;
        delete models;
        delete framebuffers;
        delete pre_framebuffers;
        delete vertex_arrays;
        delete vertex_buffers;
        delete animator;
        delete animations;
    }


    inline void read_config()
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
    inline void GetDeltaTime(float t)
    {
        deltaTime = t;
    }
    void ready_render();

    void start_render();

    inline unsigned int GetSwidth() { return screenWidth; }
    inline void SetSwidth(unsigned int sw) { screenWidth = sw; }
    inline unsigned int GetSheight() { return screenHeight; }
    inline void SetSheight(unsigned int sh) { screenHeight = sh; }

    void Update_Sun();

    void Update_Spotlight();

    void Initialize_Pointlight();
    void Initialize_Vertex_Arrays();
    void Initialize_Models_Positions();
    void Initialize_Terrain_cpu();
    void Initialize_Terrain_gpu();
    void Update_Pointlight();

    void Generate_Dir_CSM();

    void Generate_Point_SM();

    void Generate_Spot_SM();

    void Generate_Defered_basicDATA();

    void Generate_SSAO();

    void Generate_SSAO_blur();

    void Generate_PreShadow();

    void Generate_Origin_Screen();

    void Generate_SkyBox();

    void Generate_PostProcess();

    void Generate_CubeTexture();

    void Generate_EnvLight_Diffuse();

    void Generate_EnvLight_Specular();

    void Generate_EnvLight_Specular_BRDF();

    void Generate_Terrain_cpu();
    void Generate_Terrain_gpu();

    void Update_Models_Positions();


    void RenderText(Shader& text_shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, VertexArrays* vertex_arrays,
        VertexBuffers* vertex_buffers);

    void Debug();

    void Generate_Particle();
    void Generate_D3Particle();
    void Generate_Health_bar();
    void Generate_Health_bar_enemy();
    std::string Collision_detection();
    inline void ResetResolution(unsigned int width, unsigned int height)
    {
        delete framebuffers;
        framebuffers = new FrameBuffers(width, height);
    }
};
template <typename T1, typename T2, typename T3>
void Game::Gaussian_Blured_Texture(int j, unsigned int times, Shader& blooming_blurshader, T1& PreShadowFBO, T2& shadow_blur_horizontalFBO,
    T3& shadow_blur_verticalFBO, Renderer& renderer, VertexArray& quadVa)
{
    bool horizontal = true, first_iteration = true;
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    blooming_blurshader.Bind();
    blooming_blurshader.SetUniform1f("halox", keyinput.bloom_halox);
    blooming_blurshader.SetUniform1f("haloy", keyinput.bloom_haloy);
    blooming_blurshader.SetUniform1f("haloz", keyinput.bloom_haloz);
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

void Game::Update_Sun()
{
    float sun_height = 20;
    float east_x = static_cast<float>(sin(glfwGetTime()* keyinput.sun_speed)) * sun_height;
    float west_y = static_cast<float>(cos(glfwGetTime() * keyinput.sun_speed)) * sun_height;
    sun.Sun_Position = glm::vec3(east_x, west_y, -10.0f) + camera.Position;
    sun.Sun_Direction = -glm::vec3(east_x, west_y, -10.0f);
}

void Game::Update_Pointlight()
{
    pointlight.lightintensity = keyinput.pointlight_Intensity;
    pointlight.far_plane = keyinput.point_far_plane;
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
        pointlight.color[i] = keyinput.point_color;

}
void Game::Initialize_Pointlight()
{
    
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointlight.position[i] = glm::vec3(i * 3, 2, 0);
        pointlight.space[i].Translate(pointlight.position[i]);
        pointlight.space[i].Scale(glm::vec3(0.3f));
    }
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointlight.color[i] = glm::vec3(1.0f);
    }
    pointlight.lightintensity = keyinput.pointlight_Intensity;
    
}
void Game::Initialize_Models_Positions()
{
    
    models->Nano.position.Scale(glm::vec3(0.1f));
    models->Nano.position.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    models->Nano.position.Translate(glm::vec3(0.0f, 1.0f, 0.0f));
    models->Marry.position.Scale(0.5f);
    models->Marry.position.Translate(glm::vec3(5.0f, 0.0f, 0.0f));

    models->Planet.position.Translate(glm::vec3(10.0f, 0, 0));
    models->Floor.position.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    models->Sphere.position.Translate(glm::vec3(-2.0f, 1.0f, 0.0));

    models->Main_character.position.Rotate(-180.0, glm::vec3(0.0, 1.0, 0.0));
    models->Main_character.position.Translate(camera.Position);

    std::map<std::string, Model*>::iterator iter;
    iter = models->models_map.begin();
    int i = 0;
    for (iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
    {
        i++;
        iter->second->position.Translate(glm::vec3(0.0f, 0.0f, i * 5.0f));
        iter->second->Get_AABB();
    }
    for (iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        iter->second->Get_AABB();
    }
    
}

void Game::Update_Models_Positions()
{
    glm::mat4 mod;
    float theta = acos(glm::dot(glm::normalize(glm::vec3(camera.Front.x, 0.0, camera.Front.z)), glm::vec3(0.0f, 0.0f, -1.0f)));
    theta = camera.Front.x > 0 ? theta : static_cast<float>((2 * PI - theta));
    float character_view = static_cast<float>(theta * 180.0f / PI);
    ModelSpace modd;
    modd.Rotate(180.0f + character_view, glm::vec3(0.0, -1.0, 0.0));
    if (!camera.third_view)
    {
        mod = modd.GetModelSpace();
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
    mod[3][0] = camera.Position.x;
    mod[3][1] = camera.Position.y - 1.6f;
    mod[3][2] = camera.Position.z;
    models->Main_character.position.SetModel(mod);
    models->Main_character.Get_AABB();
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
    models->Sphere.Get_index(&sphere.index[0], static_cast<unsigned int>(sphere.index.size()));
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
    //cube
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
    //particleVa
    std::string particle_vertex_attr = "particle_vertex_attr";
    std::vector<float> particle_vertex = config.ReadVector(particle_vertex_attr);
    VertexBuffer particleVb(&particle_vertex[0], static_cast<unsigned int>(particle_vertex.size()) * sizeof(float));
    VertexBufferLayout particleLayout;
    particleLayout.Push<float>(2);
    particleLayout.Push<float>(2);
    vertex_arrays->particleVa.AddBuffer(particleVb, particleLayout);
    
}
void RootAnimation(glm::mat4& m1, glm::mat4& m2, float grade)
{
    m1[3][0] -= m2[3][0] * grade;
    m1[3][1] -= m2[3][1] * grade;
    m1[3][2] -= m2[3][2];
}
void Game::Generate_Dir_CSM()
{
    int update_CSM_SPLIT_NUM = CSM_SPLIT_NUM - 1;//每两帧更新一次3级csm
    if (Last_CSM_update)
    {
        update_CSM_SPLIT_NUM = CSM_SPLIT_NUM;
        Last_CSM_update_matrix = csm_dirlight.light_projection_matrix[3];
    }
    Last_CSM_update = !Last_CSM_update;
    Shader& shader = shaders->DirLightShadowshader;
    for (int i = 0; i < update_CSM_SPLIT_NUM; i++)
    {
        shader.Bind();
        shader.SetUniformmatri4fv("LightSpace", csm_dirlight.light_projection_matrix[i]);

        framebuffers->csm_mapFBO[i].Bind();//帧缓冲存储贴图
        framebuffers->csm_mapFBO[i].SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //绘制       
        std::map<std::string, Model*>::iterator iter;
        iter = models->models_map.begin();
        for (iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
        {
            shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
            iter->second->DrawShadow(shader);           
        }
        for (iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
        {
            auto transforms = animator->GetFinalBoneMatrices();
            auto roottrans = animator->RootTransform;
            for (int i = 0; i < transforms.size(); ++i)
            {
                RootAnimation(transforms[i], roottrans, 0.5);
                shader.SetUniformmatri4fv("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
            }
            shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
            iter->second->DrawShadow(shader);
        }
        shader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
        renderer.DrawArray(*models->Floor.va, shader);

        shader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
        renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
        framebuffers->csm_mapFBO[i].UnBind();

    }
    glViewport(0, 0, screenWidth, screenHeight);

}
void Game::Generate_Point_SM()
{
    Shader& shader = shaders->PointLightShadowshader;
    shader.Bind();
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
        
        for (int face = 0; face < 6; face++)
        {
            shader.SetUniformmatri4fv("shadowMatrices[" + std::to_string(face) + "]", shadowTransforms[face]);
        }
        shader.SetUniform3f("lightPos", position);
        shader.SetUniform1f("far_plane", pointlight.far_plane);
        framebuffers->PointlightMapfbo[i].Bind();//帧缓冲存储贴图
        framebuffers->PointlightMapfbo[i].SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //绘制
        auto transforms = animator->GetFinalBoneMatrices();
        auto roottrans = animator->RootTransform;
        for (int i = 0; i < transforms.size(); ++i)
        {
            RootAnimation(transforms[i], roottrans, 0.5);
            shader.SetUniformmatri4fv("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }
        std::map<std::string, Model*>::iterator iter;
        iter = models->models_map.begin();
        for (iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
        {
            shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
            iter->second->DrawShadow(shader);
        }
        for (iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
        {
            shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
            iter->second->DrawShadow(shader);
        }
        shader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
        renderer.DrawArray(*models->Floor.va, shader);

        shader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
        renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
        framebuffers->PointlightMapfbo[i].UnBind();
    }
    glViewport(0, 0, screenWidth, screenHeight);

}
void Game::Generate_Spot_SM()
{
    Shader& shader = shaders->SpotLightShadowshader;
    shader.Bind();
    shader.SetUniformmatri4fv("LightSpace", spotlight.SpotlightSpaceMatrix);
    framebuffers->SpotlightMapfbo.Bind();//帧缓冲存储贴图
    framebuffers->SpotlightMapfbo.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //绘制

    std::map<std::string, Model*>::iterator iter;
    iter = models->models_map.begin();
    for (iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
        iter->second->DrawShadow(shader);
    }
    for (iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
    {
        auto transforms = animator->GetFinalBoneMatrices();
        auto roottrans = animator->RootTransform;
        for (int i = 0; i < transforms.size(); ++i)
        {
            RootAnimation(transforms[i], roottrans, 0.5);
            shader.SetUniformmatri4fv("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }
        shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
        iter->second->DrawShadow(shader);
    }
    shader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
    renderer.DrawArray(*models->Floor.va, shader);

    shader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
    renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
    framebuffers->SpotlightMapfbo.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_Defered_basicDATA()
{
    if(keyinput.show_mesh)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    Shader& shader = shaders->DeferedShader;
    shader.Bind();
    shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    shader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    if (camera.third_view)
        shader.SetUniform3f("viewPos", camera.Position - glm::vec3(3.0f) * camera.Front);
    else
        shader.SetUniform3f("viewPos", camera.Position);
    framebuffers->gbuffer.Bind();
    framebuffers->gbuffer.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.SetUniform1i("use_NormalMap", keyinput.NormalMap);
    shader.SetUniform1i("use_HeightMap", keyinput.useheight);
    auto transforms = animator->GetFinalBoneMatrices();
    auto roottrans = animator->RootTransform;
    for (int i = 0; i < transforms.size(); ++i)
    {
        RootAnimation(transforms[i], roottrans, 0.5);
        shader.SetUniformmatri4fv("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    }
    std::map<std::string, Model*>::iterator iter;
    iter = models->models_map.begin();
    for (iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
        iter->second->DrawShadow(shader);
    }
    for (iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
    {
        shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
        iter->second->DrawShadow(shader);
    }
    //地板  
    textures->floor_diffuse.Bind(0);
    shader.SetUniform1i("material.texture_diffuse1", 0);
    textures->floor_specular.Bind(1);
    shader.SetUniform1i("material.texture_specular1", 1);
    textures->floor_normal.Bind(2);
    shader.SetUniform1i("material.texture_normal1", 2);
    textures->floor_height.Bind(3);
    shader.SetUniform1i("material.texture_height1", 3);
    shader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
    renderer.DrawArray(*models->Floor.va, shader);
    //球体
    shader.SetUniform1i("use_NormalMap", 0);
    shader.SetUniform1i("use_HeightMap", 0);
    shader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
    renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
    if (keyinput.use_terrain)
    {
        Generate_Terrain_gpu();
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    framebuffers->gbuffer.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_SSAO()
{
    framebuffers->ssaoFBO.Bind();
    framebuffers->ssaoFBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaders->SSAOShader.Bind();
    shaders->SSAOShader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    shaders->SSAOShader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    shaders->SSAOShader.SetUniform1f("camera.far_plane", camera.far_plane);
    shaders->SSAOShader.SetUniform1f("bias", keyinput.SSAO_bias);
    shaders->SSAOShader.SetUniform1f("radius", keyinput.SSAO_radius);
    shaders->SSAOShader.SetUniform1f("rangecheck", keyinput.SSAO_rangecheck);

    framebuffers->cameradepthFBO.BindTexture(0);
    shaders->SSAOShader.SetUniform1i("camera.Depth", 0);
    framebuffers->gbuffer.BindTexture(1, 0);
    shaders->SSAOShader.SetUniform1i("gPosition", 1);
    framebuffers->gbuffer.BindTexture(2, 1);
    shaders->SSAOShader.SetUniform1i("gNormal", 2);
    framebuffers->ssaoFBO.BindNoiseTexture(3);
    shaders->SSAOShader.SetUniform1i("Noise", 3);
    for (int i = 0; i < 64; i++)
    {
        shaders->SSAOShader.SetUniform3f("samples[" + std::to_string(i) + "]", framebuffers->ssaoFBO.ssao_data.ssaokernel[i]);
    }
    renderer.DrawArray(vertex_arrays->quadVa, shaders->SSAOShader);
    framebuffers->ssaoFBO.UnBind();
}
void Game::Generate_SSAO_blur()
{
    framebuffers->ssaoblurFBO.Bind();
    framebuffers->ssaoblurFBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaders->SSAOBlurShader.Bind();
    framebuffers->ssaoFBO.BindTexture(0);
    shaders->SSAOBlurShader.SetUniform1i("ssaoInput", 0);
    renderer.DrawArray(vertex_arrays->quadVa, shaders->SSAOBlurShader);
    framebuffers->ssaoblurFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_PreShadow()
{
    Shader& shader = shaders->DeferedPreShadowShader;
    shader.Bind();
    shader.SetUniform3f("camera.viewPos", camera.Position);
    shader.SetUniform1f("camera.far_plane", camera.far_plane);
    shader.SetUniform1f("camera.near_plane", camera.near_plane);
    shader.SetUniform1f("sun_sm_bias", keyinput.sun_sm_bias);
    shader.SetUniform1f("point_sm_radius", keyinput.point_sm_radius);
    shader.SetUniform1f("point_sm_pcf", keyinput.point_sm_pcf);
    shader.SetUniform1i("sun_pcf", keyinput.sun_pcf);
    shader.SetUniform1f("sun_pcf_radius", keyinput.sun_pcf_radius);
    shader.SetUniform3f("dirlight.direction", sun.Sun_Direction);
    for (int i = 0; i < 4; i++)
    {
        shader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointlight.position[i]);
        shader.SetUniform1f("pointlight[" + std::to_string(i) + "].far_plane", pointlight.far_plane);
    }
    framebuffers->cameradepthFBO.BindTexture(0);
    shader.SetUniform1i("camera.Depth", 0);
    framebuffers->gbuffer.BindTexture(1, 0);
    shader.SetUniform1i("gPosition", 1);
    framebuffers->gbuffer.BindTexture(2, 1);
    shader.SetUniform1i("gNormal", 2);
    framebuffers->gbuffer.BindTexture(3, 2);

    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        framebuffers->PointlightMapfbo[i].BindTexture(i + 4);
        shader.SetUniform1i("shadowMap.PointShadow[" + std::to_string(i) + "]", i + 4);
    }
    for (int i = 0; i < CSM_SPLIT_NUM; i++)
    {
        shader.SetUniform1f("split_distance[" + std::to_string(i) + "]", csm_dirlight.Get_frustum_far(i));
        shader.SetUniform1f("z_distance[" + std::to_string(i) + "]", csm_dirlight.Get_z_distance(i));
        shader.SetUniform1f("xy_distance[" + std::to_string(i) + "]", csm_dirlight.Get_xy_distance(i));
        framebuffers->csm_mapFBO[i].BindTexture(i + 8);
        shader.SetUniform1i("shadowMap.csm_map[" + std::to_string(i) + "]", i + 8);
        if (i == CSM_SPLIT_NUM - 1)
            shader.SetUniformmatri4fv("DirlightCSMMatrix[" + std::to_string(i) + "]", Last_CSM_update_matrix);
        else
            shader.SetUniformmatri4fv("DirlightCSMMatrix[" + std::to_string(i) + "]", csm_dirlight.light_projection_matrix[i]);

    }

    framebuffers->SpotlightMapfbo.BindTexture(12);
    shader.SetUniform1i("shadowMap.SpotShadow", 12);
    shader.SetUniformmatri4fv("SpotlightSpaceMatrix", spotlight.SpotlightSpaceMatrix);

    framebuffers->PreShadowFBO.Bind();//
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays->quadVa, shader);
    glEnable(GL_DEPTH_TEST);
    framebuffers->PreShadowFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_Origin_Screen()
{
    Shader& shader = shaders->DeferedLighting_shader;
    shader.Bind();
    shader.SetUniform1f("material.metallic", keyinput.metallic);
    shader.SetUniform1f("material.roughness", keyinput.roughness);
    if (camera.third_view)
        shader.SetUniform3f("camera.viewPos", camera.Position - glm::vec3(3.0f) * camera.Front);
    else
        shader.SetUniform3f("camera.viewPos", camera.Position);
    framebuffers->gbuffer.BindTexture(1, 0);
    shader.SetUniform1i("gPosition", 1);
    framebuffers->gbuffer.BindTexture(2, 1);
    shader.SetUniform1i("gNormal", 2);
    framebuffers->gbuffer.BindTexture(3, 2);
    shader.SetUniform1i("gAlbedoSpec", 3);
    pre_framebuffers->envcubemap_convolutionFBO.BindTexture(4);
    shader.SetUniform1i("EnvLight", 4);
    pre_framebuffers->envcubemap_spec_convolutionFBO.BindTexture(5);
    shader.SetUniform1i("EnvLight_spec", 5);
    pre_framebuffers->envcubemap_spec_BRDF_FBO.BindTexture(6);
    shader.SetUniform1i("EnvLight_spec_BRDF", 6);
    shader.SetUniform1i("use_EnvLight_spec", keyinput.EnvLight_spec);

    framebuffers->PreShadowFBO.BindTexture(8, 0);
    shader.SetUniform1i("predirshadow", 8);//平行光

    framebuffers->PreShadowFBO.BindTexture(9, 1);
    shader.SetUniform1i("prepointshadow", 9);//点光
    framebuffers->PreShadowFBO.BindTexture(10, 2);
    shader.SetUniform1i("prespotshadow", 10);//聚光
    shader.SetUniform1i("useSSAO", keyinput.useSSAO);
    if (keyinput.useSSAO)
    {
        framebuffers->ssaoblurFBO.BindTexture(13);
        shader.SetUniform1i("ssao", 13);
    }

    //平行光
    shader.SetUniform3f("dirlight.color", keyinput.SunColor * keyinput.SunIntensity);
    shader.SetUniform1f("dirlight.LightIntensity", keyinput.SunIntensity);
    shader.SetUniform3f("dirlight.direction", sun.Sun_Direction);
    //点光
    for (int i = 0; i < 4; i++)
    {
        shader.SetUniform3f("pointlight[" + std::to_string(i) + "].color", pointlight.color[i] * pointlight.lightintensity);
        shader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointlight.position[i]);
        shader.SetUniform1f("pointlight[" + std::to_string(i) + "].LightIntensity", pointlight.lightintensity);
        shader.SetUniform1f("pointlight[" + std::to_string(i) + "].far_plane", pointlight.far_plane);
        shader.SetUniform1f("pointlight[" + std::to_string(i) + "].near_plane", pointlight.near_plane);
    }
    //聚光手电
    shader.SetUniform3f("spotlight.color", keyinput.torch_color * (keyinput.torch_intensity * keyinput.TorchOn));
    shader.SetUniform3f("spotlight.direction", spotlight.direction);
    shader.SetUniform3f("spotlight.position", spotlight.position);
    shader.SetUniform1f("spotlight.LightIntensity", keyinput.torch_intensity * keyinput.TorchOn);
    shader.SetUniform1f("spotlight.inner_CutOff", glm::cos(glm::radians(10.5f)));//spotlight范围
    shader.SetUniform1f("spotlight.outer_CutOff", glm::cos(glm::radians(12.5f)));//spotlight范围

    framebuffers->hdrfbo.Bind();//使用非默认帧缓冲来提供HDR
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays->quadVa, shader);
    glEnable(GL_DEPTH_TEST);
    framebuffers->hdrfbo.UnBind();
}
void Game::Generate_SkyBox()
{
    //读取深度信息
    if (keyinput.show_mesh)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    framebuffers->hdrfbo.Bind();
    framebuffers->gbuffer.Read();
    framebuffers->hdrfbo.Write();
    framebuffers->gbuffer.BlitDepthBuffer();
    Shader& shader = shaders->basic_shader;
    shader.Bind();
    shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    shader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    for (unsigned int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        shader.SetUniform3f("color", pointlight.color[i] * pointlight.lightintensity);
        shader.SetUniformmatri4fv("model", pointlight.space[i].GetModelSpace());
        renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
    }
    shader.SetUniform3f("color", keyinput.SunColor * keyinput.SunIntensity * glm::vec3(10));
    ModelSpace dirlightspace;//太阳位置会变化，所以动态改变位置       
    dirlightspace.Translate(sun.Sun_Position);
    //dirlightspace.Scale(glm::vec3(0.3f));
    shader.SetUniformmatri4fv("model", dirlightspace.GetModelSpace());
    renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
    //天空盒       
    shaders->skyboxShader.Bind();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));//降维移除第四列的位移
    shaders->skyboxShader.SetUniformmatri4fv("view", view);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), camera.near_plane, camera.far_plane);
    shaders->skyboxShader.SetUniformmatri4fv("projection", projection);
    textures->skybox.Bind();
    //envcubemapFBO.BindTexture();
    //envcubemap_spec_convolutionFBO.BindTexture();
    renderer.DrawArray(vertex_arrays->skyboxVa, shaders->skyboxShader);
    if (keyinput.show_d3particle)
    {
        Generate_D3Particle();
        d3particle_generator.update_Particle();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    framebuffers->hdrfbo.UnBind();
}
void Game::Generate_PostProcess()
{

    framebuffers->blooming_hightlightFBO.Bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //将原画分出两份，将原画(ambient + color * shadow * blocker_distance),另一份是高亮highlight
    shaders->blooming_highlightshader.Bind();
    framebuffers->hdrfbo.BindTexture(0);;//原画
    shaders->blooming_highlightshader.SetUniform1i("screenTexture", 0);
    renderer.DrawArray(vertex_arrays->quadVa, shaders->blooming_highlightshader);
    framebuffers->blooming_hightlightFBO.UnBind();
    //将高亮贴图进行高斯模糊
    Gaussian_Blured_Texture(1, keyinput.bloom_times, shaders->blooming_blurshader, framebuffers->blooming_hightlightFBO, framebuffers->blooming_blur_horizontalFBO, framebuffers->blooming_blur_verticalFBO, renderer, vertex_arrays->quadVa);
    //结合原画和模糊后的图片形成泛光
    shaders->screenShader.Bind();
    shaders->screenShader.SetUniform1i("gamma", keyinput.gamma);
    shaders->screenShader.SetUniform1f("exposure", keyinput.exposure);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    framebuffers->blooming_hightlightFBO.BindTexture(0);
    shaders->screenShader.SetUniform1i("screenTexture", 0);
    framebuffers->blooming_blur_verticalFBO.BindTexture(1);
    shaders->screenShader.SetUniform1i("blooming_screenTexture", 1);
    framebuffers->cameradepthFBO.BindTexture(2);
    shaders->screenShader.SetUniform1i("assistTexture", 2);
    renderer.DrawArray(vertex_arrays->quadVa, shaders->screenShader);
    if (keyinput.assist_screen)
    {
        glViewport(0, 0, int(screenWidth * 0.4), int(screenHeight * 0.4));
        shaders->basicscreen_shader.Bind();
        framebuffers->ssaoFBO.BindTexture(0);
        renderer.DrawArray(vertex_arrays->quadVa, shaders->basicscreen_shader);
        glViewport(0, 0, screenWidth, screenHeight);
    }
    
    if(keyinput.fxaa_on)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        framebuffers->FXAA_FBO.Write();
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        framebuffers->FXAA_FBO.Bind();
        framebuffers->FXAA_FBO.BindTexture();
        shaders->FXAA_shader.Bind();
        shaders->FXAA_shader.SetUniform1i("fxaaOn", keyinput.fxaa_on);
        shaders->FXAA_shader.SetUniform1f("screen_width", float(screenWidth));
        shaders->FXAA_shader.SetUniform1f("screen_height", float(screenHeight));

        shaders->FXAA_shader.SetUniform1f("lumaThreshold", keyinput.fxaa_lumaThreshold);
        shaders->FXAA_shader.SetUniform1f("mulReduce", keyinput.fxaa_mulReduce);
        shaders->FXAA_shader.SetUniform1f("minReduce", keyinput.fxaa_minReduce);
        shaders->FXAA_shader.SetUniform1f("maxSpan", keyinput.fxaa_maxSpan);
        renderer.DrawArray(vertex_arrays->quadVa, shaders->FXAA_shader);
        framebuffers->FXAA_FBO.UnBind();
        framebuffers->FXAA_FBO.Read();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }


}

void Game::Generate_CubeTexture()
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
    shaders->EnvCubeMapShader.Bind();
    shaders->EnvCubeMapShader.SetUniformmatri4fv("projection", captureProjection);
    pre_framebuffers->envcubemapFBO.SetViewPort();
    textures->equirectangularMap.Bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        pre_framebuffers->envcubemapFBO.Bind(i);
        shaders->EnvCubeMapShader.SetUniformmatri4fv("view", captureViews[i]);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.DrawArray(vertex_arrays->cubeVa, shaders->EnvCubeMapShader);
    }
    pre_framebuffers->envcubemapFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);

}

void Game::Generate_EnvLight_Diffuse()
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
    shaders->EnvCubeMap_ConvolutionShader.Bind();
    shaders->EnvCubeMap_ConvolutionShader.SetUniformmatri4fv("projection", captureProjection);
    pre_framebuffers->envcubemap_convolutionFBO.SetViewPort();
    textures->skybox.Bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        pre_framebuffers->envcubemap_convolutionFBO.Bind(i);
        shaders->EnvCubeMap_ConvolutionShader.SetUniformmatri4fv("view", captureViews[i]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.DrawArray(vertex_arrays->cubeVa, shaders->EnvCubeMap_ConvolutionShader);
    }
    pre_framebuffers->envcubemap_convolutionFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}

void Game::Generate_EnvLight_Specular()
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
    shaders->EnvCubeMap_spec_ConvolutionShader.Bind();
    shaders->EnvCubeMap_spec_ConvolutionShader.SetUniformmatri4fv("projection", captureProjection);
    unsigned int maxMipLevels = 5;
    textures->skybox.Bind();
    for (unsigned int i = 0; i < maxMipLevels; ++i)
    {
        unsigned int mipWidth = 1024 * static_cast<unsigned int>(std::pow(0.5, i));
        unsigned int mipHeight = 1024 * static_cast<unsigned int>(std::pow(0.5, i));
        pre_framebuffers->envcubemap_spec_convolutionFBO.Bindmip_Renderbuffer(mipWidth, mipHeight);
        pre_framebuffers->envcubemap_spec_convolutionFBO.SetViewPort(mipWidth, mipHeight);
        float roughness = (float)i / (float)(maxMipLevels - 1);
        shaders->EnvCubeMap_spec_ConvolutionShader.SetUniform1f("roughness", roughness);
        for (int j = 0; j < 6; j++)
        {
            pre_framebuffers->envcubemap_spec_convolutionFBO.Bind(j, i);
            shaders->EnvCubeMap_spec_ConvolutionShader.SetUniformmatri4fv("view", captureViews[j]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderer.DrawArray(vertex_arrays->cubeVa, shaders->EnvCubeMap_spec_ConvolutionShader);
        }
    }
    textures->skybox.UnBind();
    pre_framebuffers->envcubemap_spec_convolutionFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}

void Game::Generate_EnvLight_Specular_BRDF()
{
    shaders->EnvCubeMap_spec_BRDF_Shader.Bind();
    pre_framebuffers->envcubemap_spec_BRDF_FBO.Bind();
    pre_framebuffers->envcubemap_spec_BRDF_FBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays->quadVa, shaders->EnvCubeMap_spec_BRDF_Shader);
    glEnable(GL_DEPTH_TEST);
    pre_framebuffers->envcubemap_spec_BRDF_FBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
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

std::string Game::Collision_detection()
{
    std::string text("");
    bool x_collision;
    bool y_collision;
    bool z_collision;

    std::map<std::string, Model*>::iterator iter;
    iter = models->models_map.begin();
    for (iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        if (iter->first != "Main_character")
        {
            x_collision = ((max(models->Main_character.aabb[1], iter->second->aabb[1]) - min(models->Main_character.aabb[0], iter->second->aabb[0]))
                < (iter->second->aabb[1] - iter->second->aabb[0] + models->Main_character.aabb[1] - models->Main_character.aabb[0]));
            y_collision = ((max(models->Main_character.aabb[3], iter->second->aabb[3]) - min(models->Main_character.aabb[2], iter->second->aabb[2]))
                < (iter->second->aabb[3] - iter->second->aabb[2] + models->Main_character.aabb[3] - models->Main_character.aabb[2]));
            z_collision = ((max(models->Main_character.aabb[5], iter->second->aabb[5]) - min(models->Main_character.aabb[4], iter->second->aabb[4]))
                < (iter->second->aabb[5] - iter->second->aabb[4] + models->Main_character.aabb[5] - models->Main_character.aabb[4]));
            if (x_collision && y_collision && z_collision)
            {
                text += std::string(iter->first);
            }
        }
    }
    return text;
}

void Game::Update_Spotlight()
{
    glm::vec3 right = glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 up = glm::cross(camera.Front, right);
    glm::vec3 bias = glm::mat3(right, up, camera.Front) * glm::vec3(keyinput.st_bias_x, keyinput.st_bias_y, keyinput.st_bias_z);
    spotlight.bias_direction = bias * (right - camera.Front);
    spotlight.position = camera.Position + spotlight.bias_direction;
    spotlight.direction = camera.Front - spotlight.bias_direction;
    spotlight.spotlight_far_plane = keyinput.spot_far_plane;
    glm::mat4 SpotlightProjection = glm::perspective(glm::radians(45.0f), 1.0f, spotlight.spotlight_near_plane, spotlight.spotlight_far_plane);
    glm::mat4 SpotlightView = glm::lookAt(spotlight.position, spotlight.position + spotlight.direction, glm::vec3(0.0f, 1.0f, 0.0f));//方向为负的光照方向
    spotlight.SpotlightSpaceMatrix = SpotlightProjection * SpotlightView;
}


void Game::Generate_Particle()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    Shader& shader = shaders->Particle_shader;
    shader.Bind();
    glm::mat4 projection = glm::ortho(0.0f, float(screenWidth), 0.0f, float(screenHeight));
    shader.SetUniformmatri4fv("projection", projection);
    textures->Particle_texture.Bind();
    for (Particle particle : particle_generator.particles)
    {
        if (particle.Life > 0.0f)
        {
            shader.SetUniform2f("offset", particle.Position);
            shader.SetUniform4f("color", particle.Color);
            renderer.DrawArray(vertex_arrays->particleVa, shader);
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
}

void Game::Generate_D3Particle()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    Shader& shader = shaders->D3Particle_shader;
    shader.Bind();
    shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    shader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    textures->floor_diffuse.Bind();
    for (D3Particle& particle : d3particle_generator.particles)
    {
        if (particle.Life > 0.0f)
        {
            shader.SetUniformmatri4fv("model", particle.Position);
            shader.SetUniform4f("color", particle.Color);          
            renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
}

void Game::Initialize_Terrain_cpu()
{

    terrain = Get_TerrainData_cpu();
    models->Terrain.Get_index(&terrain.index[0], static_cast<unsigned int>(terrain.index.size()));
    VertexBuffer terrainVb(&terrain.vertex[0], sizeof(float) * static_cast<unsigned int>(terrain.vertex.size()));
    VertexBufferLayout terrainLayout;
    terrainLayout.Push<float>(3);//position
    models->Terrain.va->AddBuffer(terrainVb, terrainLayout);
}
void Game::Initialize_Terrain_gpu()
{  
    terrain = Get_TerrainData_gpu(textures->Terrain_texture.Get_width(), textures->Terrain_texture.Get_height());
    //models->Terrain.Get_index(&terrain.index[0], static_cast<unsigned int>(terrain.index.size()));
    VertexBuffer terrainVb(&terrain.vertex[0], sizeof(float) * static_cast<unsigned int>(terrain.vertex.size()));
    VertexBufferLayout terrainLayout;
    terrainLayout.Push<float>(3);//position
    terrainLayout.Push<float>(2);
    models->Terrain.va->AddBuffer(terrainVb, terrainLayout);
    
}
void Game::Generate_Terrain_cpu()
{

    Shader& shader = shaders->Terrain_cpu_shader;
    shader.Bind();
    // view/projection transformations
    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetUniformmatri4fv("projection", projection);
    shader.SetUniformmatri4fv("view", view);


    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    model[3][1] = -10.0f;
    shader.SetUniformmatri4fv("model", model);


    // render the cube
    models->Terrain.va->Bind();
    models->Terrain.ib->Bind();
    //        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (int strip = 0; strip < terrain.numStrips; strip++)
    {
        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
            terrain.numTrisPerStrip,   // number of indices to render
            GL_UNSIGNED_INT,     // index data type
            (void*)(sizeof(unsigned int) * terrain.numTrisPerStrip * strip)); // offset to starting index
    }
    models->Terrain.va->Unbind();
    models->Terrain.ib->Unbind();
    shader.UnBind();
}
void Game::Generate_Terrain_gpu()
{

    Shader& shader = shaders->Terrain_gpu_shader;
    shader.Bind();
    // view/projection transformations
    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetUniformmatri4fv("projection", projection);
    shader.SetUniformmatri4fv("view", view);


    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    //model[3][1] = -10.0f;
    shader.SetUniformmatri4fv("model", model);
    textures->Terrain_texture.Bind(0);
    shader.SetUniform1i("heightMap", 0);
    textures->floor_diffuse.Bind(1);
    shader.SetUniform1i("textureMap", 1);
    shader.SetUniform1i("tess_level", keyinput.tess_level);
    // render the cube
    models->Terrain.va->Bind();
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_PATCHES, 0, 4 * terrain.rez * terrain.rez);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    models->Terrain.va->Unbind();
    shader.UnBind();
}

void Game::Generate_Health_bar()
{
    Shader& shader = shaders->Health_bar_shader;
    shader.Bind();
    shader.SetUniform1f("screenHeight", static_cast<float>(screenHeight));

    shader.SetUniform1f("max_life", my_state.max_life);
    shader.SetUniform1f("current_life", my_state.current_life);
    shader.SetUniform1f("max_magic", my_state.max_magic);
    shader.SetUniform1f("current_magic", my_state.current_magic);
    shader.SetUniform1f("max_energy", my_state.max_energy);
    shader.SetUniform1f("current_energy", my_state.current_energy);
    renderer.DrawArray(vertex_arrays->quadVa, shader);
    Generate_Health_bar_enemy();
}

void Game::Generate_Health_bar_enemy()
{
    Shader& shader = shaders->Health_bar_enemy_shader;
    shader.Bind();
    std::map<float, std::string> sorted;
    std::map<std::string, Model*>::iterator iter;
    for (iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        if (iter->first != "Main_character")
        {
            float distance = glm::length(camera.Position - glm::vec3(iter->second->position.GetVector(3, 0), iter->second->position.GetVector(3, 1), iter->second->position.GetVector(3, 2)));
            sorted[distance] = iter->first;
        }
    }
    csm_dirlight.Set_far_plane(sorted.rbegin()->first * 10);
    for (std::map<float, std::string>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        float health_bar_xcenter = (models->models_map[it->second]->aabb[0] + models->models_map[it->second]->aabb[1]) * 0.5f;
        float health_bar_y = models->models_map[it->second]->aabb[3] + (models->models_map[it->second]->aabb[3] - models->models_map[it->second]->aabb[2]) * 0.1f;

        glm::vec4 health_bar_position = camera.GetProjectionMatrix() * camera.GetViewMatrix() * glm::vec4(health_bar_xcenter, health_bar_y, 0.0f, 1.0f);
        float life_x = ((health_bar_position.x / health_bar_position.w * 0.5f) + 0.5f) * screenWidth;
        float life_y = ((health_bar_position.y / health_bar_position.w * 0.5f) + 0.5f) * screenHeight;
        shader.SetUniform1f("life_y", life_y);
        shader.SetUniform1f("life_x", life_x);
        shader.SetUniform1f("max_life", models->models_map[it->second]->max_life);
        shader.SetUniform1f("current_life", models->models_map[it->second]->current_life);

        glm::vec3 viewPos;
        if (keyinput.third_view)
            viewPos = camera.character_pos;
        else
            viewPos = camera.Position;
        glm::vec3 FragPos = glm::vec3(health_bar_xcenter, health_bar_y, models->models_map[it->second]->aabb[5]);
        glm::vec3 viewDir = glm::normalize(FragPos - viewPos);
        float length = glm::length(FragPos - viewPos);
        float theta = dot(viewDir, camera.Front);
        if(theta > 0.707 && length < 20.0f)
            renderer.DrawArray(vertex_arrays->quadVa, shader);
    }
}

void Game::ready_render()
{
    shaders = new Shaders;
    textures = new Textures;
    models = new Models;
    pre_framebuffers = new Pre_FrameBuffers();
    framebuffers = new FrameBuffers(screenWidth, screenHeight);
    vertex_arrays = new VertexArrays;
    vertex_buffers = new VertexBuffers;
    animations = new Animations(&models->Robot_boxing);
    animator = new Animator(animations->Animation_walk);
    Debug();
    read_config();

    Initialize_Models_Positions();   
    Initialize_Pointlight();
    Initialize_Vertex_Arrays();
    Initialize_Terrain_gpu();
    TextFBO::Init();
    Generate_CubeTexture();
    Generate_EnvLight_Diffuse();
    Generate_EnvLight_Specular();
    Generate_EnvLight_Specular_BRDF();
    //models->CreatModel("res/objects/x_bot/X_Bot.dae", "x_bot");
    //animator->PlayAnimation(animations->Animation_boxing);
    animator->UpdateAnimation(deltaTime);
}

void Game::start_render()
{
    if(camera.is_move)
        animator->UpdateAnimation(deltaTime);

    camera.Set_third_view(keyinput.third_view);
    camera.Set_free_view(keyinput.free_view);
    glEnable(GL_DEPTH_TEST);
    Update_Pointlight();
    Update_Models_Positions();
    Update_Sun();
    csm_dirlight.Get_light_projection(camera, sun.Sun_Position);
    Generate_Dir_CSM();
    //点光阴影贴图
    if (pointlight.lightintensity > 0)
        Generate_Point_SM();

    //聚光阴影贴图
    Update_Spotlight();
    Generate_Spot_SM();

    //defered shading 绘制基本信息    
    Generate_Defered_basicDATA();

    framebuffers->gbuffer.Read();
    framebuffers->cameradepthFBO.Write();
    framebuffers->gbuffer.BlitDepthBuffer();
    framebuffers->gbuffer.UnBind();

    if (keyinput.useSSAO)
    {
        Generate_SSAO();
        //blur
        Generate_SSAO_blur();
    }
    Generate_PreShadow();

    //设置uniform      着色
    Generate_Origin_Screen();
    //点光源模型 
    //读取gbuffer的深度信息以结合正向渲染    
    Generate_SkyBox();

    //后期处理
    Generate_PostProcess();

    glEnable(GL_BLEND);
    /*std::string text_render = Collision_detection();
    RenderText(shaders->text_shader, text_render, 0.0f, 600.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), vertex_arrays, vertex_buffers);*/
    if (keyinput.show_particle)
    {
        Generate_Particle();
        particle_generator.update_Particle(glm::vec2(screenWidth / 2, 0));
    }
    if (true)
    {
        RenderText(shaders->text_shader, "welcome to the demo", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), vertex_arrays, vertex_buffers);
    }
    glDisable(GL_BLEND);
    Generate_Health_bar();
}