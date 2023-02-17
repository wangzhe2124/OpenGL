#shader vertex
#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 Texcoord;

out vec2 fragTexcoord;
void main()
{
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
    fragTexcoord = Texcoord;
};
#shader fragment
#version 450 core
out vec4 FragColor;
in vec2 fragTexcoord;
uniform float screenHeight;

uniform float max_life;
uniform float current_life;
float life_height_down = screenHeight - 40;
float life_height_up = screenHeight - 30;
float energy_height_down = screenHeight - 60;
float energy_height_up = screenHeight - 50;
uniform float max_energy;
uniform float current_energy;
void main()
{
    bool is_life= gl_FragCoord.y > life_height_down && gl_FragCoord.y < life_height_up;
    bool is_energy = gl_FragCoord.y > energy_height_down && gl_FragCoord.y < energy_height_up;
    if (gl_FragCoord.x > 20 && gl_FragCoord.x < current_life && is_life)
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else if (gl_FragCoord.x > current_life && gl_FragCoord.x < max_life && is_life)
        FragColor = vec4(vec3(0), 1.0);
    else if (gl_FragCoord.x > 20 && gl_FragCoord.x < current_energy && is_energy)
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    else if (gl_FragCoord.x > current_energy && gl_FragCoord.x < max_energy && is_energy)
        FragColor = vec4(vec3(0), 1.0);
    else
        discard;
}
