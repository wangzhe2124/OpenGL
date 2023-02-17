#include <GL/glew.h>
#include <GLFW/glfw3.h>


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
//鼠标回调
unsigned int screenWidth = 960;
unsigned int screenHeight = 640;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

float deltaTime = 0.0f;	// 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
//创建相机
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
//键盘输入
KeyInput keyinput;
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    //glfwWindowHint(GLFW_SAMPLES, 4);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Hello World", NULL, NULL);//指针

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

    //鼠标回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keys_callback);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    const char* glsl_version = "#version 130";
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //点光源属性
    float pointlights[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // 前
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,//后
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,//左
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,//右
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f,  -1.0f,  0.0f, 0.0f, 1.0f,//下
     0.5f, -0.5f, -0.5f,  0.0f,  -1.0f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  -1.0f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  -1.0f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  -1.0f,  0.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  -1.0f,  0.0f, 0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // 上
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    VertexArray pointlightVa;
    VertexBuffer pointlightVb(pointlights, sizeof(pointlights));
    VertexBufferLayout pointlightLayout;
    pointlightLayout.Push<float>(3);
    pointlightLayout.Push<float>(3);
    pointlightLayout.Push<float>(2);
    pointlightVa.AddBuffer(pointlightVb, pointlightLayout);
    //点光源属性
#define POINT_LIGHTS_NUM 4
    glm::vec3 pointLightPositions[POINT_LIGHTS_NUM];
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointLightPositions[i] = glm::vec3(i * 3, 2, 0);
    }
    ModelSpace pointlightspace[POINT_LIGHTS_NUM];
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointlightspace[i].Translate(pointLightPositions[i]);
        pointlightspace[i].Scale(glm::vec3(0.5f));
    }
    glm::vec3 pointLightColors[POINT_LIGHTS_NUM];
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointLightColors[i] = glm::vec3(1.0f);
    }
    //地板
    float floorVertices[] = {
        // positions                        // texture Coords //tangent    //bitangent
         -30.0f, -30.0f, 0.0f, 0.0, 0.0,1.0,   0.0f,  0.0f,  1.0, 0.0, 0.0, 0.0, 1.0, 0.0,
          30.0f, -30.0f, 0.0f, 0.0, 0.0,1.0,  30.0f,  0.0f,  1.0, 0.0, 0.0, 0.0, 1.0, 0.0,
          30.0f,  30.0f, 0.0f, 0.0, 0.0,1.0,  30.0f, 30.0f,  1.0, 0.0, 0.0, 0.0, 1.0, 0.0,

          30.0f,  30.0f, 0.0f, 0.0, 0.0,1.0,  30.0f, 30.0f,  1.0, 0.0, 0.0, 0.0, 1.0, 0.0,
         -30.0f,  30.0f, 0.0f, 0.0, 0.0,1.0,   0.0f, 30.0f,  1.0, 0.0, 0.0, 0.0, 1.0, 0.0,
         -30.0f, -30.0f, 0.0f, 0.0, 0.0,1.0,   0.0f,  0.0f,  1.0, 0.0, 0.0, 0.0, 1.0, 0.0,
    };
    VertexArray floorVa;
    VertexBuffer floorVb(floorVertices, sizeof(floorVertices));
    VertexBufferLayout floorLayout;
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(2);
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(3);

    floorVa.AddBuffer(floorVb, floorLayout);
    Texture floor_diffuse("res/textures/bricks.jpg", 3);
    Texture floor_specular("res/textures/bricks.jpg", 3);
    Texture floor_normal("res/textures/bricks_normal.jpg", 3);
    Texture floor_height("res/textures/bricks_height.jpg", 3);

    //shaders
    Shader screenShader("res/shaders/screen.shader");
    Shader blooming_highlightshader("res/shaders/blooming_highlight.shader");
    Shader blooming_blurshader("res/shaders/blooming_blur.shader");
    Shader DeferedShader("res/shaders/Defered.shader");
    Shader pointlightshader("res/shaders/basic.shader");
    Shader modelshader("res/shaders/model.shader");
    //D3Shader model_geometry_shader("res/shaders/model_geomrtry.shader");
    Shader model_instance_shader("res/shaders/model_instance.shader");
    Shader tencil("res/shaders/single.shader");
    Shader skyboxShader("res/shaders/SkyBox.shader");

    Shader DirLightShadowshader("res/shaders/DirLightShadow.shader");
    //D3Shader DirLightCSMShadowshader("res/shaders/DirLightCSMShadow.shader");
    D3Shader PointLightShadowshader("res/shaders/PointLightShadow.shader");
    Shader SpotLightShadowshader("res/shaders/SpotLightShadow.shader");

    //天空盒
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    VertexArray skyboxVa;
    VertexBuffer skyboxVb(skyboxVertices, sizeof(skyboxVertices));
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
    //创建Renderer
    Renderer renderer;
    //投影矩阵
    glm::mat4 projection = glm::mat4(1.0f);
    //models
    Model ourModel("res/objects/nanosuit_upgrade/nanosuit.obj");
    Model Marry("res/objects/Marry/Marry.obj");
    Model Planet("res/objects/planet/planet.obj");
    Model Rock("res/objects/rock/rock.obj");
    Model sakura("res/objects/sakura/sakura.obj");
    //光阴影贴图
    DirLightDepthMapFBO dirlightMap(4096, 4096);
    int split_num = 4;
    CSM_Dirlight csm_dirlight(camera, split_num);
    DirLightDepthMapFBO csm_mapFBO[4] =
    {
    DirLightDepthMapFBO(4096, 4096),
    DirLightDepthMapFBO(4096, 4096),
    DirLightDepthMapFBO(4096, 4096),
    DirLightDepthMapFBO(4096, 4096)

    };

    PointLightDepthMapFBO PointlightMapFBO[4] = { 
         PointLightDepthMapFBO(1024, 1024),
         PointLightDepthMapFBO(1024, 1024),
         PointLightDepthMapFBO(1024, 1024),
         PointLightDepthMapFBO(1024, 1024)
    };
    SpotLightDepthMapFBO SpotlightMapFBO(1024, 1024);

    //后期处理 + 多重采样 + HDR + bloom + defer
        //quadVa后期处理
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    VertexArray quadVa;
    VertexBuffer quadVb(quadVertices, sizeof(quadVertices));
    VertexBufferLayout quadLayout;
    quadLayout.Push<float>(2);
    quadLayout.Push<float>(2);
    quadVa.AddBuffer(quadVb, quadLayout);
    MSAAFrameBuffer msaa(screenWidth, screenHeight, 4);
    Blooming_HighlightFBO blooming_hightlightFBO(screenWidth, screenHeight);
    Blooming_Blur_HorizontalFBO blooming_blur_horizontalFBO(screenWidth, screenHeight);
    Blooming_Blur_VerticalFBO blooming_blur_verticalFBO(screenWidth, screenHeight);
    GBuffer gbuffer(screenWidth, screenHeight);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//锁定鼠标
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //blender
    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
    //面剔除
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);//剔除背面
    glFrontFace(GL_CCW);//逆时针为正向
    //模板测试
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //多重采样
    glEnable(GL_MULTISAMPLE);
    /* Loop until the user closes the window */
    //gamma校正
    //glEnable(GL_FRAMEBUFFER_SRGB);  

    ModelSpace nano_position;
    nano_position.Scale(glm::vec3(0.1f));
    nano_position.Rotate(-85.0, glm::vec3(1.0, 0.0, 0.0));

    ModelSpace Marry_position;
    Marry_position.Translate(glm::vec3(5.0f, 0.0f, 0.0f));
    Marry_position.Scale(0.5f);
    ModelSpace Planet_position;
    Planet_position.Translate(glm::vec3(10.0f, 0, 0));
    ModelSpace floorPosition;
    floorPosition.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        keyinput.ProcessMovement(window, camera, deltaTime);//键盘输入移动
        /* Render here */
        glEnable(GL_DEPTH_TEST);
        //floorPosition.Rotate(0.02, glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
        //nano_position.Rotate(0.2f, glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
        // 
        //绘制阴影贴图
        //平行光阴影贴图
        float sun_height = 20;
        float east_x =  sin(glm::radians(45 + glfwGetTime() / 100))* sun_height;
        float west_y = pow(sun_height* sun_height - pow(east_x,2), 0.5);
        glm::vec3 DirlightPosition = glm::vec3(east_x, west_y, 1.0f) + camera.Position;
        glm::vec3 DirlightDir = -glm::normalize(glm::vec3(east_x, west_y, 1.0f));
        csm_dirlight.Get_light_projection(camera, DirlightPosition);//得到光空间投影矩阵

        for (int i = 0; i < split_num; i++)
        {
            DirLightShadowshader.Bind();
            DirLightShadowshader.SetUniformmatri4fv("LightSpace", csm_dirlight.light_projection_matrix[i]);
            csm_mapFBO[i].Bind();//帧缓冲存储贴图
            csm_mapFBO[i].SetViewPort();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //绘制       
            DirLightShadowshader.SetUniformmatri4fv("model", nano_position.GetModelSpace());
            ourModel.DrawShadow(DirLightShadowshader);

            DirLightShadowshader.SetUniformmatri4fv("model", Marry_position.GetModelSpace());
            Marry.DrawShadow(DirLightShadowshader);

            DirLightShadowshader.SetUniformmatri4fv("model", Planet_position.GetModelSpace());
            Planet.DrawShadow(DirLightShadowshader);

            DirLightShadowshader.SetUniformmatri4fv("model", floorPosition.GetModelSpace());
            renderer.DrawArray(floorVa, DirLightShadowshader);
            
            csm_mapFBO[i].UnBind();
        }

        //点光阴影贴图
        float pointlight_near_plane = 0.1f;
        float pointlight_far_plane = 10.0f;
        glm::mat4 PointlightProjection = glm::perspective(glm::radians(90.0f), 1.0f, pointlight_near_plane, pointlight_far_plane);//aspect应为阴影宽高比
        for (int i = 0; i < POINT_LIGHTS_NUM; i++)
        {
            std::vector<glm::mat4> shadowTransforms;
            shadowTransforms.push_back(PointlightProjection*
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));//右
            shadowTransforms.push_back(PointlightProjection*
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));//左
            shadowTransforms.push_back(PointlightProjection*
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));//上
            shadowTransforms.push_back(PointlightProjection *
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));//下
            shadowTransforms.push_back(PointlightProjection *
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));//前
            shadowTransforms.push_back(PointlightProjection *
                glm::lookAt(pointLightPositions[i], pointLightPositions[i] + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));//后
            PointLightShadowshader.Bind();
            for (int face = 0; face < 6; face++)
            {
                PointLightShadowshader.SetUniformmatri4fv("shadowMatrices[" + std::to_string(face) + "]", shadowTransforms[face]);
            }
            PointLightShadowshader.SetUniform3f("lightPos", pointLightPositions[i]);
            PointLightShadowshader.SetUniform1f("far_plane", pointlight_far_plane);
            PointlightMapFBO[i].Bind();//帧缓冲存储贴图
            PointlightMapFBO[i].SetViewPort();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //绘制
            PointLightShadowshader.SetUniformmatri4fv("model", nano_position.GetModelSpace());
            ourModel.DrawShadow(PointLightShadowshader);

            PointLightShadowshader.SetUniformmatri4fv("model", Marry_position.GetModelSpace());
            Marry.DrawShadow(PointLightShadowshader);

            PointLightShadowshader.SetUniformmatri4fv("model", Planet_position.GetModelSpace());
            Planet.DrawShadow(PointLightShadowshader);

            PointLightShadowshader.SetUniformmatri4fv("model", floorPosition.GetModelSpace());
            renderer.DrawArray(floorVa, PointLightShadowshader);
            
            PointlightMapFBO[i].UnBind();
        }
        //聚光阴影贴图
        float spotlight_near_plane = 0.1f;
        float spotlight_far_plane = 20.0f;
        glm::vec3 bias_direction = glm::vec3(0.15) * (glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f)) - camera.Front);
        glm::vec3 spotlight_position = camera.Position + bias_direction;
        glm::vec3 spotlight_direction = camera.Front - bias_direction;
        glm::mat4 SpotlightProjection = glm::perspective(glm::radians(45.0f), 1.0f, spotlight_near_plane, spotlight_far_plane);
        glm::mat4 SpotlightView = glm::lookAt(spotlight_position, spotlight_position + spotlight_direction, glm::vec3(0.0f, 1.0f, 0.0f));//方向为负的光照方向
        glm::mat4 SpotlightSpaceMatrix = SpotlightProjection * SpotlightView;
        SpotLightShadowshader.Bind();
        SpotLightShadowshader.SetUniformmatri4fv("LightSpace", SpotlightSpaceMatrix);
        SpotlightMapFBO.Bind();//帧缓冲存储贴图
        SpotlightMapFBO.SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //绘制
        SpotLightShadowshader.SetUniformmatri4fv("model", nano_position.GetModelSpace());
        ourModel.DrawShadow(SpotLightShadowshader);

        SpotLightShadowshader.SetUniformmatri4fv("model", Marry_position.GetModelSpace());
        Marry.DrawShadow(SpotLightShadowshader);

        SpotLightShadowshader.SetUniformmatri4fv("model", Planet_position.GetModelSpace());
        Planet.DrawShadow(SpotLightShadowshader);

        SpotLightShadowshader.SetUniformmatri4fv("model", floorPosition.GetModelSpace());
        renderer.DrawArray(floorVa, SpotLightShadowshader);
        
        SpotlightMapFBO.UnBind();
        glViewport(0, 0, screenWidth, screenHeight);
        //点光源模型
        msaa.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        pointlightshader.Bind();
        pointlightshader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        pointlightshader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
        for (unsigned int i = 0; i < POINT_LIGHTS_NUM; i++)
        {
            pointlightshader.SetUniform3f("material.color", pointLightColors[i]);
            pointlightshader.SetUniformmatri4fv("model", pointlightspace[i].GetModelSpace());
            renderer.DrawArray(pointlightVa, pointlightshader);
        }      
        pointlightshader.SetUniform3f("material.color", glm::vec3(1.0, 0.0, 0.0));
        ModelSpace dirlightspace;//太阳位置会变化，所以动态改变位置       
        dirlightspace.Translate(DirlightPosition);
        dirlightspace.Scale(glm::vec3(0.3f));
        pointlightshader.SetUniformmatri4fv("model", dirlightspace.GetModelSpace());
        renderer.DrawArray(pointlightVa, pointlightshader);

        //模型
        modelshader.Bind();       
        for (int i = 0; i < POINT_LIGHTS_NUM; i++)         
        {
            PointlightMapFBO[i].BindTexture(i + 4);
            modelshader.SetUniform1i("shadowMap.PointShadow[" + std::to_string(i) + "]", i + 4);
        }
        for (int i = 0; i < split_num; i++)
        {
            modelshader.SetUniform1f("split_distance[" + std::to_string(i) + "]", csm_dirlight.Get_frustum_far(i));
            modelshader.SetUniform1f("z_distance[" + std::to_string(i) + "]", csm_dirlight.Get_z_distance(i));

            csm_mapFBO[i].BindTexture(i + 8);
            modelshader.SetUniform1i("shadowMap.csm_map[" + std::to_string(i) + "]", i + 8);
            modelshader.SetUniformmatri4fv("DirlightCSMMatrix[" + std::to_string(i) + "]", csm_dirlight.light_projection_matrix[i]);
        }
        SpotlightMapFBO.BindTexture(12);
        modelshader.SetUniform1i("shadowMap.SpotShadow", 12);
        modelshader.SetUniformmatri4fv("SpotlightSpaceMatrix", SpotlightSpaceMatrix);

        modelshader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        modelshader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());

        modelshader.SetUniform3f("objectColor", 0.1f, 0.1f, 0.1f);
        modelshader.SetUniform1f("material.shininess", 16.0f);
        modelshader.SetUniform3f("camera.viewPos", camera.Position);
        //modelshader.SetUniform1f("time", glfwGetTime());
        modelshader.SetUniform1i("blinn_phong", keyinput.blinn_phong);
        //平行光
        modelshader.SetUniform3f("dirlight.ambient", 0.05f, 0.05f, 0.1f);
        modelshader.SetUniform3f("dirlight.diffuse", 0.2f, 0.2f, 0.7f);
        modelshader.SetUniform3f("dirlight.specular", 0.7f, 0.7f, 0.7f);
        modelshader.SetUniform3f("dirlight.direction", DirlightDir);
        modelshader.SetUniform1f("dirlight.LightIntensity", 0.0f);
        modelshader.SetUniform1f("camera.far_plane", camera.far_plane);
        modelshader.SetUniform1f("camera.near_plane", camera.near_plane);
        //点光
        for (int i = 0; i < 4; i++)
        {
            modelshader.SetUniform3f("pointlight[" + std::to_string(i) + "].ambient", pointLightColors[i]);
            modelshader.SetUniform3f("pointlight[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
            modelshader.SetUniform3f("pointlight[" + std::to_string(i) + "].specular", pointLightColors[i]);
            modelshader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointLightPositions[i]);
            modelshader.SetUniform1f("pointlight[" + std::to_string(i) + "].LightIntensity", 20.0f);
            modelshader.SetUniform1f("pointlight[" + std::to_string(i) + "].constant", 1.0f);
            modelshader.SetUniform1f("pointlight[" + std::to_string(i) + "].linear", 0.14f);
            modelshader.SetUniform1f("pointlight[" + std::to_string(i) + "].quadratic", 0.07f);
            modelshader.SetUniform1f("pointlight[" + std::to_string(i) + "].far_plane", pointlight_far_plane);
            modelshader.SetUniform1f("pointlight[" + std::to_string(i) + "].near_plane",pointlight_near_plane);
        }
       
        //聚光手电
        modelshader.SetUniform3f("spotlight.ambient", 0.1f, 0.1f, 0.1f);
        modelshader.SetUniform3f("spotlight.diffuse", 0.5f, 0.5f, 0.5f);
        modelshader.SetUniform3f("spotlight.specular", 0.5f, 0.5f, 0.5f);
        modelshader.SetUniform3f("spotlight.direction", spotlight_direction);
        modelshader.SetUniform3f("spotlight.position", spotlight_position);
        modelshader.SetUniform1f("spotlight.constant", 5.0f);
        modelshader.SetUniform1f("spotlight.linear", 0.009f);
        modelshader.SetUniform1f("spotlight.quadratic", 0.0032f);
        modelshader.SetUniform1f("spotlight.LightIntensity", 30.0f * keyinput.TorchOn);
        modelshader.SetUniform1f("spotlight.inner_CutOff", glm::cos(glm::radians(10.5f)));//spotlight范围
        modelshader.SetUniform1f("spotlight.outer_CutOff", glm::cos(glm::radians(12.5f)));//spotlight范围
        //modelshader.SetUniform1f("spotlight.far_plane", far_plane);
        //modelshader.SetUniform1f("spotlight.near_plane", near_plane);

        modelshader.SetUniform1i("use_NormalMap", 0);
        modelshader.SetUniform1i("use_HeightMap", keyinput.useheight);
        modelshader.SetUniformmatri4fv("model", nano_position.GetModelSpace());
        ourModel.Draw(modelshader);
        modelshader.SetUniform1i("use_NormalMap", 0);
        modelshader.SetUniform1i("use_HeightMap", 0);
        modelshader.SetUniformmatri4fv("model", Marry_position.GetModelSpace());
        Marry.Draw(modelshader);
        modelshader.SetUniformmatri4fv("model", Planet_position.GetModelSpace());
        Planet.Draw(modelshader);

        //地板  
        modelshader.SetUniform1i("use_NormalMap", 1);
        modelshader.SetUniform1i("use_HeightMap", keyinput.useheight);
        floor_diffuse.Bind(0);
        modelshader.SetUniform1i("material.texture_diffuse1", 0);
        floor_specular.Bind(1);
        modelshader.SetUniform1i("material.texture_specular1", 1);
        floor_normal.Bind(2);
        modelshader.SetUniform1i("material.texture_normal1", 2);
        floor_height.Bind(3);
        modelshader.SetUniform1i("material.texture_height1", 3);

        modelshader.SetUniformmatri4fv("model", floorPosition.GetModelSpace());
        renderer.DrawArray(floorVa, modelshader);
        

        //天空盒
        skyboxShader.Bind();
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));//降维移除第四列的位移
        skyboxShader.SetUniformmatri4fv("view", view);
        projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), 0.1f, 100.0f);
        skyboxShader.SetUniformmatri4fv("projection", projection);
        skybox.Bind();
        renderer.DrawArray(skyboxVa, skyboxShader);

        //后期处理
//后期处理
        blooming_hightlightFBO.Bind();
        msaa.Read();
        blooming_hightlightFBO.Write();
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        //将原画分出两份，另一份是高亮highlight
        //blooming_hightlight.Bind();
        blooming_highlightshader.Bind();
        blooming_hightlightFBO.BindTexture();
        glDisable(GL_DEPTH_TEST);
        renderer.DrawArray(quadVa, blooming_highlightshader);
        blooming_hightlightFBO.UnBind();
        //将高亮贴图进行高斯模糊
        bool horizontal = true, first_iteration = true;
        int amount = 10;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        blooming_blurshader.Bind();
        for (unsigned int i = 0; i < amount; i++)
        {
            blooming_blurshader.SetUniform1i("horizontal", horizontal);
            if (first_iteration)
            {
                blooming_blur_horizontalFBO.Bind();
                first_iteration = false;
                blooming_hightlightFBO.BindTexture(0, 1);
            }
            else
                if (horizontal)
                {
                    blooming_blur_horizontalFBO.Bind();
                    blooming_blur_verticalFBO.BindTexture();
                }
                else
                {
                    blooming_blur_verticalFBO.Bind();
                    blooming_blur_horizontalFBO.BindTexture();
                }
            renderer.DrawArray(quadVa, blooming_blurshader);
            horizontal = !horizontal;
        }
        blooming_blur_horizontalFBO.UnBind();
        //结合原画和模糊后的图片形成泛光
        screenShader.Bind();
        screenShader.SetUniform1i("gamma", keyinput.gamma);
        screenShader.SetUniform1f("exposure", keyinput.exposure);

        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        blooming_hightlightFBO.BindTexture(0);
        screenShader.SetUniform1i("screenTexture", 0);
        blooming_blur_verticalFBO.BindTexture(1);
        screenShader.SetUniform1i("blooming_screenTexture", 1);
        renderer.DrawArray(quadVa, screenShader);
        /*
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        screenShader.Bind();
        csm_map[1].BindTexture();
        renderer.DrawArray(quadVa, screenShader);
        */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);//储存着GLFW窗口每一个像素颜色值的大缓冲（双缓冲）
        /* Poll for and process events */
        glfwPollEvents();//触发键盘鼠标输入事件
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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