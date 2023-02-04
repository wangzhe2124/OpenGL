#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <random>
#include <vector>

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
//���ص�
unsigned int screenWidth = 960;
unsigned int screenHeight = 640;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
template <typename T1, typename T2, typename T3>
void Gaussian_Blured_Texture(int j, int times, Shader& blooming_blurshader, T1& PreShadowFBO, T2& shadow_blur_horizontalFBO,
    T3& shadow_blur_verticalFBO, Renderer& renderer, VertexArray& quadVa);
struct Sun_DATA
{
    glm::vec3 Sun_Position;
    glm::vec3 Sun_Direction;
};
Sun_DATA GetSunPosition(Camera& camera);
struct SSAO_DATA
{
    std::vector<glm::vec3> ssaokernel;
    std::vector<glm::vec3> ssaonoise;
};
SSAO_DATA Get_SSAO_SAMPLE();
void Generate_Dir_CSM(int update_split_num, Shader& dircsmshader, CSM_Dirlight& csm_dirlight, DirLightDepthMapFBO* csm_mapFBO,
    Models&models, ModelSpaces& model_positions, Renderer& renderer, VertexArrays& vertex_arrays);
float deltaTime = 0.0f;	// ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
//�������
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
//��������
KeyInput keyinput;
bool Last_CSM_update = true;
glm::mat4 Last_CSM_update_matrix;
int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_SAMPLES, 4);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "demo", NULL, NULL);//ָ��

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK)
        std::cout << "error" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
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
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //config
    std::string strConfigFileName("src/config.ini");
    std::fstream out(strConfigFileName);

    Config config(strConfigFileName);
    // ��ʼ��д��ע��
    out.open(strConfigFileName, std::ios::app);
    if (out.is_open()) {
        //��ȡconfig.ini
        out >> config;
        out.close();
    }   
    //���ص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback); 
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keys_callback);
    
    //���Դ����
    std::string cube_vertex_attr = "cube_vertex_attr";
    std::vector<float> cube_vertex = config.ReadVector(cube_vertex_attr);
    VertexArray cubeVa;
    VertexBuffer cubeVb(&cube_vertex[0], cube_vertex.size() * sizeof(float));
    VertexBufferLayout cubeLayout;
    cubeLayout.Push<float>(3);
    cubeLayout.Push<float>(3);
    cubeLayout.Push<float>(2);
    cubeVa.AddBuffer(cubeVb, cubeLayout);
    //���Դ����
    #define POINT_LIGHTS_NUM 4
    glm::vec3 pointLightPositions[POINT_LIGHTS_NUM];
    for(int i = 0; i< POINT_LIGHTS_NUM; i++)
    {
        pointLightPositions[i] = glm::vec3(i * 3, 2, 0);
    }
    ModelSpace pointlightspace[POINT_LIGHTS_NUM];
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointlightspace[i].Translate(pointLightPositions[i]);
        pointlightspace[i].Scale(glm::vec3(0.3f));
    }
    glm::vec3 pointLightColors[POINT_LIGHTS_NUM];
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointLightColors[i] = glm::vec3(50.0f);
    }
    //�ذ�
    std::string floor_vertex_attr = "floor_vertex_attr";
    std::vector<float> floor_vertex = config.ReadVector(floor_vertex_attr);
    VertexArray floorVa;
    VertexBuffer floorVb(&floor_vertex[0], floor_vertex.size() * sizeof(float));
    VertexBufferLayout floorLayout;
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(2);
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(3);

    floorVa.AddBuffer(floorVb, floorLayout);
    VertexArrays vertex_arrays;
    vertex_arrays.floorVa.AddBuffer(floorVb, floorLayout);
    Texture floor_diffuse("res/textures/bricks.jpg", 3);
    Texture floor_specular("res/textures/bricks.jpg", 3);
    Texture floor_normal("res/textures/bricks_normal.jpg", 3);
    Texture floor_height("res/textures/bricks_height.jpg", 3);
    HDRTexture equirectangularMap("res/textures/Ice_Lake_HiRes_TMap.jpg", 3);

    //shaders
    Shader screenShader("res/shaders/screen.shader");
    Shader blooming_highlightshader("res/shaders/blooming_highlight.shader");
    Shader blooming_blurshader("res/shaders/blooming_blur.shader");
    Shader shadow_blurshader("res/shaders/shadow_blur.shader");
    Shader DeferedShader("res/shaders/Defered.shader");
    Shader DeferedPreShadowShader("res/shaders/DeferedPreShadow.shader");
    Shader SSAOShader("res/shaders/SSAO.shader");
    Shader SSAOBlurShader("res/shaders/SSAOblur.shader");
    Shader pointlightshader("res/shaders/basic.shader");
    Shader EnvCubeMapShader("res/shaders/EnvCubeMap.shader");
    Shader EnvCubeMap_ConvolutionShader("res/shaders/EnvCubeMap_Convolution.shader");
    Shader EnvCubeMap_spec_ConvolutionShader("res/shaders/EnvCubeMap_spec_Convolution.shader");
    Shader EnvCubeMap_spec_BRDF_Shader("res/shaders/EnvCubeMap_spec_BRDF.shader");
    Shader DeferedLighting_shader("res/shaders/DeferedLighting.shader");
    //D3Shader model_geometry_shader("res/shaders/model_geomrtry.shader");
    Shader model_instance_shader("res/shaders/model_instance.shader");
    Shader tencil("res/shaders/single.shader");
    Shader skyboxShader("res/shaders/SkyBox.shader");

    Shader DirLightShadowshader("res/shaders/DirLightShadow.shader");
    D3Shader PointLightShadowshader("res/shaders/PointLightShadow.shader");
    Shader SpotLightShadowshader("res/shaders/SpotLightShadow.shader");

    //��պ�
    std::string skybox_vertex_attr = "skybox_vertex_attr";
    std::vector<float> skybox_vertex = config.ReadVector(skybox_vertex_attr);
    VertexArray skyboxVa;
    VertexBuffer skyboxVb(&skybox_vertex[0], skybox_vertex.size() * sizeof(float));
    VertexBufferLayout skyboxLayout;
    skyboxLayout.Push<float>(3);
    skyboxVa.AddBuffer(skyboxVb, skyboxLayout);
    std::vector<std::string> faces
    {
        "res/textures/skybox/right.jpg",
        "res/textures/skybox/left.jpg",
        "res/textures/skybox/top.jpg",
        "res/textures/skybox/bottom.jpg",
        "res/textures/skybox/front.jpg",
        "res/textures/skybox/back.jpg"
    };
    CubeMapTexture skybox(faces);
    //����Renderer
    Renderer renderer;
    //ͶӰ����
    glm::mat4 projection = glm::mat4(1.0f);
    //models
    Model Nano("res/objects/nanosuit_upgrade/nanosuit.obj");
    Model Marry("res/objects/Marry/Marry.obj");
    Model Planet("res/objects/planet/planet.obj");
    Model Rock("res/objects/rock/rock.obj");
    Model sakura("res/objects/sakura/sakura.obj");
    Models models;
    //����Ӱ��ͼ
    DirLightDepthMapFBO global_dirshadowfbo(4096, 4096);
    int split_num = 4;
    CSM_Dirlight csm_dirlight(camera, split_num);
    DirLightDepthMapFBO csm_mapFBO[4] =
    {
    DirLightDepthMapFBO(4096, 4096),
    DirLightDepthMapFBO(4096, 4096),
    DirLightDepthMapFBO(4096, 4096),
    DirLightDepthMapFBO(4096, 4096)

    };
    PointLightDepthMapFBO PointlightMapfbo[4] = {
         PointLightDepthMapFBO(1024, 1024),
         PointLightDepthMapFBO(1024, 1024),
         PointLightDepthMapFBO(1024, 1024),
         PointLightDepthMapFBO(1024, 1024)
    };
    SpotLightDepthMapFBO SpotlightMapfbo(1024, 1024);
    //��������
    EnvCubeMapFBO envcubemapFBO(1024, 1024);
    EnvCubeMap_ConvolutionFBO envcubemap_convolutionFBO(32, 32);
    EnvCubeMap_spec_ConvolutionFBO envcubemap_spec_convolutionFBO(256, 256);
    EnvCubeMap_spec_BRDF_FBO envcubemap_spec_BRDF_FBO(512, 512);
    //cubeenv
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
    envcubemapFBO.SetViewPort();
    equirectangularMap.Bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        envcubemapFBO.Bind(i);
        EnvCubeMapShader.SetUniformmatri4fv("view", captureViews[i]);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.DrawArray(cubeVa, EnvCubeMapShader);
    }
    envcubemapFBO.UnBind();
    //cubeenv convolution
    {
        EnvCubeMap_ConvolutionShader.Bind();
        EnvCubeMap_ConvolutionShader.SetUniformmatri4fv("projection", captureProjection);
        envcubemap_convolutionFBO.SetViewPort();
        skybox.Bind();
        for (unsigned int i = 0; i < 6; ++i)
        {
            envcubemap_convolutionFBO.Bind(i);
            EnvCubeMap_ConvolutionShader.SetUniformmatri4fv("view", captureViews[i]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderer.DrawArray(cubeVa, EnvCubeMap_ConvolutionShader);
        }
        envcubemap_convolutionFBO.UnBind();
        glViewport(0, 0, screenWidth, screenHeight);
    }
    //cubeenv specular convolution
    {
        EnvCubeMap_spec_ConvolutionShader.Bind();
        EnvCubeMap_spec_ConvolutionShader.SetUniformmatri4fv("projection", captureProjection);
        unsigned int maxMipLevels = 5;
        skybox.Bind();
        for (unsigned int i = 0; i < maxMipLevels; ++i)
        {
            unsigned int mipWidth = 256 * std::pow(0.5, i);
            unsigned int mipHeight = 256 * std::pow(0.5, i);
            envcubemap_spec_convolutionFBO.Bindmip_Renderbuffer(mipWidth, mipHeight);
            envcubemap_spec_convolutionFBO.SetViewPort(mipWidth, mipHeight);
            float roughness = (float)i / (float)(maxMipLevels - 1);
            EnvCubeMap_spec_ConvolutionShader.SetUniform1f("roughness", roughness);
            for (int j = 0; j < 6; j++)
            {
                envcubemap_spec_convolutionFBO.Bind(j , i);
                EnvCubeMap_spec_ConvolutionShader.SetUniformmatri4fv("view", captureViews[j]);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                renderer.DrawArray(cubeVa, EnvCubeMap_spec_ConvolutionShader);
            }
        }
        envcubemap_spec_convolutionFBO.UnBind();
        glViewport(0, 0, screenWidth, screenHeight);
    }
    //���ڴ��� + ���ز��� + HDR + bloom + defer
        //quadVa���ڴ���
    std::string quad_vertex_attr = "quad_vertex_attr";
    std::vector<float> quad_vertex = config.ReadVector(quad_vertex_attr);
    VertexArray quadVa;
    VertexBuffer quadVb(&quad_vertex[0], quad_vertex.size() * sizeof(float));
    VertexBufferLayout quadLayout;
    quadLayout.Push<float>(3);
    quadLayout.Push<float>(2);
    quadVa.AddBuffer(quadVb, quadLayout);
    //       //cubeenv specular BRDF
    {
        EnvCubeMap_spec_BRDF_Shader.Bind();
        envcubemap_spec_BRDF_FBO.Bind();
        envcubemap_spec_BRDF_FBO.SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        renderer.DrawArray(quadVa, EnvCubeMap_spec_BRDF_Shader);
        glEnable(GL_DEPTH_TEST);
        envcubemap_spec_BRDF_FBO.UnBind();
        glViewport(0, 0, screenWidth, screenHeight);

    }
    MSAAFrameBuffer msaa(screenWidth, screenHeight, 4);
    HDRFBO hdrfbo(screenWidth, screenHeight);
    CameraDepthFBO cameradepthFBO(screenWidth, screenHeight);
    Blooming_HighlightFBO blooming_hightlightFBO(screenWidth, screenHeight);
    Blooming_Blur_HorizontalFBO blooming_blur_horizontalFBO(screenWidth, screenHeight);
    Blooming_Blur_VerticalFBO blooming_blur_verticalFBO(screenWidth, screenHeight);
    Shadow_Blur_HorizontalFBO shadow_blur_horizontalFBO(screenWidth, screenHeight);
    Shadow_Blur_VerticalFBO shadow_blur_verticalFBO(screenWidth, screenHeight);
    GBuffer gbuffer(screenWidth, screenHeight);
    GBuffer PreShadowFBO(screenWidth, screenHeight);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//�������
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //blender
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
    //���޳�
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);//�޳�����
    glFrontFace(GL_CCW);//��ʱ��Ϊ����
    //ģ�����
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //���ز���
    glEnable(GL_MULTISAMPLE);
    /* Loop until the user closes the window */
    //gammaУ��
    //glEnable(GL_FRAMEBUFFER_SRGB); 
    //SSAO��������
    std::vector<glm::vec3> ssaoKernel = Get_SSAO_SAMPLE().ssaokernel;
    std::vector<glm::vec3> ssaoNoise = Get_SSAO_SAMPLE().ssaonoise;
    SSAOFBO ssaoFBO(screenWidth, screenHeight, ssaoNoise);
    SSAOBlurFBO ssaoblurFBO(screenWidth, screenHeight);
    //ģ��λ��
    ModelSpaces model_positions;
    model_positions.nano_position.Scale(glm::vec3(0.1f));
    model_positions.nano_position.Translate(glm::vec3(0.0f, 1.0f, 0.0f));
    model_positions.nano_position.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    model_positions.Marry_position.Translate(glm::vec3(5.0f, 0.0f, 0.0f));
    model_positions.Marry_position.Scale(0.5f);
    model_positions.Planet_position.Translate(glm::vec3(10.0f, 0, 0));
    model_positions.floor_Position.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    model_positions.sphere_position.Translate(glm::vec3(-2.0f, 1.0f, 0.0));


    ModelSpace nano_position;
    nano_position.Scale(glm::vec3(0.1f));
    nano_position.Translate(glm::vec3(0.0f, 1.0f, 0.0f));

    nano_position.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));

    ModelSpace Marry_position;
    Marry_position.Translate(glm::vec3(5.0f, 0.0f, 0.0f));
    Marry_position.Scale(0.5f);
    ModelSpace Planet_position;
    Planet_position.Translate(glm::vec3(10.0f, 0, 0));
    ModelSpace floorPosition;
    floorPosition.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    //sphere
    sphere_data sphere = SphereData();
    IndexBuffer sphereIb(&sphere.index[0], sphere.index.size());
    VertexArray sphereVa;
    VertexBuffer sphereVb(&sphere.vertex[0], sizeof(float) * sphere.vertex.size());
    VertexBufferLayout sphereLayout;
    sphereLayout.Push<float>(3);//position
    sphereLayout.Push<float>(3);//normal
    sphereLayout.Push<float>(2);//texcoord
    sphereVa.AddBuffer(sphereVb, sphereLayout);
    ModelSpace spherePosition;
    spherePosition.Translate(glm::vec3(-2.0f, 1.0f, 0.0));
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        keyinput.ProcessMovement(window, camera, deltaTime);//���������ƶ�
        /* Render here */
        glEnable(GL_DEPTH_TEST);
        //floorPosition.Rotate((GLfloat)glfwGetTime() * 2, glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
        //nano_position.Rotate(0.2f, glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
        // 
        //������Ӱ��ͼ
        //ƽ�й���Ӱ��ͼ
        Sun_DATA sun_data = GetSunPosition(camera);
        csm_dirlight.Get_light_projection(camera, sun_data.Sun_Position);//�õ���ռ�ͶӰ����
        
        int update_split_num = split_num-1;//ÿ��֡����һ��3��csm
        if (Last_CSM_update)
        {
            update_split_num = split_num; 
            Last_CSM_update_matrix = csm_dirlight.light_projection_matrix[3];
        }
        Last_CSM_update = !Last_CSM_update;
       // Generate_Dir_CSM(update_split_num, DirLightShadowshader, csm_dirlight,
         //   csm_mapFBO, models, model_positions, renderer, vertex_arrays);
        for (int i = 0; i < update_split_num; i++)
        {
            DirLightShadowshader.Bind();
            DirLightShadowshader.SetUniformmatri4fv("LightSpace", csm_dirlight.light_projection_matrix[i]);

            csm_mapFBO[i].Bind();//֡����洢��ͼ
            csm_mapFBO[i].SetViewPort();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //����       
            DirLightShadowshader.SetUniformmatri4fv("model", nano_position.GetModelSpace());
            Nano.DrawShadow(DirLightShadowshader);

            DirLightShadowshader.SetUniformmatri4fv("model", Marry_position.GetModelSpace());
            Marry.DrawShadow(DirLightShadowshader);

            DirLightShadowshader.SetUniformmatri4fv("model", Planet_position.GetModelSpace());
            Planet.DrawShadow(DirLightShadowshader);

            DirLightShadowshader.SetUniformmatri4fv("model", floorPosition.GetModelSpace());
            renderer.DrawArray(floorVa, DirLightShadowshader);
            csm_mapFBO[i].UnBind();
        }
        //�����Ӱ��ͼ
        float pointlight_near_plane = 0.1f;
        float pointlight_far_plane = 10.0f;
        glm::mat4 PointlightProjection = glm::perspective(glm::radians(90.0f), 1.0f, pointlight_near_plane, pointlight_far_plane);//aspectӦΪ��Ӱ��߱�
        for (int i = 0; i < POINT_LIGHTS_NUM; i++)
        {
            std::vector<glm::mat4> shadowTransforms;
            shadowTransforms.push_back(PointlightProjection *
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));//��
            shadowTransforms.push_back(PointlightProjection *
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));//��
            shadowTransforms.push_back(PointlightProjection *
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));//��
            shadowTransforms.push_back(PointlightProjection *
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));//��
            shadowTransforms.push_back(PointlightProjection *
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));//ǰ
            shadowTransforms.push_back(PointlightProjection *
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));//��
            PointLightShadowshader.Bind();
            for (int face = 0; face < 6; face++)
            {
                PointLightShadowshader.SetUniformmatri4fv("shadowMatrices[" + std::to_string(face) + "]", shadowTransforms[face]);
            }
            PointLightShadowshader.SetUniform3f("lightPos", pointLightPositions[i]);
            PointLightShadowshader.SetUniform1f("far_plane", pointlight_far_plane);
            PointlightMapfbo[i].Bind();//֡����洢��ͼ
            PointlightMapfbo[i].SetViewPort();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //����
            PointLightShadowshader.SetUniformmatri4fv("model", nano_position.GetModelSpace());
            Nano.DrawShadow(PointLightShadowshader);

            PointLightShadowshader.SetUniformmatri4fv("model", Marry_position.GetModelSpace());
            Marry.DrawShadow(PointLightShadowshader);

            PointLightShadowshader.SetUniformmatri4fv("model", Planet_position.GetModelSpace());
            Planet.DrawShadow(PointLightShadowshader);

            PointLightShadowshader.SetUniformmatri4fv("model", floorPosition.GetModelSpace());
            renderer.DrawArray(floorVa, PointLightShadowshader);
            PointlightMapfbo[i].UnBind();
        }   
        //�۹���Ӱ��ͼ
        float spotlight_near_plane = 0.1f;
        float spotlight_far_plane = 20.0f;
        glm::vec3 bias_direction = glm::vec3(0.15) * (glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f)) - camera.Front);
        glm::vec3 spotlight_position = camera.Position + bias_direction;
        glm::vec3 spotlight_direction = camera.Front - bias_direction;
        glm::mat4 SpotlightProjection = glm::perspective(glm::radians(45.0f), 1.0f, spotlight_near_plane, spotlight_far_plane);
        glm::mat4 SpotlightView = glm::lookAt(spotlight_position, spotlight_position + spotlight_direction, glm::vec3(0.0f, 1.0f, 0.0f));//����Ϊ���Ĺ��շ���
        glm::mat4 SpotlightSpaceMatrix = SpotlightProjection * SpotlightView;
        SpotLightShadowshader.Bind();
        SpotLightShadowshader.SetUniformmatri4fv("LightSpace", SpotlightSpaceMatrix);
        SpotlightMapfbo.Bind();//֡����洢��ͼ
        SpotlightMapfbo.SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //����
        SpotLightShadowshader.SetUniformmatri4fv("model", nano_position.GetModelSpace());
        Nano.DrawShadow(SpotLightShadowshader);

        SpotLightShadowshader.SetUniformmatri4fv("model", Marry_position.GetModelSpace());
        Marry.DrawShadow(SpotLightShadowshader);

        SpotLightShadowshader.SetUniformmatri4fv("model", Planet_position.GetModelSpace());
        Planet.DrawShadow(SpotLightShadowshader);

        SpotLightShadowshader.SetUniformmatri4fv("model", floorPosition.GetModelSpace());
        renderer.DrawArray(floorVa, SpotLightShadowshader);
        SpotlightMapfbo.UnBind();
        glViewport(0, 0, screenWidth, screenHeight);
//defered shading ���ƻ�����Ϣ           
        DeferedShader.Bind();
        DeferedShader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        DeferedShader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
        DeferedShader.SetUniform3f("viewPos", camera.Position);
        gbuffer.Bind();
        gbuffer.SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DeferedShader.SetUniform1i("use_NormalMap", keyinput.NormalMap);
        DeferedShader.SetUniform1i("use_HeightMap", keyinput.useheight);
        DeferedShader.SetUniformmatri4fv("model", nano_position.GetModelSpace());
        Nano.Draw(DeferedShader);
        DeferedShader.SetUniform1i("use_NormalMap", 0);
        DeferedShader.SetUniform1i("use_HeightMap", 0);
        DeferedShader.SetUniformmatri4fv("model", Marry_position.GetModelSpace());
        Marry.Draw(DeferedShader);
        DeferedShader.SetUniformmatri4fv("model", Planet_position.GetModelSpace());
        Planet.Draw(DeferedShader);
        //�ذ�  
        DeferedShader.SetUniform1i("use_NormalMap", keyinput.NormalMap);
        DeferedShader.SetUniform1i("use_HeightMap", keyinput.useheight);
        floor_diffuse.Bind(0);
        DeferedShader.SetUniform1i("material.texture_diffuse1", 0);
        floor_specular.Bind(1);
        DeferedShader.SetUniform1i("material.texture_specular1", 1);
        floor_normal.Bind(2);
        DeferedShader.SetUniform1i("material.texture_normal1", 2);
        floor_height.Bind(3);
        DeferedShader.SetUniform1i("material.texture_height1", 3);
        DeferedShader.SetUniformmatri4fv("model", floorPosition.GetModelSpace());
        renderer.DrawArray(floorVa, DeferedShader);
        //����
        DeferedShader.SetUniform1i("use_NormalMap", 0);
        DeferedShader.SetUniform1i("use_HeightMap", 0);
        DeferedShader.SetUniformmatri4fv("model", spherePosition.GetModelSpace());
        renderer.DrawElement(sphereVa,sphereIb, DeferedShader);

        gbuffer.UnBind();
        glViewport(0, 0, screenWidth, screenHeight);
        //SSAO
        gbuffer.Read();
        cameradepthFBO.Write();
        gbuffer.BlitDepthBuffer();
        gbuffer.UnBind();
        if (keyinput.useSSAO)
        {
            ssaoFBO.Bind();
            ssaoFBO.SetViewPort();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            SSAOShader.Bind();
            SSAOShader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
            SSAOShader.SetUniformmatri4fv("view", camera.GetViewMatrix());
            SSAOShader.SetUniform3f("camera.position", camera.Position);
            SSAOShader.SetUniform1f("camera.far_plane", camera.far_plane);

            cameradepthFBO.BindTexture(0);
            SSAOShader.SetUniform1i("camera.Depth", 0);
            gbuffer.BindTexture(1, 0);
            SSAOShader.SetUniform1i("gPosition", 1);
            gbuffer.BindTexture(2, 1);
            SSAOShader.SetUniform1i("gNormal", 2);
            ssaoFBO.BindNoiseTexture(3);
            SSAOShader.SetUniform1i("Noise", 3);
            for (int i = 0; i < 64; i++)
            {
                SSAOShader.SetUniform3f("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
            }
            renderer.DrawArray(quadVa, SSAOShader);
            ssaoFBO.UnBind();
            //blur
            ssaoblurFBO.Bind();
            ssaoblurFBO.SetViewPort();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            SSAOBlurShader.Bind();
            ssaoFBO.BindTexture(0);
            SSAOBlurShader.SetUniform1i("ssaoInput", 0);
            renderer.DrawArray(quadVa, SSAOBlurShader);
            ssaoblurFBO.UnBind();
        }
        glViewport(0, 0, screenWidth, screenHeight);
//Ԥ����shadow     ,ע��cameradepthFBO����ssao���󶨵ģ�����Ҫ��ssao֮����shadow 
        DeferedPreShadowShader.Bind();
        DeferedPreShadowShader.SetUniform3f("camera.viewPos", camera.Position);
        DeferedPreShadowShader.SetUniform1f("camera.far_plane", camera.far_plane);
        DeferedPreShadowShader.SetUniform1f("camera.near_plane", camera.near_plane);
        DeferedPreShadowShader.SetUniform3f("dirlight.direction", sun_data.Sun_Direction);
        for (int i = 0; i < 4; i++)
        {
            DeferedPreShadowShader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointLightPositions[i]);
            DeferedPreShadowShader.SetUniform1f("pointlight[" + std::to_string(i) + "].far_plane", pointlight_far_plane);
        }
        DeferedPreShadowShader.SetUniform3f("camera.viewPos", camera.Position);
        cameradepthFBO.BindTexture(0);
        DeferedPreShadowShader.SetUniform1i("camera.Depth", 0);
        gbuffer.BindTexture(1, 0);
        DeferedPreShadowShader.SetUniform1i("gPosition", 1);
        gbuffer.BindTexture(2, 1);
        DeferedPreShadowShader.SetUniform1i("gNormal", 2);
        gbuffer.BindTexture(3, 2);

        for (int i = 0; i < POINT_LIGHTS_NUM; i++)
        {
            PointlightMapfbo[i].BindTexture(i + 4);
            DeferedPreShadowShader.SetUniform1i("shadowMap.PointShadow[" + std::to_string(i) + "]", i + 4);
        }
        for (int i = 0; i < split_num; i++)
        {
            DeferedPreShadowShader.SetUniform1f("split_distance[" + std::to_string(i) + "]", csm_dirlight.Get_frustum_far(i));
            DeferedPreShadowShader.SetUniform1f("z_distance[" + std::to_string(i) + "]", csm_dirlight.Get_z_distance(i));
            DeferedPreShadowShader.SetUniform1f("xy_distance[" + std::to_string(i) + "]", csm_dirlight.Get_xy_distance(i));
            csm_mapFBO[i].BindTexture(i + 8);
            DeferedPreShadowShader.SetUniform1i("shadowMap.csm_map[" + std::to_string(i) + "]", i + 8);
            if(i ==split_num-1)
                DeferedPreShadowShader.SetUniformmatri4fv("DirlightCSMMatrix[" + std::to_string(i) + "]", Last_CSM_update_matrix);
            else
                DeferedPreShadowShader.SetUniformmatri4fv("DirlightCSMMatrix[" + std::to_string(i) + "]", csm_dirlight.light_projection_matrix[i]);

        }

        SpotlightMapfbo.BindTexture(12);
        DeferedPreShadowShader.SetUniform1i("shadowMap.SpotShadow", 12);
        DeferedPreShadowShader.SetUniformmatri4fv("SpotlightSpaceMatrix", SpotlightSpaceMatrix);

        PreShadowFBO.Bind();//
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        renderer.DrawArray(quadVa, DeferedPreShadowShader);
        glEnable(GL_DEPTH_TEST);
        PreShadowFBO.UnBind();
        glViewport(0, 0, screenWidth, screenHeight);
//��preshdowģ������
        for (int j = 0; j<1; j++)
        {
            Gaussian_Blured_Texture(j, 4,shadow_blurshader, PreShadowFBO, shadow_blur_horizontalFBO, shadow_blur_verticalFBO, renderer, quadVa);
            shadow_blur_verticalFBO.Bind();
            shadow_blur_verticalFBO.ReadTexture();
            PreShadowFBO.WriteTexture(j);
            shadow_blur_verticalFBO.UnBind();
        }

//����uniform      ��ɫ
        DeferedLighting_shader.Bind();
        DeferedLighting_shader.SetUniform3f("objectColor", glm::vec3(keyinput.objectColor));
        DeferedLighting_shader.SetUniform1f("material.metallic", keyinput.metallic);
        DeferedLighting_shader.SetUniform1f("material.roughness", keyinput.roughness);
        DeferedLighting_shader.SetUniform3f("camera.viewPos", camera.Position);
        gbuffer.BindTexture(1, 0);
        DeferedLighting_shader.SetUniform1i("gPosition", 1);
        gbuffer.BindTexture(2, 1);
        DeferedLighting_shader.SetUniform1i("gNormal", 2);
        gbuffer.BindTexture(3, 2);
        DeferedLighting_shader.SetUniform1i("gAlbedoSpec", 3);
        envcubemap_convolutionFBO.BindTexture(4);
        DeferedLighting_shader.SetUniform1i("EnvLight", 4);
        envcubemap_spec_convolutionFBO.BindTexture(5);
        DeferedLighting_shader.SetUniform1i("EnvLight_spec", 5);
        envcubemap_spec_BRDF_FBO.BindTexture(6);
        DeferedLighting_shader.SetUniform1i("EnvLight_spec_BRDF", 6);
        DeferedLighting_shader.SetUniform1i("use_EnvLight_spec", keyinput.EnvLight_spec);


        PreShadowFBO.BindTexture(8, 0);
        DeferedLighting_shader.SetUniform1i("predirshadow", 8);//ƽ�й�

        PreShadowFBO.BindTexture(9, 1);
        DeferedLighting_shader.SetUniform1i("prepointshadow", 9);//���
        PreShadowFBO.BindTexture(10, 2);
        DeferedLighting_shader.SetUniform1i("prespotshadow", 10);//�۹�
        DeferedLighting_shader.SetUniform1i("useSSAO", keyinput.useSSAO);
        if (keyinput.useSSAO)
        {
            ssaoblurFBO.BindTexture(13);
            DeferedLighting_shader.SetUniform1i("ssao", 13);
        }
        
        //ƽ�й�
        DeferedLighting_shader.SetUniform3f("dirlight.color", glm::vec3(keyinput.SunColor));
        DeferedLighting_shader.SetUniform1f("dirlight.LightIntensity", keyinput.SunIntensity);
        DeferedLighting_shader.SetUniform3f("dirlight.direction", sun_data.Sun_Direction);
        //���
        for (int i = 0; i < 4; i++)
        {
            float lightintensity = 0.0;
            DeferedLighting_shader.SetUniform3f("pointlight[" + std::to_string(i) + "].color", pointLightColors[i] * lightintensity);
            DeferedLighting_shader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointLightPositions[i]);
            DeferedLighting_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].LightIntensity", lightintensity);
            DeferedLighting_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].far_plane", pointlight_far_plane);
            DeferedLighting_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].near_plane", pointlight_near_plane);
        }
        //�۹��ֵ�
        DeferedLighting_shader.SetUniform3f("spotlight.color", glm::vec3(30.0f));
        DeferedLighting_shader.SetUniform3f("spotlight.direction", spotlight_direction);
        DeferedLighting_shader.SetUniform3f("spotlight.position", spotlight_position);
        DeferedLighting_shader.SetUniform1f("spotlight.LightIntensity", 1.0f * keyinput.TorchOn);
        DeferedLighting_shader.SetUniform1f("spotlight.inner_CutOff", glm::cos(glm::radians(10.5f)));//spotlight��Χ
        DeferedLighting_shader.SetUniform1f("spotlight.outer_CutOff", glm::cos(glm::radians(12.5f)));//spotlight��Χ
       
        hdrfbo.Bind();//ʹ�÷�Ĭ��֡�������ṩHDR
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        renderer.DrawArray(quadVa, DeferedLighting_shader);
        glEnable(GL_DEPTH_TEST);
        //���Դģ�� 
        //��ȡgbuffer�������Ϣ�Խ��������Ⱦ       
        gbuffer.Read();
        hdrfbo.Write();
        gbuffer.BlitDepthBuffer();

        pointlightshader.Bind();
        pointlightshader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        pointlightshader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
        for (unsigned int i = 0; i < POINT_LIGHTS_NUM; i++)
        {
            pointlightshader.SetUniform3f("material.color", pointLightColors[i]);
            pointlightshader.SetUniformmatri4fv("model", pointlightspace[i].GetModelSpace());
            renderer.DrawArray(cubeVa, pointlightshader);
        }
        pointlightshader.SetUniform3f("material.color", glm::vec3(keyinput.SunColor) * keyinput.SunIntensity);
        ModelSpace dirlightspace;//̫��λ�û�仯�����Զ�̬�ı�λ��       
        dirlightspace.Translate(sun_data.Sun_Position);
        dirlightspace.Scale(glm::vec3(0.3f));
        pointlightshader.SetUniformmatri4fv("model", dirlightspace.GetModelSpace());
        renderer.DrawElement(sphereVa, sphereIb, pointlightshader);
        //��պ�       
        skyboxShader.Bind();
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));//��ά�Ƴ������е�λ��
        skyboxShader.SetUniformmatri4fv("view", view);
        projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), camera.near_plane, camera.far_plane);
        skyboxShader.SetUniformmatri4fv("projection", projection);
        skybox.Bind();
        //envcubemapFBO.BindTexture();
        //envcubemap_spec_convolutionFBO.BindTexture();
        renderer.DrawArray(skyboxVa, skyboxShader);
        hdrfbo.UnBind();

        //����ԭ��
        blooming_hightlightFBO.Bind();
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        //��ԭ���ֳ����ݣ���ԭ��(ambient + color * shadow * blocker_distance),��һ���Ǹ���highlight
        blooming_highlightshader.Bind();
        hdrfbo.BindTexture(0, 0);;//ԭ��
        blooming_highlightshader.SetUniform1i("screenTexture", 0);
        renderer.DrawArray(quadVa, blooming_highlightshader);
        blooming_hightlightFBO.UnBind();
        //��������ͼ���и�˹ģ��
        Gaussian_Blured_Texture(1,10, blooming_blurshader, blooming_hightlightFBO, blooming_blur_horizontalFBO, blooming_blur_verticalFBO, renderer, quadVa);
        //���ԭ����ģ�����ͼƬ�γɷ���
        screenShader.Bind();
        screenShader.SetUniform1i("gamma", keyinput.gamma);
        screenShader.SetUniform1f("exposure", keyinput.exposure);

        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        blooming_hightlightFBO.BindTexture(0);
        screenShader.SetUniform1i("screenTexture", 0);
        blooming_blur_verticalFBO.BindTexture(1);
        screenShader.SetUniform1i("blooming_screenTexture", 1);
        glDisable(GL_BLEND);
        renderer.DrawArray(quadVa, screenShader);

        /*
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        screenShader.Bind();
        glBindTexture(GL_TEXTURE_2D, 0);
        //cameradepth.BindTexture();
        //blooming_hightlightFBO.BindTexture(0,1);
        //blooming_blur_verticalFBO.BindTexture();
        //hdrfbo.BindTexture(0,2);
        //global_dirshadowfbo.BindTexture();
        //shadow_blur_verticalFBO.BindTexture();
        //PreShadowFBO.BindTexture(0, 2);
        //csm_mapFBO[3].BindTexture();
        //envcubemap_spec_BRDF_FBO.BindTexture();
        ssaoblurFBO.BindTexture();
        renderer.DrawArray(quadVa, screenShader);
        */

        
             //GUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //GUI
        // Rendering
        {
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Torch", &keyinput.TorchOn);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Gamma", &keyinput.gamma);
            ImGui::Checkbox("HeightMap", &keyinput.useheight);
            ImGui::Checkbox("NormalMap", &keyinput.NormalMap);
            ImGui::Checkbox("SSAO", &keyinput.useSSAO);
            ImGui::Checkbox("use_EnvLight_spec", &keyinput.EnvLight_spec);
            ImGui::SliderFloat("ObjectColor", &keyinput.objectColor, 0.0f, 1.0f); 
            ImGui::SliderFloat("Metallic", &keyinput.metallic, 0.0f, 1.0f);
            ImGui::SliderFloat("Roughness", &keyinput.roughness, 0.0f, 1.0f);
            ImGui::SliderFloat("LightColor", &keyinput.SunColor, 0.0f, 100.0f);
            ImGui::SliderFloat("LightIntensity", &keyinput.SunIntensity, 0.0f, 5.0f);
            ImGui::SliderFloat("exposure", &keyinput.exposure, 0.0f, 100.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
            static int counter = 0;
            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        if (keyinput.ui) 
        {
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, screenWidth, screenWidth);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//�������
            glfwSetCursorPosCallback(window, nullptr);
            firstMouse = true;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//�������
            glfwSetCursorPosCallback(window, mouse_callback);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);//������GLFW����ÿһ��������ɫֵ�Ĵ󻺳壨˫���壩
        /* Poll for and process events */
        glfwPollEvents();//����������������¼�
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
bool keysPressed[1024];
bool keysReleased[1024] = { true };
void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key >= 0 && key <= 1024)
    {
        if (action == GLFW_PRESS && keysReleased[key])
        {
            keysPressed[key] = true;
            keysReleased[key] = false;
        }
        else if (action == GLFW_RELEASE)
        {
            keysPressed[key] = false;
            keysReleased[key] = true;
        }
    }
    keyinput.ProcessKey(window, key, action);
}
template <typename T1, typename T2, typename T3>
void Gaussian_Blured_Texture(int j, int times, Shader& blooming_blurshader, T1& PreShadowFBO, T2& shadow_blur_horizontalFBO,
    T3& shadow_blur_verticalFBO, Renderer& renderer, VertexArray& quadVa)
{
    bool horizontal = true, first_iteration = true;
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    blooming_blurshader.Bind();
    for (unsigned int i = 0; i <times; i++)
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
SSAO_DATA Get_SSAO_SAMPLE()
{
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // �������������Χ0.0 - 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    for (GLuint i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0;
        scale = 0.1 + 0.9 * scale;
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    std::vector<glm::vec3> ssaoNoise;
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        ssaoNoise.push_back(noise);
    }
    SSAO_DATA ssaodata = { ssaoKernel, ssaoNoise };
    return ssaodata;
}
Sun_DATA GetSunPosition(Camera& camera)
{
    float sun_height = 20;
    float east_x = sin(glm::radians(45 + glfwGetTime() / 100)) * sun_height;
    float west_y = pow(sun_height * sun_height - pow(east_x, 2), 0.5);
    glm::vec3 SunPosition = glm::vec3(east_x, west_y, 1.0f) + camera.Position;
    glm::vec3 SunDireciton = -glm::vec3(east_x, west_y, 1.0f);
    Sun_DATA sundata = { SunPosition, SunDireciton };
    return sundata;
}
