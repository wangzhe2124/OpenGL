#shader vertex
#version 420 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 Normal;
layout(location = 2) in vec2 TexCoord;

uniform mat4 projection;
uniform  mat4 view;
uniform mat4 model;
out VS_OUT
{
vec2 TexCoord;
}vs_out;
void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	vs_out.TexCoord = TexCoord;
};
#shader fragment
#version 420 core
out vec4 FragColor;
in VS_OUT
{
vec2 TexCoord;
}fs_in;
uniform sampler2D Texture;
struct Material {
	vec3 color;
};
uniform Material material;
void main()
{
	vec3 result = vec3(1.0) * material.color;
	//result += vec3(texture(Texture, fs_in.TexCoord));
	FragColor = vec4(result, 1.0); // 将向量的四个分量全部设置为1.0
}
