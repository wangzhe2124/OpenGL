#shader vertex
#version 420 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;

layout(location = 7) in mat4 instanceMatrix;
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool is_instance;


void main()
{
    mat4 model_matrix;
    if (is_instance)
        model_matrix = instanceMatrix;
    else
        model_matrix = model;
    //anime
    mat4 transMatrix = mat4(0.0f);
    bool no_bone = true;
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        no_bone = no_bone && (boneIds[i] == -1);
        if (boneIds[i] == -1)
        {

            continue;
        }
        if (boneIds[i] >= MAX_BONES)
        {
            transMatrix = mat4(1.0f);
            break;
        }
        transMatrix += finalBonesMatrices[boneIds[i]] * weights[i];
    }
    if (no_bone)
        transMatrix = mat4(1.0f);
    vec4 totalPosition = transMatrix * vec4(Position, 1.0f);
    gl_Position = projection * view * model_matrix * totalPosition;

};

#shader fragment
#version 420 core

void main()
{

}