#shader vertex
#version 420 core
layout(location = 0) in vec3 position;

uniform mat4 LightSpace;
uniform mat4 model;
void main()
{
	gl_Position = LightSpace * model * vec4(position, 1.0f);
};
#shader fragment
#version 420 core
layout(location = 0) out vec4 fragcolor;

void main()
{
	//gl_FragDepth = gl_FragCoord.z;
	//fragcolor = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
}
