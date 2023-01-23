#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>
class KeyInput
{
private:
    
public:
    void UpDown2Alpha(GLFWwindow* window, float& alpha);
    void ProcessKey(GLFWwindow* window, Camera& camera, float deltaTime);
};

void KeyInput::UpDown2Alpha(GLFWwindow* window, float& alpha)
{
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        alpha += 0.01f;
        alpha = alpha < 1 ? alpha : 1;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        alpha -= 0.01f;
        alpha = alpha > 0 ? alpha : 0.0f;
    }
}

void KeyInput::ProcessKey(GLFWwindow* window, Camera& camera, float deltaTime)
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

}


