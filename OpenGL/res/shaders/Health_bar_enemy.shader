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
//uniform float screenHeight;

uniform float life_x;
uniform float life_y;
float life_x_left = life_x - 50;
float life_x_right = life_x + 50;
void main()
{
    bool is_life = gl_FragCoord.y > life_y && gl_FragCoord.y < (life_y + 10);
    if (gl_FragCoord.x < life_x_right && gl_FragCoord.x>life_x_left && is_life)
        FragColor = vec4(vec3(1.0, 0.0, 0.0), 1.0);
    else
        discard;
}
