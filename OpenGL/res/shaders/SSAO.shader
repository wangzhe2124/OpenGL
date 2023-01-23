#shader vertex
#version 420 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;

#define SPLITNUM 4
out VS_OUT{
 vec2 TexCoord;
}vs_out;
void main()
{
	gl_Position = vec4(Position, 1.0f);
	vs_out.TexCoord = TexCoord;
};

#shader fragment
#version 420 core
layout(location = 0) out float color;
in VS_OUT{
 vec2 TexCoord;
}fs_in;
struct Camera
{
	sampler2D Depth;
};
uniform Camera camera;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D Noise;
uniform vec3 samples[64];
uniform mat4 projection;
uniform mat4 view;
const vec2 noiseScale = vec2(960.0 / 4.0, 640.0 / 4.0);
float LinearizeDepth(float depth)
{
	float NEAR = 0.1f;
	float FAR = 100.0f;
	float z = depth * 2.0 - 1.0; // 回到NDC
	return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}
void main()
{
	vec3 FragPos = texture(gPosition, fs_in.TexCoord).xyz;
	vec3 Normal = texture(gNormal, fs_in.TexCoord).rgb;
	float Depth = texture(camera.Depth, fs_in.TexCoord).x;
	vec3 randomVec = texture(Noise, fs_in.TexCoord * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - Normal * dot(randomVec, Normal));//施密特正交化
	vec3 bitangent = cross(Normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, Normal);
	float occlusion = 0.0;
	float radius = 0.2f;
	int kernelSize = 64;
	for (int i = 0; i < kernelSize; ++i)
	{
		// 获取样本位置
		vec3 samp = TBN * samples[i]; // 
		vec3 fragpos = (view * vec4(FragPos, 1.0)).xyz;
		samp = fragpos + samp * radius;
		vec4 offset = vec4(samp, 1.0);
		offset = projection  * offset; // 
		offset.xyz /= offset.w; // 透视划分
		offset.xyz = offset.xyz * 0.5 + 0.5; // 变换到0.0 - 1.0的值域
		float sampleDepth = texture(camera.Depth, offset.xy).x;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(LinearizeDepth(Depth) - LinearizeDepth(sampleDepth)));
		occlusion += (sampleDepth >= Depth - 0.000025 ? 1.0 : 0.0) *rangeCheck;
	}
	occlusion /= kernelSize;
	color = occlusion;
};
