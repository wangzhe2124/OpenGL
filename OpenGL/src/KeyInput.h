#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>
class KeyInput
{
private:
    
public:
    bool blinn_phong;
    bool TorchOn;
    bool gamma;
    float exposure;
    float objectColor;
    float metallic;
    float roughness;
    float SunIntensity;
    glm::vec3 SunColor;
    bool useheight;
    bool NormalMap;
    bool useSSAO;
    bool ui;
    bool blur_shadow;
    bool EnvLight_spec;
    float SSAO_bias;
    float SSAO_radius;
    float SSAO_rangecheck;
    bool SSAO_window;
    bool assist_screen;
    KeyInput();
    void ProcessKey(GLFWwindow* window, int key, int action);
    void ProcessMovement(GLFWwindow *window, Camera& camera, float deltaTime);
};
KeyInput::KeyInput() 
    :blinn_phong(true),
    TorchOn(false),
    gamma(true),
    useheight(true),exposure(1.0f),
    useSSAO(true),
    ui(false),
    NormalMap(true),
    objectColor(0.5f),
    metallic(0.5f),
    roughness(0.5f),
    SunIntensity(10.0f),
    SunColor(1.0f),
    blur_shadow(true),
    EnvLight_spec(true),
    SSAO_bias(0.3),
    SSAO_radius(0.2),
    SSAO_rangecheck(0.446),
    SSAO_window(false),
    assist_screen(false)
{}
void KeyInput::ProcessMovement(GLFWwindow* window, Camera& camera, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        exposure += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        exposure -= 0.01;
        if (exposure < 0)
            exposure = 0.0;
    }
}
void KeyInput::ProcessKey(GLFWwindow* window, int key, int action)
{

    //按B切换blinn_phong和phong
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        blinn_phong = !blinn_phong;
        std::cout << "lighting: " << (blinn_phong ? "blinn_phong" : "phong") << std::endl;
    }

    //按T开关手电筒
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        TorchOn = !TorchOn;
        std::cout << "torch : " << (TorchOn ? "on" : "off") << std::endl;
    }

    //按G切换gamma校正
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        gamma = !gamma;
        std::cout << "gamma校正 : " << (gamma ? "on" : "off") << std::endl;
    }

    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        useheight = !useheight;
        std::cout << "heightmap : " << (useheight ? "on" : "off") << std::endl;
    }
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        useSSAO = !useSSAO;
        std::cout << "SSAO : " << (useSSAO ? "on" : "off") << std::endl;
    }
    if (key == GLFW_KEY_U && action == GLFW_PRESS)
    {
        ui = !ui;
        std::cout << "UI : " << (ui ? "on" : "off") << std::endl;
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        NormalMap = !NormalMap;
        std::cout << "NormalMap : " << (NormalMap ? "on" : "off") << std::endl;
    }

}


