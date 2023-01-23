#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 Texcoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out vec2 fragTexcoord;
void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	fragTexcoord = Texcoord;

};
#shader fragment
#version 330 core
out vec4 FragColor;
in vec2 fragTexcoord;
uniform sampler2D Texture;
struct Material {
	vec3 color;
};
uniform Material material;
void main()
{
	FragColor = texture(Texture, fragTexcoord); // 将向量的四个分量全部设置为1.0
}
