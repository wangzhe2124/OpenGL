#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <queue>
#include <unordered_map>
#include <string>
namespace Chord_Key
{
    struct key_element
    {
        int key;
        float time;
    };
}

class KeyInput
{
private:
    std::queue<Chord_Key::key_element> sequence_k;
    std::unordered_map<std::string, int> control_map;
public:
    bool keysPressed[1024];
    bool keysReleased[1024] = { true };
    bool blinn_phong;
    
    bool gamma;
    float exposure;
    float metallic;
    float roughness;
    bool use_terrain;
    bool useheight;
    bool NormalMap;
    bool useSSAO;
    bool ui;
    bool full_screen;
    bool blur_shadow;
    bool EnvLight_spec;
    float SSAO_bias;
    float SSAO_radius;
    float SSAO_rangecheck;
    bool SSAO_window;
    bool assist_screen;
    int tess_level;
    bool third_view; bool free_view; bool show_mesh;
    //animation
    bool animation_window; bool chage_animation; int animation_type;
    //bloom
    bool bloom_window; int bloom_times; float bloom_halox; float bloom_haloy; float bloom_haloz; float bloom_edge;
    //sun
    bool sun_window; float SunIntensity; glm::vec3 SunColor; float sun_sm_bias; float sun_speed; bool sun_pcf; float sun_pcf_radius;
    bool spotlight_window; float st_bias_x; float st_bias_y; float st_bias_z; float spot_far_plane; bool TorchOn; glm::vec3 torch_color; float torch_intensity;
    bool pointlight_window; float point_sm_radius; bool point_sm_pcf; float pointlight_Intensity; float point_far_plane; glm::vec3 point_color;
    //particle
    bool particle_window; bool show_particle; float particle_scale; float particle_offset; int new_particle_num; float particle_vel; float particle_life_reduce; bool show_d3particle;
    bool fxaa_window; bool fxaa_on; float fxaa_lumaThreshold; float fxaa_mulReduce; float fxaa_minReduce; float fxaa_maxSpan;
    KeyInput();
    void RecordKey(int key, int action);
    void ProcessKey(GLFWwindow* window, int key, int action);
    void ProcessMovement(GLFWwindow *window, Camera& camera, float deltaTime, float& energy);
};


