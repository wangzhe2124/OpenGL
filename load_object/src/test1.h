#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>    
#include<fstream>
#include <string>
#include <sstream>
#include <vector>

#include "Debugger.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"
#include "KeyInput.h"
#include "Texture.h"

#include "ModelSpace.h"
#include "Camera.h"
//���ص�
unsigned int screenWidth = 640;
unsigned int screenHeight = 480;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
float deltaTime = 0.0f;	// ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
//�������
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//����ģ�Ϳռ�
ModelSpace model;
//��������
KeyInput keyinput;

int main(void)
{


    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Hello World", NULL, NULL);//ָ��


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

    glEnable(GL_DEPTH_TEST);
    //���ص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    float positions[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f
    };
    unsigned int indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };
    glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    std::vector<glm::mat4> model_position(10, model.GetModelSpace());
    for (int i = 0; i < 10; i++)
    {
        model_position[i] = glm::translate(model_position[i], cubePositions[i]);
    }
    //blender
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
    //�����������飬���㻺�壬��������
    VertexArray va;
    VertexBuffer vb;
    vb.BindData(positions, sizeof(positions));
    IndexBuffer ib;
    ib.BindData(indices, sizeof(indices));
    //��������
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);//�����㻺���붥�������

    //����shader
    Shader shader("res/shaders/source.shader");
    shader.Bind();
    //����Renderer
    Renderer renderer;
    //��������
    Texture texture1;
    texture1.BindData("res/textures/test4.png","s");
    Texture texture2;
    texture2.BindData("res/textures/awesomeface.png","2");
    texture1.Bind(0);
    texture2.Bind(1);
    shader.SetUniform1i("u_Texture1", 0);
    shader.SetUniform1i("u_Texture2", 1);
    //ͶӰ����
    glm::mat4 projection = glm::mat4(1.0f);


    //���
    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.UnBind();

    //Camera

    float alpha = 0.5;
    /* Loop until the user closes the window */
    glm::vec3 cameraposition = glm::vec3(0.0, 0.0, 3.0);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//�������

    while (!glfwWindowShouldClose(window))
    {
        shader.Bind();
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* Render here */
        renderer.Clear(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//��Ȳ���
        shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), 0.1f, 100.0f);
        shader.SetUniformmatri4fv("projection", projection);

        for (unsigned int i = 0; i < 10; i++)
        {
            model.SetModel(model_position[i]);
            float angle = 0.02f * (i + 1);
            model.Rotate(angle, glm::vec3(1.0f, 0.3f, 0.5f));
            model_position[i] = model.GetModelSpace();
            shader.SetUniformmatri4fv("model", model.GetModelSpace());
            renderer.DrawArray(va, shader);
        }

        keyinput.ProcessKey(window, camera, deltaTime);//��������
        keyinput.UpDown2Alpha(window, alpha);
        shader.SetUniform1f("AlphaValue", alpha);

        //renderer.DrawElement(va, ib, shader);
        //renderer.DrawArray(va, ib, shader);

        renderer.ClearColor(0.4f, 0.4f, 0.2f, 0.2f);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);//������GLFW����ÿһ��������ɫֵ�Ĵ󻺳壨˫���壩

        /* Poll for and process events */
        glfwPollEvents();//����������������¼�
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