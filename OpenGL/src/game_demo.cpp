#pragma once

#include "Game.h"
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void GUI_Initialize(GLFWwindow* window);

void GUI_Process(GLFWwindow* window, KeyInput& keyinput);

//鼠标回调
unsigned int screenWidth = 960;
unsigned int screenHeight = 640;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;	// 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
Game game;
int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "demo", NULL, NULL);//指针
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

    // Setup Dear ImGui context
    GUI_Initialize(window);

    //鼠标回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keys_callback);

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
    glEnable(GL_MULTISAMPLE);
    /* Loop until the user closes the window */
    //gamma校正
    //glEnable(GL_FRAMEBUFFER_SRGB); 
    game.ready_render();
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        game.keyinput.ProcessMovement(window, game.camera, deltaTime);//键盘输入移动
        game.start_render();
        //glDisable(GL_DEPTH_TEST);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //game.shaders->basicscreen_shader.Bind();
        //glBindTexture(GL_TEXTURE_2D, 0);
        ////cameradepthFBO.BindTexture();
        ////blooming_hightlightFBO.BindTexture(0,1);
        ////blooming_blur_verticalFBO.BindTexture();
        ////hdrfbo.BindTexture(0,2);
        ////global_dirshadowfbo.BindTexture();
        ////shadow_blur_verticalFBO.BindTexture();
        //game.framebuffers->PreShadowFBO.BindTexture(0, 0);
        ////game.framebuffers->csm_mapFBO[0].BindTexture();
        ////envcubemap_spec_BRDF_FBO.BindTexture();
        ////ssaoFBO.BindTexture();
        ////SpotlightMapfbo.BindTexture();
        ////glBindTexture(GL_TEXTURE_2D, Characters[10].TextureID);
        //game.renderer.DrawArray(game.vertex_arrays->quadVa, game.shaders->basicscreen_shader);
        //       
             //GUI
        GUI_Process(window, game.keyinput);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);//储存着GLFW窗口每一个像素颜色值的大缓冲（双缓冲）
        /* Poll for and process events */
        glfwPollEvents();//触发键盘鼠标输入事件
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
void GUI_Process(GLFWwindow* window, KeyInput& keyinput)
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
    ImGui::Checkbox("use_EnvLight_spec", &keyinput.EnvLight_spec);
    ImGui::Checkbox("third_view", &keyinput.third_view);
    ImGui::SliderFloat("ObjectColor", &keyinput.objectColor, 0.0f, 1.0f);
    ImGui::SliderFloat("Metallic", &keyinput.metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("Roughness", &keyinput.roughness, 0.0f, 1.0f);
    //ImGui::SliderFloat("LightColor", &keyinput.SunColor, 0.0f, 100.0f);
    ImGui::SliderFloat("LightIntensity", &keyinput.SunIntensity, 0.0f, 50.0f);
    ImGui::SliderFloat("exposure", &keyinput.exposure, 0.0f, 100.0f);
    ImGui::Checkbox("assist screen", &keyinput.assist_screen);

    ImGui::Checkbox("SSAO Window", &keyinput.SSAO_window);
    if (keyinput.SSAO_window)
    {
        ImGui::Begin("SSAO Window", &keyinput.SSAO_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            keyinput.SSAO_window = false;
        ImGui::Checkbox("SSAO", &keyinput.useSSAO);
        ImGui::SliderFloat("SSAO sample radius", &keyinput.SSAO_radius, 0.0f, 10.0f);
        ImGui::SliderFloat("SSAO sample bias", &keyinput.SSAO_bias, 0.0f, 1.0f);
        ImGui::SliderFloat("SSAO sample rangecheck", &keyinput.SSAO_rangecheck, 0.0f, 10.0f);
        ImGui::End();
    }

    ImGui::ColorEdit3("clear color", (float*)&keyinput.SunColor); // Edit 3 floats representing a color
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
        glViewport(0, 0, screenWidth, screenWidth);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//解锁鼠标
        glfwSetCursorPosCallback(window, nullptr);
        firstMouse = true;
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//锁定鼠标
        glfwSetCursorPosCallback(window, mouse_callback);
    }
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

