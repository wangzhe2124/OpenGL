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
#define CSM_SPLIT_NUM 4
#define POINT_LIGHTS_NUM 4
struct Sun_DATA
{
    glm::vec3 Sun_Position;
    glm::vec3 Sun_Direction;
}sun;

struct SSAO_DATA
{
    std::vector<glm::vec3> ssaokernel;
    std::vector<glm::vec3> ssaonoise;
};
struct SpotLight_DATA
{
    float spotlight_near_plane = 0.1f;
    float spotlight_far_plane = 20.0f;
    glm::vec3 bias_direction;
    glm::vec3 spotlight_position;
    glm::vec3 spotlight_direction;
    glm::mat4 SpotlightSpaceMatrix;
}spotlight;

struct PointLight_DATA
{
    float lightintensity = 0.0;
    float pointlight_near_plane = 0.1f;
    float pointlight_far_plane = 10.0f;
    glm::vec3 pointLightPositions[POINT_LIGHTS_NUM];
    ModelSpace pointlightspace[POINT_LIGHTS_NUM];
    glm::vec3 pointLightColors[POINT_LIGHTS_NUM];
} pointlight;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

template <typename T1, typename T2, typename T3>
void Gaussian_Blured_Texture(int j, int times, Shader& blooming_blurshader, T1& PreShadowFBO, T2& shadow_blur_horizontalFBO,
    T3& shadow_blur_verticalFBO, Renderer& renderer, VertexArray& quadVa);

Sun_DATA GetSunPosition(Camera& camera);

SSAO_DATA Get_SSAO_SAMPLE();

void Generate_Dir_CSM(Shader& dircsmshader, CSM_Dirlight& csm_dirlight, DirLightDepthMapFBO csm_mapFBO[4],
    Models& models, ModelSpaces& model_positions, Renderer& renderer, VertexArrays& vertex_arrays);

void Generate_Point_SM(D3Shader& Point_sm_shader, PointLightDepthMapFBO* PointlightMapfbo,
    Models& models, ModelSpaces& model_positions, Renderer& renderer, VertexArrays& vertex_arrays, glm::vec3* pointLightPositions);

void Generate_Spot_SM(Shader& Spot_sm_shader, SpotLightDepthMapFBO& SpotlightMapfbo,
    Models& models, ModelSpaces& model_positions, Renderer& renderer, VertexArrays& vertex_arrays, glm::vec3& spot_position,glm::vec3& spot_direction );

void Generate_Defered_basicDATA(Shader& DeferedShader, Camera& camera, GBuffer& gbuffer,
    Models& models, ModelSpaces& model_positions, Renderer& renderer, VertexArrays& vertex_arrays, Textures& textures, IndexBuffers& index_buffers);

void Generate_SSAO(Shader& ssaoshader, Camera&camera, SSAOFBO& ssaoFBO, CameraDepthFBO& cameradepthFBO, std::vector<glm::vec3>& ssaoKernel,
    Renderer& renderer, VertexArrays& vertex_arrays, GBuffer& gbuffer, KeyInput& keyinput);

void Generate_SSAO_blur(Shader& SSAOBlurShader, SSAOBlurFBO& ssaoblurFBO, SSAOFBO& ssaoFBO, Renderer& renderer, VertexArrays& vertex_arrays);

void Generate_PreShadow(Shader& DeferedPreShadowShader, Camera&camera, CameraDepthFBO& cameradepthFBO, GBuffer& gbuffer, GBuffer& PreShadowFBO,
    DirLightDepthMapFBO* csm_mapFBO, CSM_Dirlight& csm_dirlight,PointLightDepthMapFBO* PointlightMapfbo, SpotLightDepthMapFBO& SpotlightMapfbo,
    Sun_DATA& sun, PointLight_DATA& pointlight, SpotLight_DATA& spotlight, Renderer& renderer, VertexArrays& vertex_arrays);

void Generate_Origin_Screen(Shader& DeferedLighting_shader, KeyInput& keyinput, GBuffer& gbuffer, GBuffer& PreShadowFBO,
    EnvCubeMap_ConvolutionFBO& envcubemap_convolutionFBO, EnvCubeMap_spec_ConvolutionFBO& envcubemap_spec_convolutionFBO,
    EnvCubeMap_spec_BRDF_FBO& envcubemap_spec_BRDF_FBO, HDRFBO& hdrfbo, SSAOBlurFBO& ssaoblurFBO,
    Sun_DATA& sun, PointLight_DATA& pointlight, SpotLight_DATA& spotlight, Renderer& renderer, VertexArrays& vertex_arrays);

void Generate_SkyBox(Shader& basic_shader, Shader& skyboxShader,GBuffer& gbuffer, HDRFBO& hdrfbo, Camera& camera, KeyInput& keyinput,
    VertexArrays& vertex_arrays, IndexBuffers& index_buffers, Renderer& renderer, Textures& textures);

void Generate_PostProcess(Shader& screenShader, Shader& blooming_highlightshader, Shader& blooming_blurshader,
    Blooming_HighlightFBO& blooming_hightlightFBO, HDRFBO& hdrfbo, Blooming_Blur_HorizontalFBO& blooming_blur_horizontalFBO,
    Blooming_Blur_VerticalFBO& blooming_blur_verticalFBO, Renderer& renderer, VertexArrays& vertex_arrays);

void Generate_CubeTexture(Shader& EnvCubeMapShader, EnvCubeMapFBO& envcubemapFBO, Textures& textures, VertexArrays& vertex_arrays, Renderer& renderer);

void Generate_EnvLight_Diffuse(Shader& EnvCubeMap_ConvolutionShader, EnvCubeMap_ConvolutionFBO& envcubemap_convolutionFBO,
    Renderer& renderer, VertexArrays& vertex_arrays, Textures& textures);

void Generate_EnvLight_Specular(Shader& EnvCubeMap_spec_ConvolutionShader, EnvCubeMap_spec_ConvolutionFBO& envcubemap_spec_convolutionFBO,
    Renderer& renderer, VertexArrays& vertex_arrays, Textures& textures);

void Generate_EnvLight_Specular_BRDF(Shader& EnvCubeMap_spec_BRDF_Shader, EnvCubeMap_spec_BRDF_FBO& envcubemap_spec_BRDF_FBO,
    Renderer& renderer, VertexArrays& vertex_arrays);

void Initialize_Models_Positions(ModelSpaces& ms);

void GUI_Initialize(GLFWwindow* window);

void GUI_Process(GLFWwindow* window,KeyInput& keyinput);

void Initialize_Vertex_Arrays(VertexArrays& vertex_arrays, Config& config);
//���ص�
unsigned int screenWidth = 960;
unsigned int screenHeight = 640;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;	// ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
//�������
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
//��������
KeyInput keyinput;
//�𼶸���CSM
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
    GUI_Initialize(window);
    //config
    std::string strConfigFileName("src/config.ini");
    std::fstream out(strConfigFileName);
    Config config(strConfigFileName);
    // ��ʼ��д��config
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
    VertexArrays vertex_arrays;
    Initialize_Vertex_Arrays(vertex_arrays, config);

    IndexBuffers index_buffers;

    //���Դ����
    
    for(int i = 0; i< POINT_LIGHTS_NUM; i++)
    {
        pointlight.pointLightPositions[i] = glm::vec3(i * 3, 2, 0);
    }
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointlight.pointlightspace[i].Translate(pointlight.pointLightPositions[i]);
        pointlight.pointlightspace[i].Scale(glm::vec3(0.3f));
    }
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointlight.pointLightColors[i] = glm::vec3(50.0f);
    }
    //texture
    Textures textures;
    //shaders
    Shader screenShader("res/shaders/screen.shader");
    Shader blooming_highlightshader("res/shaders/blooming_highlight.shader");
    Shader blooming_blurshader("res/shaders/blooming_blur.shader");
    Shader shadow_blurshader("res/shaders/shadow_blur.shader");
    Shader DeferedShader("res/shaders/Defered.shader");
    Shader DeferedPreShadowShader("res/shaders/DeferedPreShadow.shader");
    Shader SSAOShader("res/shaders/SSAO.shader");
    Shader SSAOBlurShader("res/shaders/SSAOblur.shader");
    Shader basic_shader("res/shaders/basic.shader");
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

    //����Renderer
    Renderer renderer;
    //models
    Models models;
    //ģ��λ��
    ModelSpaces model_positions;
    Initialize_Models_Positions(model_positions);
    //����Ӱ��ͼ
    DirLightDepthMapFBO global_dirshadowfbo(4096, 4096);
    CSM_Dirlight csm_dirlight(camera, CSM_SPLIT_NUM);
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
    Generate_CubeTexture(EnvCubeMapShader, envcubemapFBO, textures, vertex_arrays, renderer);
    //textures.skybox.CopyId(envcubemapFBO.GetTextureId());
    //cubeenv convolution
    Generate_EnvLight_Diffuse(EnvCubeMap_ConvolutionShader, envcubemap_convolutionFBO,
        renderer, vertex_arrays, textures);
    //cubeenv specular convolution
    Generate_EnvLight_Specular(EnvCubeMap_spec_ConvolutionShader, envcubemap_spec_convolutionFBO,
        renderer, vertex_arrays, textures);
    //cubeenv specular BRDF
    Generate_EnvLight_Specular_BRDF(EnvCubeMap_spec_BRDF_Shader, envcubemap_spec_BRDF_FBO,
        renderer, vertex_arrays);
    //���ڴ��� + ���ز��� + HDR + bloom + defer
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
    //SSAO��������
    std::vector<glm::vec3> ssaoKernel = Get_SSAO_SAMPLE().ssaokernel;
    std::vector<glm::vec3> ssaoNoise = Get_SSAO_SAMPLE().ssaonoise;
    SSAOFBO ssaoFBO(screenWidth, screenHeight, ssaoNoise);
    SSAOBlurFBO ssaoblurFBO(screenWidth, screenHeight);
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
        sun = GetSunPosition(camera);
        csm_dirlight.Get_light_projection(camera, sun.Sun_Position);//�õ���ռ�ͶӰ����
        
        
        Generate_Dir_CSM(DirLightShadowshader, csm_dirlight,
            csm_mapFBO, models, model_positions, renderer, vertex_arrays);

        //�����Ӱ��ͼ

        float pointlight_near_plane = 0.1f;
        float pointlight_far_plane = 10.0f;
        Generate_Point_SM(PointLightShadowshader, PointlightMapfbo,
            models, model_positions, renderer, vertex_arrays, pointlight.pointLightPositions);
 
        //�۹���Ӱ��ͼ

        spotlight.bias_direction = glm::vec3(0.15) * (glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f)) - camera.Front);
        spotlight.spotlight_position = camera.Position + spotlight.bias_direction;
        spotlight.spotlight_direction = camera.Front - spotlight.bias_direction;
        Generate_Spot_SM(SpotLightShadowshader, SpotlightMapfbo,
            models, model_positions, renderer, vertex_arrays, spotlight.spotlight_position, spotlight.spotlight_direction);
        glm::mat4 SpotlightProjection = glm::perspective(glm::radians(45.0f), 1.0f, spotlight.spotlight_near_plane, spotlight.spotlight_far_plane);
        glm::mat4 SpotlightView = glm::lookAt(spotlight.spotlight_position, spotlight.spotlight_position + spotlight.spotlight_direction, glm::vec3(0.0f, 1.0f, 0.0f));//����Ϊ���Ĺ��շ���
        glm::mat4 SpotlightSpaceMatrix = SpotlightProjection * SpotlightView;
        
//defered shading ���ƻ�����Ϣ    
        Generate_Defered_basicDATA(DeferedShader, camera, gbuffer,
            models, model_positions, renderer, vertex_arrays, textures, index_buffers);
        //deliver depth
        gbuffer.Read();
        cameradepthFBO.Write();
        gbuffer.BlitDepthBuffer();
        gbuffer.UnBind();
        //SSAO       
        if (keyinput.useSSAO)
        {
            Generate_SSAO(SSAOShader, camera, ssaoFBO, cameradepthFBO, ssaoKernel,
                renderer, vertex_arrays, gbuffer, keyinput);
            //blur
            Generate_SSAO_blur(SSAOBlurShader, ssaoblurFBO, ssaoFBO, renderer, vertex_arrays);
        }       
//Ԥ����shadow     ,ע��cameradepthFBO����ssao���󶨵ģ�����Ҫ��ssao֮����shadow 
        Generate_PreShadow(DeferedPreShadowShader, camera, cameradepthFBO, gbuffer, PreShadowFBO,
            csm_mapFBO, csm_dirlight, PointlightMapfbo, SpotlightMapfbo,
            sun, pointlight, spotlight, renderer, vertex_arrays);

//��preshdowģ������
        for (int j = 0; j<1; j++)
        {
            Gaussian_Blured_Texture(j, 4,shadow_blurshader, PreShadowFBO, shadow_blur_horizontalFBO, shadow_blur_verticalFBO, renderer, vertex_arrays.quadVa);
            shadow_blur_verticalFBO.Bind();
            shadow_blur_verticalFBO.ReadTexture();
            PreShadowFBO.WriteTexture(j);
            shadow_blur_verticalFBO.UnBind();
        }

//����uniform      ��ɫ
        Generate_Origin_Screen(DeferedLighting_shader, keyinput, gbuffer, PreShadowFBO,
            envcubemap_convolutionFBO, envcubemap_spec_convolutionFBO,
            envcubemap_spec_BRDF_FBO, hdrfbo, ssaoblurFBO,
            sun, pointlight, spotlight, renderer, vertex_arrays);
        //���Դģ�� 
        //��ȡgbuffer�������Ϣ�Խ��������Ⱦ    
        Generate_SkyBox(basic_shader, skyboxShader, gbuffer, hdrfbo, camera, keyinput,
            vertex_arrays, index_buffers, renderer, textures);

        //����ԭ��
        Generate_PostProcess(screenShader, blooming_highlightshader, blooming_blurshader,
            blooming_hightlightFBO, hdrfbo, blooming_blur_horizontalFBO,
            blooming_blur_verticalFBO, renderer, vertex_arrays);


        
        //glDisable(GL_DEPTH_TEST);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        //screenShader.Bind();
        //glBindTexture(GL_TEXTURE_2D, 0);
        ////cameradepthFBO.BindTexture();
        ////blooming_hightlightFBO.BindTexture(0,1);
        ////blooming_blur_verticalFBO.BindTexture();
        ////hdrfbo.BindTexture(0,2);
        ////global_dirshadowfbo.BindTexture();
        ////shadow_blur_verticalFBO.BindTexture();
        ////PreShadowFBO.BindTexture(0, 2);
        ////csm_mapFBO[3].BindTexture();
        ////envcubemap_spec_BRDF_FBO.BindTexture();
        //ssaoFBO.BindTexture();
        //renderer.DrawArray(vertex_arrays.quadVa, screenShader);
        //

        
             //GUI
        GUI_Process(window, keyinput);
        //GUI
        // Rendering
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
    system("pause");
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
void Generate_Dir_CSM(Shader& dircsmshader, CSM_Dirlight& csm_dirlight,
    DirLightDepthMapFBO csm_mapFBO[4], Models& models, ModelSpaces& model_positions, Renderer& renderer, VertexArrays& vertex_arrays)
{
    int update_CSM_SPLIT_NUM = CSM_SPLIT_NUM - 1;//ÿ��֡����һ��3��csm
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

        csm_mapFBO[i].Bind();//֡����洢��ͼ
        csm_mapFBO[i].SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //����       
        dircsmshader.SetUniformmatri4fv("model", model_positions.nano_position.GetModelSpace());
        models.Nano.DrawShadow(dircsmshader);

        dircsmshader.SetUniformmatri4fv("model", model_positions.Marry_position.GetModelSpace());
        models.Marry.DrawShadow(dircsmshader);

        dircsmshader.SetUniformmatri4fv("model", model_positions.Planet_position.GetModelSpace());
        models.Planet.DrawShadow(dircsmshader);

        dircsmshader.SetUniformmatri4fv("model", model_positions.floor_Position.GetModelSpace());
        renderer.DrawArray(vertex_arrays.floorVa, dircsmshader);
        csm_mapFBO[i].UnBind();

    }
    glViewport(0, 0, screenWidth, screenHeight);

}
void Generate_Point_SM(D3Shader& Point_sm_shader,  PointLightDepthMapFBO* PointlightMapfbo,
    Models& models, ModelSpaces& model_positions, Renderer& renderer, VertexArrays& vertex_arrays, glm::vec3* pointLightPositions)
{
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
        Point_sm_shader.Bind();
        for (int face = 0; face < 6; face++)
        {
            Point_sm_shader.SetUniformmatri4fv("shadowMatrices[" + std::to_string(face) + "]", shadowTransforms[face]);
        }
        Point_sm_shader.SetUniform3f("lightPos", pointLightPositions[i]);
        Point_sm_shader.SetUniform1f("far_plane", pointlight_far_plane);
        PointlightMapfbo[i].Bind();//֡����洢��ͼ
        PointlightMapfbo[i].SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //����
        Point_sm_shader.SetUniformmatri4fv("model", model_positions.nano_position.GetModelSpace());
        models.Nano.DrawShadow(Point_sm_shader);

        Point_sm_shader.SetUniformmatri4fv("model", model_positions.Marry_position.GetModelSpace());
        models.Marry.DrawShadow(Point_sm_shader);

        Point_sm_shader.SetUniformmatri4fv("model", model_positions.Planet_position.GetModelSpace());
        models.Planet.DrawShadow(Point_sm_shader);

        Point_sm_shader.SetUniformmatri4fv("model", model_positions.floor_Position.GetModelSpace());
        renderer.DrawArray(vertex_arrays.floorVa, Point_sm_shader);
        PointlightMapfbo[i].UnBind();      
    }
    glViewport(0, 0, screenWidth, screenHeight);

}
void Generate_Spot_SM(Shader& Spot_sm_shader, SpotLightDepthMapFBO& SpotlightMapfbo,
    Models& models, ModelSpaces& model_positions, Renderer& renderer, VertexArrays& vertex_arrays, glm::vec3& spotlight_position, glm::vec3& spotlight_direction)
{
    float spotlight_near_plane = 0.1f;
    float spotlight_far_plane = 20.0f;
    glm::mat4 SpotlightProjection = glm::perspective(glm::radians(45.0f), 1.0f, spotlight_near_plane, spotlight_far_plane);
    glm::mat4 SpotlightView = glm::lookAt(spotlight_position, spotlight_position + spotlight_direction, glm::vec3(0.0f, 1.0f, 0.0f));//����Ϊ���Ĺ��շ���
    glm::mat4 SpotlightSpaceMatrix = SpotlightProjection * SpotlightView;
    Spot_sm_shader.Bind();
    Spot_sm_shader.SetUniformmatri4fv("LightSpace", SpotlightSpaceMatrix);
    SpotlightMapfbo.Bind();//֡����洢��ͼ
    SpotlightMapfbo.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //����
    Spot_sm_shader.SetUniformmatri4fv("model", model_positions.nano_position.GetModelSpace());
    models.Nano.DrawShadow(Spot_sm_shader);

    Spot_sm_shader.SetUniformmatri4fv("model", model_positions.Marry_position.GetModelSpace());
    models.Marry.DrawShadow(Spot_sm_shader);

    Spot_sm_shader.SetUniformmatri4fv("model", model_positions.Planet_position.GetModelSpace());
    models.Planet.DrawShadow(Spot_sm_shader);

    Spot_sm_shader.SetUniformmatri4fv("model", model_positions.floor_Position.GetModelSpace());
    renderer.DrawArray(vertex_arrays.floorVa, Spot_sm_shader);
    SpotlightMapfbo.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Generate_Defered_basicDATA(Shader& DeferedShader, Camera& camera, GBuffer& gbuffer,
    Models& models, ModelSpaces& model_positions, Renderer& renderer, VertexArrays& vertex_arrays, Textures& textures, IndexBuffers& index_buffers)
{
    DeferedShader.Bind();
    DeferedShader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    DeferedShader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    DeferedShader.SetUniform3f("viewPos", camera.Position);
    gbuffer.Bind();
    gbuffer.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DeferedShader.SetUniform1i("use_NormalMap", keyinput.NormalMap);
    DeferedShader.SetUniform1i("use_HeightMap", keyinput.useheight);
    DeferedShader.SetUniformmatri4fv("model", model_positions.nano_position.GetModelSpace());
    models.Nano.Draw(DeferedShader);
    DeferedShader.SetUniform1i("use_NormalMap", 0);
    DeferedShader.SetUniform1i("use_HeightMap", 0);
    DeferedShader.SetUniformmatri4fv("model", model_positions.Marry_position.GetModelSpace());
    models.Marry.Draw(DeferedShader);
    DeferedShader.SetUniformmatri4fv("model", model_positions.Planet_position.GetModelSpace());
    models.Planet.Draw(DeferedShader);
    //�ذ�  
    DeferedShader.SetUniform1i("use_NormalMap", keyinput.NormalMap);
    DeferedShader.SetUniform1i("use_HeightMap", keyinput.useheight);
    textures.floor_diffuse.Bind(0);
    DeferedShader.SetUniform1i("material.texture_diffuse1", 0);
    textures.floor_specular.Bind(1);
    DeferedShader.SetUniform1i("material.texture_specular1", 1);
    textures.floor_normal.Bind(2);
    DeferedShader.SetUniform1i("material.texture_normal1", 2);
    textures.floor_height.Bind(3);
    DeferedShader.SetUniform1i("material.texture_height1", 3);
    DeferedShader.SetUniformmatri4fv("model", model_positions.floor_Position.GetModelSpace());
    renderer.DrawArray(vertex_arrays.floorVa, DeferedShader);
    //����
    DeferedShader.SetUniform1i("use_NormalMap", 0);
    DeferedShader.SetUniform1i("use_HeightMap", 0);
    DeferedShader.SetUniformmatri4fv("model", model_positions.sphere_position.GetModelSpace());
    renderer.DrawElement(vertex_arrays.sphereVa, index_buffers.sphereIb, DeferedShader);

    gbuffer.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Generate_SSAO(Shader& SSAOShader, Camera& camera, SSAOFBO& ssaoFBO, CameraDepthFBO& cameradepthFBO, std::vector<glm::vec3>& ssaoKernel,
    Renderer& renderer, VertexArrays& vertex_arrays, GBuffer& gbuffer, KeyInput& keyinput)
{
    ssaoFBO.Bind();
    ssaoFBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SSAOShader.Bind();
    SSAOShader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    SSAOShader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    SSAOShader.SetUniform3f("camera.position", camera.Position);
    SSAOShader.SetUniform1f("camera.far_plane", camera.far_plane);
    SSAOShader.SetUniform1f("bias", keyinput.SSAO_bias);
    SSAOShader.SetUniform1f("radius", keyinput.SSAO_radius);

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
    renderer.DrawArray(vertex_arrays.quadVa, SSAOShader);
    ssaoFBO.UnBind();
}
void Generate_SSAO_blur(Shader& SSAOBlurShader, SSAOBlurFBO& ssaoblurFBO, SSAOFBO& ssaoFBO, Renderer& renderer, VertexArrays& vertex_arrays)
{
    ssaoblurFBO.Bind();
    ssaoblurFBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SSAOBlurShader.Bind();
    ssaoFBO.BindTexture(0);
    SSAOBlurShader.SetUniform1i("ssaoInput", 0);
    renderer.DrawArray(vertex_arrays.quadVa, SSAOBlurShader);
    ssaoblurFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Generate_PreShadow(Shader& DeferedPreShadowShader, Camera& camera, CameraDepthFBO& cameradepthFBO, GBuffer& gbuffer, GBuffer& PreShadowFBO,
    DirLightDepthMapFBO* csm_mapFBO, CSM_Dirlight& csm_dirlight, PointLightDepthMapFBO* PointlightMapfbo, SpotLightDepthMapFBO& SpotlightMapfbo,
    Sun_DATA& sun, PointLight_DATA& pointlight, SpotLight_DATA& spotlight, Renderer& renderer, VertexArrays& vertex_arrays)
{
    DeferedPreShadowShader.Bind();
    DeferedPreShadowShader.SetUniform3f("camera.viewPos", camera.Position);
    DeferedPreShadowShader.SetUniform1f("camera.far_plane", camera.far_plane);
    DeferedPreShadowShader.SetUniform1f("camera.near_plane", camera.near_plane);
    DeferedPreShadowShader.SetUniform3f("dirlight.direction", sun.Sun_Direction);
    for (int i = 0; i < 4; i++)
    {
        DeferedPreShadowShader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointlight.pointLightPositions[i]);
        DeferedPreShadowShader.SetUniform1f("pointlight[" + std::to_string(i) + "].far_plane", pointlight.pointlight_far_plane);
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
    for (int i = 0; i < CSM_SPLIT_NUM; i++)
    {
        DeferedPreShadowShader.SetUniform1f("split_distance[" + std::to_string(i) + "]", csm_dirlight.Get_frustum_far(i));
        DeferedPreShadowShader.SetUniform1f("z_distance[" + std::to_string(i) + "]", csm_dirlight.Get_z_distance(i));
        DeferedPreShadowShader.SetUniform1f("xy_distance[" + std::to_string(i) + "]", csm_dirlight.Get_xy_distance(i));
        csm_mapFBO[i].BindTexture(i + 8);
        DeferedPreShadowShader.SetUniform1i("shadowMap.csm_map[" + std::to_string(i) + "]", i + 8);
        if (i == CSM_SPLIT_NUM - 1)
            DeferedPreShadowShader.SetUniformmatri4fv("DirlightCSMMatrix[" + std::to_string(i) + "]", Last_CSM_update_matrix);
        else
            DeferedPreShadowShader.SetUniformmatri4fv("DirlightCSMMatrix[" + std::to_string(i) + "]", csm_dirlight.light_projection_matrix[i]);

    }

    SpotlightMapfbo.BindTexture(12);
    DeferedPreShadowShader.SetUniform1i("shadowMap.SpotShadow", 12);
    DeferedPreShadowShader.SetUniformmatri4fv("SpotlightSpaceMatrix", spotlight.SpotlightSpaceMatrix);

    PreShadowFBO.Bind();//
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays.quadVa, DeferedPreShadowShader);
    glEnable(GL_DEPTH_TEST);
    PreShadowFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Generate_Origin_Screen(Shader& DeferedLighting_shader, KeyInput& keyinput, GBuffer& gbuffer, GBuffer& PreShadowFBO,
    EnvCubeMap_ConvolutionFBO& envcubemap_convolutionFBO, EnvCubeMap_spec_ConvolutionFBO& envcubemap_spec_convolutionFBO,
    EnvCubeMap_spec_BRDF_FBO& envcubemap_spec_BRDF_FBO, HDRFBO& hdrfbo, SSAOBlurFBO& ssaoblurFBO,
    Sun_DATA& sun, PointLight_DATA& pointlight, SpotLight_DATA& spotlight, Renderer& renderer, VertexArrays& vertex_arrays)
{
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
    DeferedLighting_shader.SetUniform3f("dirlight.direction", sun.Sun_Direction);
    //���
    for (int i = 0; i < 4; i++)
    {
        DeferedLighting_shader.SetUniform3f("pointlight[" + std::to_string(i) + "].color", pointlight.pointLightColors[i] * pointlight.lightintensity);
        DeferedLighting_shader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointlight.pointLightPositions[i]);
        DeferedLighting_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].LightIntensity", pointlight.lightintensity);
        DeferedLighting_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].far_plane", pointlight.pointlight_far_plane);
        DeferedLighting_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].near_plane", pointlight.pointlight_near_plane);
    }
    //�۹��ֵ�
    DeferedLighting_shader.SetUniform3f("spotlight.color", glm::vec3(30.0f));
    DeferedLighting_shader.SetUniform3f("spotlight.direction", spotlight.spotlight_direction);
    DeferedLighting_shader.SetUniform3f("spotlight.position", spotlight.spotlight_position);
    DeferedLighting_shader.SetUniform1f("spotlight.LightIntensity", 1.0f * keyinput.TorchOn);
    DeferedLighting_shader.SetUniform1f("spotlight.inner_CutOff", glm::cos(glm::radians(10.5f)));//spotlight��Χ
    DeferedLighting_shader.SetUniform1f("spotlight.outer_CutOff", glm::cos(glm::radians(12.5f)));//spotlight��Χ

    hdrfbo.Bind();//ʹ�÷�Ĭ��֡�������ṩHDR
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays.quadVa, DeferedLighting_shader);
    glEnable(GL_DEPTH_TEST);
}
void Generate_SkyBox(Shader& basic_shader, Shader& skyboxShader, GBuffer& gbuffer, HDRFBO& hdrfbo, Camera& camera, KeyInput& keyinput,
    VertexArrays& vertex_arrays, IndexBuffers& index_buffers, Renderer& renderer, Textures& textures)
{
    //��ȡ�����Ϣ
    gbuffer.Read();
    hdrfbo.Write();
    gbuffer.BlitDepthBuffer();

    basic_shader.Bind();
    basic_shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    basic_shader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    for (unsigned int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        basic_shader.SetUniform3f("material.color", pointlight.pointLightColors[i]);
        basic_shader.SetUniformmatri4fv("model", pointlight.pointlightspace[i].GetModelSpace());
        renderer.DrawElement(vertex_arrays.sphereVa, index_buffers.sphereIb, basic_shader);
    }
    basic_shader.SetUniform3f("material.color", glm::vec3(keyinput.SunColor) * keyinput.SunIntensity);
    ModelSpace dirlightspace;//̫��λ�û�仯�����Զ�̬�ı�λ��       
    dirlightspace.Translate(sun.Sun_Position);
    dirlightspace.Scale(glm::vec3(0.3f));
    basic_shader.SetUniformmatri4fv("model", dirlightspace.GetModelSpace());
    renderer.DrawElement(vertex_arrays.sphereVa, index_buffers.sphereIb, basic_shader);
    //��պ�       
    skyboxShader.Bind();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));//��ά�Ƴ������е�λ��
    skyboxShader.SetUniformmatri4fv("view", view);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), camera.near_plane, camera.far_plane);
    skyboxShader.SetUniformmatri4fv("projection", projection);
    textures.skybox.Bind();
    //envcubemapFBO.BindTexture();
    //envcubemap_spec_convolutionFBO.BindTexture();
    renderer.DrawArray(vertex_arrays.skyboxVa, skyboxShader);
    hdrfbo.UnBind();
}
void Generate_PostProcess(Shader& screenShader, Shader& blooming_highlightshader, Shader& blooming_blurshader,
    Blooming_HighlightFBO& blooming_hightlightFBO, HDRFBO& hdrfbo, Blooming_Blur_HorizontalFBO& blooming_blur_horizontalFBO,
    Blooming_Blur_VerticalFBO& blooming_blur_verticalFBO, Renderer& renderer, VertexArrays& vertex_arrays)
{
    blooming_hightlightFBO.Bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //��ԭ���ֳ����ݣ���ԭ��(ambient + color * shadow * blocker_distance),��һ���Ǹ���highlight
    blooming_highlightshader.Bind();
    hdrfbo.BindTexture(0, 0);;//ԭ��
    blooming_highlightshader.SetUniform1i("screenTexture", 0);
    renderer.DrawArray(vertex_arrays.quadVa, blooming_highlightshader);
    blooming_hightlightFBO.UnBind();
    //��������ͼ���и�˹ģ��
    Gaussian_Blured_Texture(1, 10, blooming_blurshader, blooming_hightlightFBO, blooming_blur_horizontalFBO, blooming_blur_verticalFBO, renderer, vertex_arrays.quadVa);
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
    renderer.DrawArray(vertex_arrays.quadVa, screenShader);
}

void Generate_CubeTexture(Shader& EnvCubeMapShader, EnvCubeMapFBO& envcubemapFBO, Textures& textures, VertexArrays& vertex_arrays, Renderer& renderer)
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
    envcubemapFBO.SetViewPort();
    textures.equirectangularMap.Bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        envcubemapFBO.Bind(i);
        EnvCubeMapShader.SetUniformmatri4fv("view", captureViews[i]);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.DrawArray(vertex_arrays.cubeVa, EnvCubeMapShader);
    }
    envcubemapFBO.UnBind();
}

void Generate_EnvLight_Diffuse(Shader& EnvCubeMap_ConvolutionShader, EnvCubeMap_ConvolutionFBO& envcubemap_convolutionFBO,
    Renderer& renderer, VertexArrays& vertex_arrays, Textures& textures)
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
    envcubemap_convolutionFBO.SetViewPort();
    textures.skybox.Bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        envcubemap_convolutionFBO.Bind(i);
        EnvCubeMap_ConvolutionShader.SetUniformmatri4fv("view", captureViews[i]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.DrawArray(vertex_arrays.cubeVa, EnvCubeMap_ConvolutionShader);
    }
    envcubemap_convolutionFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}

void Generate_EnvLight_Specular(Shader& EnvCubeMap_spec_ConvolutionShader, EnvCubeMap_spec_ConvolutionFBO& envcubemap_spec_convolutionFBO,
    Renderer& renderer, VertexArrays& vertex_arrays, Textures& textures)
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
    textures.skybox.Bind();
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
            envcubemap_spec_convolutionFBO.Bind(j, i);
            EnvCubeMap_spec_ConvolutionShader.SetUniformmatri4fv("view", captureViews[j]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderer.DrawArray(vertex_arrays.cubeVa, EnvCubeMap_spec_ConvolutionShader);
        }
    }
    envcubemap_spec_convolutionFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}

void Generate_EnvLight_Specular_BRDF(Shader& EnvCubeMap_spec_BRDF_Shader, EnvCubeMap_spec_BRDF_FBO& envcubemap_spec_BRDF_FBO,
    Renderer& renderer, VertexArrays& vertex_arrays)
{
    EnvCubeMap_spec_BRDF_Shader.Bind();
    envcubemap_spec_BRDF_FBO.Bind();
    envcubemap_spec_BRDF_FBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays.quadVa, EnvCubeMap_spec_BRDF_Shader);
    glEnable(GL_DEPTH_TEST);
    envcubemap_spec_BRDF_FBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}

void Initialize_Models_Positions(ModelSpaces& model_positions)
{
    model_positions.nano_position.Scale(glm::vec3(0.1f));
    model_positions.nano_position.Translate(glm::vec3(0.0f, 1.0f, 0.0f));
    model_positions.nano_position.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    model_positions.Marry_position.Translate(glm::vec3(5.0f, 0.0f, 0.0f));
    model_positions.Marry_position.Scale(0.5f);
    model_positions.Planet_position.Translate(glm::vec3(10.0f, 0, 0));
    model_positions.floor_Position.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    model_positions.sphere_position.Translate(glm::vec3(-2.0f, 1.0f, 0.0));

}

void GUI_Process(GLFWwindow* window,KeyInput& keyinput)
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

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
    ImGui::SliderFloat("SSAO sample radius", &keyinput.SSAO_radius, 0.0f, 10.0f);
    ImGui::SliderFloat("SSAO sample bias", &keyinput.SSAO_bias, 0.0f, 0.01f);
    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
    static int counter = 0;
    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void GUI_Initialize(GLFWwindow* window)
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

void Initialize_Vertex_Arrays(VertexArrays& vertex_arrays, Config& config)
{
    std::string floor_vertex_attr = "floor_vertex_attr";
    std::vector<float> floor_vertex = config.ReadVector(floor_vertex_attr);
    VertexBuffer floorVb(&floor_vertex[0], floor_vertex.size() * sizeof(float));
    VertexBufferLayout floorLayout;
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(2);
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(3);
    vertex_arrays.floorVa.AddBuffer(floorVb, floorLayout);
    //sphere
    sphere_data sphere = SphereData();
    IndexBuffer sphereIb(&sphere.index[0], sphere.index.size());
    VertexBuffer sphereVb(&sphere.vertex[0], sizeof(float) * sphere.vertex.size());
    VertexBufferLayout sphereLayout;
    sphereLayout.Push<float>(3);//position
    sphereLayout.Push<float>(3);//normal
    sphereLayout.Push<float>(2);//texcoord
    vertex_arrays.sphereVa.AddBuffer(sphereVb, sphereLayout);
    //quad
    std::string quad_vertex_attr = "quad_vertex_attr";
    std::vector<float> quad_vertex = config.ReadVector(quad_vertex_attr);
    VertexBuffer quadVb(&quad_vertex[0], quad_vertex.size() * sizeof(float));
    VertexBufferLayout quadLayout;
    quadLayout.Push<float>(3);
    quadLayout.Push<float>(2);
    vertex_arrays.quadVa.AddBuffer(quadVb, quadLayout);
    //��պ�
    std::string skybox_vertex_attr = "skybox_vertex_attr";
    std::vector<float> skybox_vertex = config.ReadVector(skybox_vertex_attr);
    VertexBuffer skyboxVb(&skybox_vertex[0], skybox_vertex.size() * sizeof(float));
    VertexBufferLayout skyboxLayout;
    skyboxLayout.Push<float>(3);
    vertex_arrays.skyboxVa.AddBuffer(skyboxVb, skyboxLayout);
    //���Դ����
    std::string cube_vertex_attr = "cube_vertex_attr";
    std::vector<float> cube_vertex = config.ReadVector(cube_vertex_attr);
    VertexBuffer cubeVb(&cube_vertex[0], cube_vertex.size() * sizeof(float));
    VertexBufferLayout cubeLayout;
    cubeLayout.Push<float>(3);
    cubeLayout.Push<float>(3);
    cubeLayout.Push<float>(2);
    vertex_arrays.cubeVa.AddBuffer(cubeVb, cubeLayout);

}