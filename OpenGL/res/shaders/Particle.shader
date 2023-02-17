#shader vertex
#version 420 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 Texcoord;

out vec2 fragTexcoord;
uniform mat4 projection;
uniform vec2 offset;
void main()
{
    float scale = 1.0f;
    gl_Position = projection * vec4(position.xy * scale + offset, 0.0f, 1.0f);
    fragTexcoord = Texcoord;
};
#shader fragment
#version 420 core
out vec4 FragColor;
in vec2 fragTexcoord;
uniform vec4 color;
uniform sampler2D Texture;
void main()
{
    FragColor = vec4(vec3(texture(Texture, fragTexcoord) * color), 0.5f);
};