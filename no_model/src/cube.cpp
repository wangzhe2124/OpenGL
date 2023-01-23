#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f
    };

    unsigned int indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  2.0f,  -2.0f),
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

    std::vector<glm::mat4> model_position(10, glm::mat4(1.0f));
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
    VertexBufferLayout layout1;
    layout1.Push<float>(3);
    layout1.Push<float>(3);
    layout1.Push<float>(2);
    va.AddBuffer(vb, layout1);//�����㻺���붥�������

    VertexArray lightva;
    VertexBufferLayout layout2;
    layout2.Push<float>(3);
    layout2.Push<float>(3);
    layout2.Push<float>(2);
    lightva.AddBuffer(vb, layout2);
    //����shader
    Shader shader("res/shaders/basic.shader");
    Shader cubeshader("res/shaders/cube.shader");


    //����Renderer
    Renderer renderer;
    //��������
    Texture texture("res/textures/container2_diffuse.png", "diffuse");
    Texture texture2("res/textures/container2_specular.png", "specular");
    texture.Bind(0);
    texture2.Bind(1);

    cubeshader.SetUniform1i("material.texture_diffuse1", 0);
    cubeshader.SetUniform1i("material.texture_specular1", 1);

    //ͶӰ����
    glm::mat4 projection = glm::mat4(1.0f);

    //���
    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.UnBind();
    cubeshader.UnBind();

    /* Loop until the user closes the window */

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//�������
    //���Դλ��
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
    float alpha = 0.5;
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* Render here */
        renderer.Clear(0);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//��Ȳ���
        shader.Bind();
        shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), 0.1f, 100.0f);
        shader.SetUniformmatri4fv("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            shader.SetUniform3f("material.color", pointLightColors[i]);
            ModelSpace model;
            model.Translate(pointLightPositions[i]);
            model.Scale(glm::vec3(0.5f));
            shader.SetUniformmatri4fv("model", model.GetModelSpace());
            renderer.DrawArray(va, shader);
        }
        shader.UnBind();

        cubeshader.Bind();
        cubeshader.SetUniform3f("objectColor", 0.2f, 0.2f, 0.31f);

        cubeshader.SetUniform1f("material.shininess", 64.0f);
        cubeshader.SetUniform3f("viewPos", camera.Position);
        //ƽ�й�
        cubeshader.SetUniform3f("dirlight.ambient", 0.05f, 0.05f, 0.1f);
        cubeshader.SetUniform3f("dirlight.diffuse", 0.2f, 0.2f, 0.7f);
        cubeshader.SetUniform3f("dirlight.specular", 0.7f, 0.7f, 0.7f);
        cubeshader.SetUniform3f("dirlight.direction", -0.2f, -1.0f, -0.3f);
        cubeshader.SetUniform1f("dirlight.LightIntensity", 1.0f);
        //���
        cubeshader.SetUniform3f("pointlight[0].ambient", pointLightColors[0]);
        cubeshader.SetUniform3f("pointlight[0].diffuse", pointLightColors[0]);
        cubeshader.SetUniform3f("pointlight[0].specular", pointLightColors[0]);
        cubeshader.SetUniform3f("pointlight[0].position", pointLightPositions[0]);
        cubeshader.SetUniform1f("pointlight[0].LightIntensity", 1.0f);
        cubeshader.SetUniform1f("pointlight[0].constant", 1.0f);
        cubeshader.SetUniform1f("pointlight[0].linear", 0.14f);
        cubeshader.SetUniform1f("pointlight[0].quadratic", 0.07f);

        cubeshader.SetUniform3f("pointlight[1].ambient", pointLightColors[1]);
        cubeshader.SetUniform3f("pointlight[1].diffuse", pointLightColors[1]);
        cubeshader.SetUniform3f("pointlight[1].specular", pointLightColors[1]);
        cubeshader.SetUniform3f("pointlight[1].position", pointLightPositions[1]);
        cubeshader.SetUniform1f("pointlight[1].LightIntensity", 1.0f);
        cubeshader.SetUniform1f("pointlight[1].constant", 1.0f);
        cubeshader.SetUniform1f("pointlight[1].linear", 0.14f);
        cubeshader.SetUniform1f("pointlight[1].quadratic", 0.07f);

        cubeshader.SetUniform3f("pointlight[2].ambient", pointLightColors[2]);
        cubeshader.SetUniform3f("pointlight[2].diffuse", pointLightColors[2]);
        cubeshader.SetUniform3f("pointlight[2].specular", pointLightColors[2]);
        cubeshader.SetUniform3f("pointlight[2].position", pointLightPositions[2]);
        cubeshader.SetUniform1f("pointlight[2].LightIntensity", 1.0f);
        cubeshader.SetUniform1f("pointlight[2].constant", 1.0f);
        cubeshader.SetUniform1f("pointlight[2].linear", 0.14f);
        cubeshader.SetUniform1f("pointlight[2].quadratic", 0.07f);

        cubeshader.SetUniform3f("pointlight[3].ambient", pointLightColors[3]);
        cubeshader.SetUniform3f("pointlight[3].diffuse", pointLightColors[3]);
        cubeshader.SetUniform3f("pointlight[3].specular", pointLightColors[3]);
        cubeshader.SetUniform3f("pointlight[3].position", pointLightPositions[3]);
        cubeshader.SetUniform1f("pointlight[3].LightIntensity", 1.0f);
        cubeshader.SetUniform1f("pointlight[3].constant", 1.0f);
        cubeshader.SetUniform1f("pointlight[3].linear", 0.14f);
        cubeshader.SetUniform1f("pointlight[3].quadratic", 0.07f);
        //�۹��ֵ�
        cubeshader.SetUniform3f("spotlight.ambient", 0.0f, 0.0f, 0.0f);
        cubeshader.SetUniform3f("spotlight.diffuse", 1.0f, 1.0f, 1.0f);
        cubeshader.SetUniform3f("spotlight.specular", 1.0f, 1.0f, 1.0f);
        cubeshader.SetUniform3f("spotlight.direction", camera.Front);
        cubeshader.SetUniform3f("spotlight.position", camera.Position);
        cubeshader.SetUniform1f("spotlight.constant", 1.0f);
        cubeshader.SetUniform1f("spotlight.linear", 0.009f);
        cubeshader.SetUniform1f("spotlight.quadratic", 0.0032f);

        cubeshader.SetUniform1f("spotlight.LightIntensity", 10.0f);
        cubeshader.SetUniform1f("spotlight.inner_CutOff", glm::cos(glm::radians(10.5f)));//spotlight��Χ
        cubeshader.SetUniform1f("spotlight.outer_CutOff", glm::cos(glm::radians(12.5f)));//spotlight��Χ


        cubeshader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        cubeshader.SetUniformmatri4fv("projection", projection);
        for (unsigned int i = 0; i < 10; i++)
        {
            ModelSpace cubemodel;
            cubemodel.SetModel(model_position[i]);
            float angle = 20.0f * i;
            cubemodel.Rotate(angle, glm::vec3(1.0f, 0.3f, 0.5f));
            cubeshader.SetUniformmatri4fv("model", cubemodel.GetModelSpace());

            renderer.DrawArray(va, cubeshader);//

        }
        
        //keyinput.UpDown2Alpha(window, alpha);
        //shader.SetUniform1f("AlphaValue", alpha);
        keyinput.ProcessKey(window, camera, deltaTime);//��������
        keyinput.UpDown2Alpha(window, alpha);
        cubeshader.SetUniform1f("alpha", alpha);
        cubeshader.UnBind();
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