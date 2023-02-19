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
    float metallic;
    float roughness;
    
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
    int tess_level;
    bool third_view; bool free_view; bool show_mesh;
    bool sun_window; float SunIntensity; glm::vec3 SunColor;
    bool spotlight_window; float st_bias_x; float st_bias_y; float st_bias_z; float spot_far_plane;
    bool pointlight_window; float point_sm_radius; bool point_sm_pcf; float pointlight_Intensity; float point_far_plane; glm::vec3 point_color;
    bool particle_window; bool show_particle; bool show_d3particle;
    bool fxaa_window; bool fxaa_on; float fxaa_lumaThreshold; float fxaa_mulReduce; float fxaa_minReduce; float fxaa_maxSpan;
    KeyInput();
    void ProcessKey(GLFWwindow* window, int key, int action);
    void ProcessMovement(GLFWwindow *window, Camera& camera, float deltaTime, float& energy);
};
KeyInput::KeyInput()
    :blinn_phong(true),
    TorchOn(false),
    gamma(true),
    useheight(true), exposure(2.0f),
    useSSAO(true),
    ui(false),
    NormalMap(true),
    metallic(0.5f),
    roughness(0.8f),
    
    
    blur_shadow(true),
    EnvLight_spec(true),
    SSAO_window(false), SSAO_bias(0.3f), SSAO_radius(0.25f), SSAO_rangecheck(0.446f),
    assist_screen(false),
    tess_level(1),
    third_view(true), free_view(false),show_mesh(false),
    sun_window(false), SunIntensity(3.0f), SunColor(1.0f),
    spotlight_window(false), st_bias_x(0.15f), st_bias_y(0.15f), st_bias_z(0.15f), spot_far_plane(20.0f),
    pointlight_window(false), point_sm_radius(0.001f), point_sm_pcf(false), pointlight_Intensity(0.0f),point_far_plane(10.0f), point_color(1.0f),
    particle_window(false),show_particle(false), show_d3particle(false),
    fxaa_window(false), fxaa_on(false), fxaa_lumaThreshold(0.5f), fxaa_mulReduce(0.125f), fxaa_minReduce(0.001f), fxaa_maxSpan(8.0f)
{}
void KeyInput::ProcessMovement(GLFWwindow* window, Camera& camera, float deltaTime, float& energy)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera.ProcessKeyboard(FORWARD_LEFT, deltaTime);
            camera.character_Front = FORWARD_LEFT;
            camera.is_move = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera.ProcessKeyboard(FORWARD_RIGHT, deltaTime);
            camera.character_Front = FORWARD_RIGHT;
            camera.is_move = true;
        }
        else
        {
            camera.ProcessKeyboard(FORWARD, deltaTime);
            camera.character_Front = FORWARD;
            camera.is_move = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera.ProcessKeyboard(BACKWARD_LEFT, deltaTime);
            camera.character_Front = BACKWARD_LEFT;
            camera.is_move = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera.ProcessKeyboard(BACKWARD_RIGHT, deltaTime);
            camera.character_Front = BACKWARD_RIGHT;
            camera.is_move = true;
        }
        else
        {
            camera.ProcessKeyboard(BACKWARD, deltaTime);
            camera.character_Front = BACKWARD;
            camera.is_move = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
        camera.character_Front = LEFT;
        camera.is_move = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        camera.character_Front = RIGHT;
        camera.is_move = true;
    }
    else
        camera.is_move = false;
    //dash
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        if (energy > 0)
        {
            camera.dash = true;
            energy -= 0.05f;
            if (energy < 0)
                energy = 0.0f;
        }
        else
            camera.dash = false;

    }
    else
    {
        camera.dash = false;
        energy += 1.0f;
        if (energy > 100.0f)
            energy = 100.0f;
    }
    //exposure
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        exposure += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        exposure -= 0.01f;
        if (exposure < 0.0f)
            exposure = 0.0f;
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


