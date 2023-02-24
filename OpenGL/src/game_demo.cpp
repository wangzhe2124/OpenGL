#pragma once

#include "Game.h"
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void windowSize_callback(GLFWwindow* window, int cx, int cy);
void GUI_Initialize(GLFWwindow* window);

void GUI_Process(GLFWwindow* window, KeyInput& keyinput);
using namespace irrklang;

ISoundEngine* SoundEngine = createIrrKlangDevice();
//鼠标回调
unsigned int screenWidth = 960;
unsigned int screenHeight = 640;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;	// 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
Game game;
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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(game.GetSwidth(), game.GetSheight(), "demo", NULL, NULL);//指针
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
    glCheckError();
    std::cout << glGetString(GL_VERSION) << std::endl;

    //鼠标回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keys_callback);
    glfwSetWindowSizeCallback(window, windowSize_callback);
    // Setup Dear ImGui context
    GUI_Initialize(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//锁定鼠标
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
    //glEnable(GL_MULTISAMPLE);
    /* Loop until the user closes the window */
    //gamma校正
    //glEnable(GL_FRAMEBUFFER_SRGB); 
    game.ready_render();
    SoundEngine->play2D("res/music/girl.mp3", GL_TRUE);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        game.GetDeltaTime(deltaTime);
        game.keyinput.ProcessMovement(window, game.camera, deltaTime, game.my_state.current_energy);//键盘输入移动
        game.start_render();
        if (game.keyinput.full_screen)
        {
            GLFWmonitor* _monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(_monitor);
            glfwSetWindowMonitor(window, _monitor, 0, 0, mode->width, mode->height, 0);
            game.SetSwidth(mode->width);
            game.SetSheight(mode->height);
        }
        else
        {
            glm::ivec2 window_pos;
            glfwGetWindowPos(window, &window_pos.x, &window_pos.y);
            glm::ivec2 window_size;
            glfwGetWindowSize(window, &window_size.x, &window_size.y);
            glfwSetWindowMonitor(window, nullptr, 0, 0, 960, 640, 0);
            game.SetSwidth(960);
            game.SetSheight(640);
        }
        //glDisable(GL_DEPTH_TEST);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //game.shaders->basicscreen_shader.Bind();
        //glBindTexture(GL_TEXTURE_2D, 0);
        ////cameradepthFBO.BindTexture();
        ////blooming_hightlightFBO.BindTexture(0,1);
        ////blooming_blur_verticalFBO.BindTexture();
        ////game.framebuffers->hdrfbo.BindTexture(0);
        ////global_dirshadowfbo.BindTexture();
        ////shadow_blur_verticalFBO.BindTexture();
        ////game.framebuffers->FXAA_FBO.BindTexture(0);
        ////game.framebuffers->gbuffer.BindTexture(0, 1);
        //game.framebuffers->csm_mapFBO[1].BindTexture();
        ////envcubemap_spec_BRDF_FBO.BindTexture();
        ////ssaoFBO.BindTexture();
        ////SpotlightMapfbo.BindTexture();
        ////glBindTexture(GL_TEXTURE_2D, Characters[10].TextureID);
        //game.renderer.DrawArray(game.vertex_arrays->quadVa, game.shaders->basicscreen_shader);
               

        /* Poll for and process events */
        glfwPollEvents();//触发键盘鼠标输入事件
        //GUI
        GUI_Process(window, game.keyinput);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);//储存着GLFW窗口每一个像素颜色值的大缓冲（双缓冲）
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
    if(!game.keyinput.ui)
        game.camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    game.camera.ProcessMouseScroll(static_cast<float>(yoffset));
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
    game.keyinput.ProcessKey(window, key, action);
}
void windowSize_callback(GLFWwindow* window, int cx, int cy)
{
    std::cout << "called" << std::endl;
    game.ResetResolution(cx, cy);
}
void GUI_Process(GLFWwindow* window, KeyInput& keyinput)
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
          // Edit bools storing our window open/close state
    ImGui::Checkbox("Gamma", &keyinput.gamma);
    ImGui::Checkbox("use terrain", &keyinput.use_terrain);
    ImGui::Checkbox("HeightMap", &keyinput.useheight);
    ImGui::Checkbox("NormalMap", &keyinput.NormalMap);
    ImGui::Checkbox("use_EnvLight_spec", &keyinput.EnvLight_spec);
    ImGui::Checkbox("third_view", &keyinput.third_view);
    ImGui::Checkbox("free_view", &keyinput.free_view);
    ImGui::Checkbox("blur shadow", &keyinput.blur_shadow);
    ImGui::Checkbox("show mesh", &keyinput.show_mesh);
    ImGui::Checkbox("fullscreen", &keyinput.full_screen);

    ImGui::SliderFloat("camera speed", &game.camera.MovementSpeed, 0.0f, 1000.0f);

    ImGui::SliderFloat("Metallic", &keyinput.metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("Roughness", &keyinput.roughness, 0.0f, 1.0f);
    
    
    ImGui::SliderFloat("exposure", &keyinput.exposure, 0.0f, 100.0f);
    ImGui::SliderInt("tess level", &keyinput.tess_level, 0, 100);
    //sun
    ImGui::Checkbox("sun Window", &keyinput.sun_window);
    if (keyinput.sun_window)
    {
        ImGui::Begin("sun Window", &keyinput.sun_window);
        if (ImGui::Button("Close Me"))
            keyinput.sun_window = false;
        ImGui::SliderFloat("SunIntensity", &keyinput.SunIntensity, 0.0f, 10.0f);
        ImGui::SliderFloat("sm_bias", &keyinput.sun_sm_bias, 0.0f, 5.0f);
        ImGui::SliderFloat("sun speed", &keyinput.sun_speed, 0.0f, 50.0f);
        ImGui::SliderFloat("sun pcf radius", &keyinput.sun_pcf_radius, 0.0f, 10.0f);
        ImGui::Checkbox("sun pcf", &keyinput.sun_pcf);

        ImGui::ColorEdit3("sun color", (float*)&keyinput.SunColor); // Edit 3 floats representing a color
        ImGui::End();
    }
    ImGui::SameLine();
    //pointlight
    ImGui::Checkbox("pointlight Window", &keyinput.pointlight_window);
    if (keyinput.pointlight_window)
    {
        ImGui::Begin("pointlight Window", &keyinput.pointlight_window);
        if (ImGui::Button("Close Me"))
            keyinput.pointlight_window = false;
        ImGui::SliderFloat("pt_sm_radius", &keyinput.point_sm_radius, 0.0f, 0.01f);
        ImGui::SliderFloat("pt_intensity", &keyinput.pointlight_Intensity, 0.0f, 50.0f);
        ImGui::SliderFloat("pt_far_plane", &keyinput.point_far_plane, 0.0f, 100.0f);
        ImGui::Checkbox("pt_sm_pcf", &keyinput.point_sm_pcf);
        ImGui::ColorEdit3("pt_color", (float*)&keyinput.point_color);
        ImGui::End();
    }
    ImGui::SameLine();
    //spotlight
    ImGui::Checkbox("spotlight Window", &keyinput.spotlight_window);
    if (keyinput.spotlight_window)
    {
        ImGui::Begin("spot Window", &keyinput.spotlight_window);
        if (ImGui::Button("Close Me"))
            keyinput.spotlight_window = false;
        ImGui::Checkbox("Torch", &keyinput.TorchOn);
        ImGui::SliderFloat("bias_x", &keyinput.st_bias_x, -5.0f, 5.0f);
        ImGui::SliderFloat("bias_y", &keyinput.st_bias_y, -5.0f, 5.0f);
        ImGui::SliderFloat("bias_z", &keyinput.st_bias_z, 0.0f, 5.0f);
        ImGui::SliderFloat("st_far_plane", &keyinput.spot_far_plane, 5.0f, 50.0f);
        ImGui::SliderFloat("torch intensity", &keyinput.torch_intensity, 0.0f, 50.0f);
        ImGui::ColorEdit3("torch color", (float*)&keyinput.torch_color);

        ImGui::End();
    }
    //bloom
    ImGui::Checkbox("bloom Window", &keyinput.bloom_window);
    if (keyinput.bloom_window)
    {
        ImGui::Begin("bloom Window", &keyinput.bloom_window);
        if (ImGui::Button("Close Me"))
            keyinput.bloom_window = false;
        ImGui::SliderInt("bloom times", &keyinput.bloom_times, 0, 16);
        ImGui::SliderFloat("bloom halox", &keyinput.bloom_halox, 0.0f, 1.5f);
        ImGui::SliderFloat("bloom haloy", &keyinput.bloom_haloy, 0.0f, 1.5f);
        ImGui::SliderFloat("bloom haloz", &keyinput.bloom_haloz, 0.0f, 1.5f);
        ImGui::End();
    }
    ImGui::Checkbox("assist screen", &keyinput.assist_screen);
    //ssao
    ImGui::Checkbox("SSAO Window", &keyinput.SSAO_window);
    if (keyinput.SSAO_window)
    {
        ImGui::Begin("SSAO Window", &keyinput.SSAO_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::Button("Close Me"))
            keyinput.SSAO_window = false;
        ImGui::Checkbox("SSAO", &keyinput.useSSAO);
        ImGui::SliderFloat("SSAO sample radius", &keyinput.SSAO_radius, 0.0f, 10.0f);
        ImGui::SliderFloat("SSAO sample bias", &keyinput.SSAO_bias, 0.0f, 5.0f);
        ImGui::SliderFloat("SSAO sample rangecheck", &keyinput.SSAO_rangecheck, 0.0f, 1.0f);
        ImGui::End();
    }
    ImGui::SameLine();
    //particle
    ImGui::Checkbox("particle Window", &keyinput.particle_window);
    if (keyinput.particle_window)
    {
        ImGui::Begin("particle Window", &keyinput.particle_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::Button("Close Me"))
            keyinput.particle_window = false;
        ImGui::Checkbox("2d particle", &keyinput.show_particle);
        ImGui::Checkbox("3d particle", &keyinput.show_d3particle);
        ImGui::End();
    }
    ImGui::SameLine();
    //fxaa
    ImGui::Checkbox("FXAA Window", &keyinput.fxaa_window);
    if (keyinput.fxaa_window)
    {
        ImGui::Begin("particle Window", &keyinput.fxaa_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::Button("Close Me"))
            keyinput.fxaa_window = false;
        ImGui::Checkbox("fxaa on", &keyinput.fxaa_on);
        ImGui::SliderFloat("fxaa lumaThreshold", &keyinput.fxaa_lumaThreshold, 0.0f, 1.0f);
        ImGui::SliderFloat("fxaa mulReduce", &keyinput.fxaa_mulReduce, 0.0f, 1.0f);
        ImGui::SliderFloat("fxaa minReduce", &keyinput.fxaa_minReduce, 0.0f, 1.0f);
        ImGui::SliderFloat("fxaa maxSpan", &keyinput.fxaa_maxSpan, 0.0f, 16.0f);
        ImGui::End();
    }
    static int counter = 0;
    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    if (keyinput.ui)
    {
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//解锁鼠标
        firstMouse = true;
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//锁定鼠标
    }
    ImGui::EndFrame();
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

