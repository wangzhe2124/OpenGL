#shader vertex
#version 420 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out VS_OUT{
 vec3 Normal;
 vec3 FragPos;
 vec2 TexCoord;
 mat3 TBN;
}vs_out;
void main()
{
    gl_Position = projection * view * model * vec4(Position, 1.0f);
    vs_out.FragPos = vec3(model * vec4(Position, 1.0));
    vs_out.Normal = mat3(transpose(inverse(model))) * Normal;//法线变换到世界空间，应设置uniform在主程序执行提升效率	
    vs_out.TexCoord = TexCoord;
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(Normal, 0.0)));

    //T = normalize(T - dot(T, N) * N);	
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    //vec3 B = cross(T, N);
    vs_out.TBN = mat3(T, B, N);
};

#shader fragment
#version 420 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;
in VS_OUT{
vec3 Normal;//注意顺序要与顶点着色器输出一致
vec3 FragPos;
vec2 TexCoord;
mat3 TBN;
}fs_in;

struct Material//材质信息
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    float shininess;
};
uniform Material material;
uniform bool use_NormalMap;
uniform bool use_HeightMap;
uniform vec3 viewPos;
//高度（视差）贴图计算位移的纹理坐标
vec2 ParallaxMapping(vec2 TexCoord, vec3 viewDir, vec3 normal)
{
	float height_scale = 0.1;
	// number of depth layers
	const float numLayers = 20;
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy * height_scale;
	vec2 deltaTexCoords = P / numLayers;
	// get initial values
	vec2  currentTexCoords = TexCoord;
	float currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;
	while (currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	// get depth after and before collision for linear interpolation
	float afterDepth = currentLayerDepth - currentDepthMapValue;
	float beforeDepth = texture(material.texture_height1, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth + beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
	return finalTexCoords;
}
void main()
{
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //选择是否使用高度贴图的纹理坐标
    vec2 TexCoord = ParallaxMapping(fs_in.TexCoord, viewDir, fs_in.Normal);
    TexCoord = use_HeightMap == true ? TexCoord : fs_in.TexCoord;
    //选择是否使用法线贴图
    vec3 Normal = texture(material.texture_normal1, TexCoord).rgb;
    Normal = normalize(fs_in.TBN * normalize(Normal));
    vec3 normal = use_NormalMap == true ? Normal : normalize(fs_in.Normal);

    // store the fragment position vector in the first gbuffer texture
    gPosition = fs_in.FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normal;
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(material.texture_diffuse1, TexCoord).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(material.texture_specular1, TexCoord).r;
}