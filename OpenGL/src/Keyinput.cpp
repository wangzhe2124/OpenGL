#pragma once
#include "KeyInput.h"
#include <iostream>

KeyInput::KeyInput()
    :blinn_phong(true),

    gamma(true),
    exposure(2.0f),
    useSSAO(true),
    ui(false),
    NormalMap(false),
    useheight(false),
    metallic(0.5f),
    roughness(0.8f),
    use_terrain(false),
    //resolution
    full_screen(false), reset_resolution(false),
    blur_shadow(false),
    EnvLight_spec(true),
    //ssao
    SSAO_window(false), SSAO_bias(8.0f), SSAO_radius(0.25f), SSAO_rangecheck(0.446f),
    assist_screen(false),
    tess_level(1),

    third_view(true), free_view(false), show_mesh(false),
    //
    animation_window(false), chage_animation(false), animation_type(0),
    //bloom
    bloom_window(false), bloom_times(8), bloom_halox(0.8f), bloom_haloy(0.8f), bloom_haloz(0.8f), bloom_edge(2.0f), bloom_radius(1),
    //sun
    sun_window(false), SunIntensity(3.0f), SunColor(1.0f), sun_sm_bias(0.4f), sun_speed(0.0f), sun_pcf(true), sun_pcf_radius(1.0f),
    //spotlight
    spotlight_window(false), st_bias_x(0.15f), st_bias_y(0.15f), st_bias_z(0.15f), spot_far_plane(20.0f), TorchOn(false), torch_color(1.0f), torch_intensity(5.0f),
    //pointlight
    pointlight_window(false), point_sm_radius(2.0f), point_sm_pcf(false), pointlight_Intensity(0.0f), point_far_plane(5.0f), point_color(1.0f),
    //particles
    particle_window(false), show_particle(false), particle_scale(30.0f), particle_offset(1.0f), new_particle_num(5), particle_vel(1.0f), particle_life_reduce(0.018f), show_d3particle(false),
    //fxaa
    fxaa_window(false), fxaa_on(true), fxaa_lumaThreshold(0.2f), fxaa_maxReduce(0.125f), fxaa_minReduce(0.001f), fxaa_maxSpan(8.0f),
    //taa
    taa_window(false), taa_on(true), taa_mixWeight(0.1f),
    //mlaa
    mlaa_window(false), mlaa_on(true), mlaa_threShold(0.16f), mlaa_searchNum(86)
{

}
void KeyInput::ProcessMovement(GLFWwindow* window, Camera& camera, float deltaTime, float& energy)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
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
void KeyInput::RecordKey(int key, int action)
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

    float time = static_cast<float>(glfwGetTime());
    Chord_Key::key_element k = { key, time };
    if (sequence_k.empty())
    {
        sequence_k.push(k);
    }
    else
    {
        if (sequence_k.size() > 3)
            sequence_k.pop();
        while (!sequence_k.empty() && (time - sequence_k.front().time) > 1.0f)
        {
            sequence_k.pop();
        }
        sequence_k.push(k);
        //processSequence();
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
