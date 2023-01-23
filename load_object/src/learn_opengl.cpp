#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include "Debugger.h"

#include "Shader.h"

#include "KeyInput.h"

#include "Model.h"
#include "ModelSpace.h"
#include "Camera.h"
//鼠标回调
unsigned int screenWidth = 960;
unsigned int screenHeight = 640;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
float deltaTime = 0.0f;	// 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
//创建相机
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//创建模型空间

//键盘输入
KeyInput keyinput;

int main(void)
{


    /* Initialize the library */
    if (!glfwInit())
        return -1;

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
    //深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //模板测试
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //鼠标回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    Shader shader("res/shaders/basic.shader");
    Shader cubeshader("res/shaders/cube.shader");
    Shader tencil("res/shaders/single.shader");
     //投影矩阵
    glm::mat4 projection = glm::mat4(1.0f);

    shader.UnBind();
    cubeshader.UnBind();

    /* Loop until the user closes the window */

    glm::vec3 pointLightPositions[] = {
glm::vec3(0.7f,  0.2f,  2.0f),
glm::vec3(2.3f, -3.3f, -4.0f),
glm::vec3(-4.0f,  2.0f, -12.0f),
glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec3 pointLightColors[] = {
     glm::vec3(0.2f, 0.2f, 0.6f),
     glm::vec3(0.3f, 0.3f, 0.7f),
     glm::vec3(0.0f, 0.0f, 0.3f),
     glm::vec3(0.4f, 0.4f, 0.4f)
    };
    Model ourModel("res/objects/nanosuit/nanosuit.obj");
    Model Marry("res/objects/Marry/Marry.obj");
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* Render here */

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);//深度，模板测试
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有的片段都应该更新模板缓冲
        glStencilMask(0xFF); // 启用模板缓冲写入

        cubeshader.Bind();
        cubeshader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), 0.1f, 100.0f);
        cubeshader.SetUniformmatri4fv("projection", projection);
        ModelSpace nano_position;
        cubeshader.SetUniformmatri4fv("model", nano_position.GetModelSpace());
        cubeshader.SetUniform3f("objectColor", 0.2f, 0.2f, 0.31f);
        cubeshader.SetUniform1f("material.shininess", 64.0f);
        cubeshader.SetUniform3f("viewPos", camera.Position);
        //平行光
        cubeshader.SetUniform3f("dirlight.ambient", 0.05f, 0.05f, 0.1f);
        cubeshader.SetUniform3f("dirlight.diffuse", 0.2f, 0.2f, 0.7f);
        cubeshader.SetUniform3f("dirlight.specular", 0.7f, 0.7f, 0.7f);
        cubeshader.SetUniform3f("dirlight.direction", -0.2f, -1.0f, -0.3f);
        cubeshader.SetUniform1f("dirlight.LightIntensity", 10.0f);
        //点光
        cubeshader.SetUniform3f("pointlight[0].ambient", pointLightColors[0]);
        cubeshader.SetUniform3f("pointlight[0].diffuse", pointLightColors[0]);
        cubeshader.SetUniform3f("pointlight[0].specular", pointLightColors[0]);
        cubeshader.SetUniform3f("pointlight[0].position", pointLightPositions[0]);
        cubeshader.SetUniform1f("pointlight[0].LightIntensity", 10.0f);
        cubeshader.SetUniform1f("pointlight[0].constant", 1.0f);
        cubeshader.SetUniform1f("pointlight[0].linear", 0.14f);
        cubeshader.SetUniform1f("pointlight[0].quadratic", 0.07f);

        cubeshader.SetUniform3f("pointlight[1].ambient", pointLightColors[1]);
        cubeshader.SetUniform3f("pointlight[1].diffuse", pointLightColors[1]);
        cubeshader.SetUniform3f("pointlight[1].specular", pointLightColors[1]);
        cubeshader.SetUniform3f("pointlight[1].position", pointLightPositions[1]);
        cubeshader.SetUniform1f("pointlight[1].LightIntensity", 10.0f);
        cubeshader.SetUniform1f("pointlight[1].constant", 1.0f);
        cubeshader.SetUniform1f("pointlight[1].linear", 0.14f);
        cubeshader.SetUniform1f("pointlight[1].quadratic", 0.07f);

        cubeshader.SetUniform3f("pointlight[2].ambient", pointLightColors[2]);
        cubeshader.SetUniform3f("pointlight[2].diffuse", pointLightColors[2]);
        cubeshader.SetUniform3f("pointlight[2].specular", pointLightColors[2]);
        cubeshader.SetUniform3f("pointlight[2].position", pointLightPositions[2]);
        cubeshader.SetUniform1f("pointlight[2].LightIntensity", 10.0f);
        cubeshader.SetUniform1f("pointlight[2].constant", 1.0f);
        cubeshader.SetUniform1f("pointlight[2].linear", 0.14f);
        cubeshader.SetUniform1f("pointlight[2].quadratic", 0.07f);

        cubeshader.SetUniform3f("pointlight[3].ambient", pointLightColors[3]);
        cubeshader.SetUniform3f("pointlight[3].diffuse", pointLightColors[3]);
        cubeshader.SetUniform3f("pointlight[3].specular", pointLightColors[3]);
        cubeshader.SetUniform3f("pointlight[3].position", pointLightPositions[3]);
        cubeshader.SetUniform1f("pointlight[3].LightIntensity", 10.0f);
        cubeshader.SetUniform1f("pointlight[3].constant", 1.0f);
        cubeshader.SetUniform1f("pointlight[3].linear", 0.14f);
        cubeshader.SetUniform1f("pointlight[3].quadratic", 0.07f);
        //聚光手电
        cubeshader.SetUniform3f("spotlight.ambient", 0.0f, 0.0f, 0.0f);
        cubeshader.SetUniform3f("spotlight.diffuse", 1.0f, 1.0f, 1.0f);
        cubeshader.SetUniform3f("spotlight.specular", 1.0f, 1.0f, 1.0f);
        cubeshader.SetUniform3f("spotlight.direction", camera.Front);
        cubeshader.SetUniform3f("spotlight.position", camera.Position);
        cubeshader.SetUniform1f("spotlight.constant", 1.0f);
        cubeshader.SetUniform1f("spotlight.linear", 0.009f);
        cubeshader.SetUniform1f("spotlight.quadratic", 0.0032f);

        cubeshader.SetUniform1f("spotlight.LightIntensity", 1.0f);
        cubeshader.SetUniform1f("spotlight.inner_CutOff", glm::cos(glm::radians(10.5f)));//spotlight范围
        cubeshader.SetUniform1f("spotlight.outer_CutOff", glm::cos(glm::radians(12.5f)));//spotlight范围

        ourModel.Draw(cubeshader);
        ModelSpace Marry_position = nano_position;
        Marry_position.Translate(glm::vec3(5.0f, 0.0f, 0.0f));
        cubeshader.SetUniformmatri4fv("model", Marry_position.GetModelSpace());
        Marry.Draw(cubeshader);
        //使用模板缓冲制作边框
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); // 禁止模板缓冲的写入
        glDisable(GL_DEPTH_TEST);
        tencil.Bind();
        tencil.SetUniformmatri4fv("view", camera.GetViewMatrix());
        tencil.SetUniformmatri4fv("projection", projection);
        Marry_position.Scale(glm::vec3(1.01f, 1.01f, 1.02f));
        tencil.SetUniformmatri4fv("model", Marry_position.GetModelSpace());
        Marry.Draw(tencil);
        nano_position.Scale(glm::vec3(1.01f, 1.01f, 1.02f));
        tencil.SetUniformmatri4fv("model", nano_position.GetModelSpace());

        
        ourModel.Draw(tencil);
        glStencilMask(0xFF);
        glEnable(GL_DEPTH_TEST);
        //keyinput.UpDown2Alpha(window, alpha);
        //shader.SetUniform1f("AlphaValue", alpha);
        keyinput.ProcessKey(window, camera, deltaTime);//键盘输入

        //renderer.DrawElement(va, ib, shader);

        //renderer.ClearColor(0.4f, 0.4f, 0.2f, 0.2f);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);//储存着GLFW窗口每一个像素颜色值的大缓冲（双缓冲）

        /* Poll for and process events */
        glfwPollEvents();//触发键盘鼠标输入事件
    }
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