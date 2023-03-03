#pragma once
#include <glm/glm.hpp>
#include "Model.h"
struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
private:
	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	void ReadMissingBones(const aiAnimation* animation, Model& model)
	{
		int size = animation->mNumChannels;

		auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
		int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_Bones.push_back(Bone(channel->mNodeName.data,
				boneInfoMap[channel->mNodeName.data].id, channel));
		}

		m_BoneInfoMap = boneInfoMap;
	}

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		assert(src);

		dest.name = src->mName.data;
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		for (unsigned int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
public:
	Animation() = default;

	Animation(std::string const& animationPath, Model* model)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];
		m_Duration = static_cast<float>(animation->mDuration);
		std::fstream out;

		m_TicksPerSecond = static_cast<int>(animation->mTicksPerSecond);


		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		globalTransformation = globalTransformation.Inverse();
		ReadHierarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);
	}

	~Animation()
	{
	}

	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);
		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}


	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration; }
	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
	inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
	{
		return m_BoneInfoMap;
	}
};

class Animations
{
public:
	static enum {walk, death, punching, attack, MAX};
	Animation* Animation_death;
	Animation* Animation_punching;
	Animation* Animation_walk;
	Animation* Animation_attack;
	std::unordered_map<int, Animation*> animations_map;
	Animations(Model* model)
	{
		Animation_death = new Animation("res/objects/death.dae", model);
		animations_map[death] = Animation_death;
		Animation_punching = new Animation("res/objects/Punching.dae", model);
		animations_map[punching] = Animation_punching;
		Animation_walk = new Animation("res/objects/walking.dae", model);
		animations_map[walk] = Animation_walk;
		Animation_attack = new Animation("res/objects/attack.dae", model);
		animations_map[attack] = Animation_attack;
	}
	~Animations()
	{
		for (std::unordered_map<int, Animation*>::iterator iter = animations_map.begin(); iter != animations_map.end(); iter++)
			delete iter->second;
	}
};
class Animator
{
private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
	float Duration;
	bool is_RootAnime;
public:
	glm::mat4 RootTransform;
	Animator(Animation* animation = nullptr, bool is_root = false) : is_RootAnime(is_root)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
		if(m_CurrentAnimation)
			Duration = m_CurrentAnimation->GetDuration();
	}

	void UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
			for (glm::mat4& m1 : m_FinalBoneMatrices)
			{
				m1[3][2] -= RootTransform[3][2];
			}
		}
	}
	float GetCurrentTime()
	{
		return m_CurrentTime;
	}
	float GetDuration()
	{
		return Duration;
	}
	void SetCurrentTime(float time)
	{
		m_CurrentTime = time;
	}
	float GetRatio()
	{
		return m_CurrentTime / m_CurrentAnimation->GetDuration();
	}
	void PlayAnimation(Animation* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
		if(m_CurrentAnimation)
			Duration = m_CurrentAnimation->GetDuration();
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;
		//std::cout << nodeName << std::endl;
		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}


		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
			if (nodeName == "mixamorig_Hips")
			{
				RootTransform = m_FinalBoneMatrices[index];
			}
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}
};

class Models
{
private:

public:
	std::unordered_map<std::string, Model*> models_map;
	std::unordered_map<std::string, Model*> anime_models_map;
	std::unordered_map<std::string, Animator> animator_map;
	Model Nano = Model("res/objects/nanosuit_upgrade/nanosuit.obj");
	Model Marry = Model("res/objects/Marry/Marry.obj");
	Model Planet = Model("res/objects/planet/planet.obj");
	Model Rock = Model("res/objects/rock/rock.obj");
	Model Floor;
	Model Sphere = Model();
	Model Main_character = Model("res/objects/Erika Archer/Erika Archer.dae");
	Model Terrain = Model();
	Model Sphere_instance = Model();
	Model Robot_boxing = Main_character;
	Model Robot_pray = Main_character;
	Model Robot_death = Main_character;
	Model Robot_walk = Main_character;
	void Get_models()
	{

		models_map["Nano"] = &Nano;
		models_map["Marry"] = &Marry;
		models_map["Planet"] = &Planet;
		models_map["Rock"] = &Rock;
	}
	void Get_anime_models()
	{
		anime_models_map["Main_character"] = &Main_character;
		anime_models_map["Robot_boxing"] = &Robot_boxing;
		anime_models_map["Robot_pray"] = &Robot_pray;
		anime_models_map["Robot_death"] = &Robot_death;
		anime_models_map["Robot_walk"] = &Robot_walk;
		for (std::unordered_map<std::string, Model*>::iterator iter = anime_models_map.begin(); iter != anime_models_map.end(); iter++)
		{
			animator_map[iter->first] = Animator();
		}
	}
	Models()
	{
		Get_models();
		Get_anime_models();
	}
	void CreatModel(string const& path, string const& name, float life = 100.0f)
	{
		Model* newmodel = new Model(path, life);
		models_map[name] = newmodel;
	}

};
