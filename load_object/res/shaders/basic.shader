#shader vertex
#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);

};
#shader fragment
#version 330 core
out vec4 FragColor;
struct Material {
	vec3 color;
};
uniform Material material;
void main()
{
	vec3 result = vec3(1.0) * material.color;
	FragColor = vec4(result, 1.0); // 将向量的四个分量全部设置为1.0
}
