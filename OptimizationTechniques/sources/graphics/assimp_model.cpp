#include "assimp_model.h"

AssimpBone::AssimpBone(uint32_t ID, const std::string& name, const aiNodeAnim* channel)
	: ID(ID), name(name), localTransform(1.0f)
{
	for (int positionIndex = 0; positionIndex < channel->mNumPositionKeys; positionIndex++)
	{
		KeyPosition data;

		data.position = AssimpGLMHelpers::getGLMVec3(channel->mPositionKeys[positionIndex].mValue);
		data.timeStamp = channel->mPositionKeys[positionIndex].mTime;
	
		positions.push_back(data);
	}

	for (int rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; rotationIndex++)
	{
		KeyRotation data;

		data.orientation = AssimpGLMHelpers::getGLMQuat(channel->mRotationKeys[rotationIndex].mValue);
		data.timeStamp = channel->mRotationKeys[rotationIndex].mTime;

		rotations.push_back(data);
	}

	for (int scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; scaleIndex++)
	{
		KeyScale data;

		data.scale = AssimpGLMHelpers::getGLMVec3(channel->mScalingKeys[scaleIndex].mValue);
		data.timeStamp = channel->mScalingKeys[scaleIndex].mTime;

		scales.push_back(data);
	}
}

int AssimpBone::getPositionIndex(float animationTime)
{
	for (int index = 0; index < positions.size() - 1; index++)
	{
		if (animationTime < positions[index + 1].timeStamp)
		{
			return index;
		}
	}

	assert(0);
}

int AssimpBone::getRotationIndex(float animationTime)
{
	for (int index = 0; index < rotations.size() - 1; index++)
	{
		if (animationTime < rotations[index + 1].timeStamp)
		{
			return index;
		}
	}

	assert(0);
}

int AssimpBone::getScaleIndex(float animationTime)
{
	for (int index = 0; index < scales.size() - 1; index++)
	{
		if (animationTime < scales[index + 1].timeStamp)
		{
			return index;
		}
	}

	assert(0);
}

void AssimpBone::update(float animationTime)
{
	glm::mat4 translation = interpolatePosition(animationTime);
	glm::mat4 rotation = interpolateRotation(animationTime);
	glm::mat4 scale = interpolateScale(animationTime);

	localTransform = translation * rotation * scale;
}

float AssimpBone::calcNormalizedFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;

	return midWayLength / framesDiff;
}

glm::mat4 AssimpBone::interpolatePosition(float animationTime)
{
	if (positions.size() == 1)
	{
		return glm::translate(glm::mat4(1.0f), positions[0].position);
	}

	int p0Index = getPositionIndex(animationTime);
	int p1Index = p0Index + 1;

	float interpolationFactor = calcNormalizedFactor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, animationTime);
	
	glm::vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position, interpolationFactor);
	
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 AssimpBone::interpolateRotation(float animationTime)
{
	if (rotations.size() == 1)
	{
		auto rotation = glm::normalize(rotations[0].orientation);

		return glm::toMat4(rotation);
	}

	int p0Index = getRotationIndex(animationTime);
	int p1Index = p0Index + 1;

	float interpolationFactor = calcNormalizedFactor(rotations[p0Index].timeStamp, rotations[p1Index].timeStamp, animationTime);

	glm::quat finalRotation = glm::slerp(rotations[p0Index].orientation, rotations[p1Index].orientation, interpolationFactor);
	finalRotation = glm::normalize(finalRotation);

	return glm::toMat4(finalRotation);
}

glm::mat4 AssimpBone::interpolateScale(float animationTime)
{
	if (scales.size() == 1)
	{
		return glm::scale(glm::mat4(1.0f), scales[0].scale);
	}

	int p0Index = getScaleIndex(animationTime);
	int p1Index = p0Index + 1;

	float interpolationFactor = calcNormalizedFactor(scales[p0Index].timeStamp, scales[p1Index].timeStamp, animationTime);
	
	glm::vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale, interpolationFactor);
	
	return glm::scale(glm::mat4(1.0f), finalScale);
}

AssimpMesh::AssimpMesh(const std::vector<AssimpMVertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<AssimpMTexture>& textures)
	: VAO(0), VBO(0), IBO(0), vertices(vertices), indices(indices), textures(textures)
{
	setup();
}

void AssimpMesh::render(ShaderProgram* shader)
{
	int unit = 0;

	for (const AssimpMTexture& texture : textures)
	{
		if (texture.type == AssimpMTexture::Type::DIFFUSE)
		{
			shader->setUniform1i("uMaterial.diffuseMap", unit);
		}

		if (texture.type == AssimpMTexture::Type::SPECULAR)
		{
			// shader->setUniform1i("uMaterial.specularMap", unit);
		}

		// Activating and binding texture.
		if (unit >= 0 && unit <= 15)
		{
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(GL_TEXTURE_2D, texture.ID);
		}
		else
		{
			std::cout << "[ERROR] ASSIMP MESH: Failed to bind texture in unit " << unit << "." << std::endl;

			return;
		}

		unit += 1;
	}

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	
	glBindVertexArray(0);
}

void AssimpMesh::clean()
{
	// TODO.
}

void AssimpMesh::setup()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(AssimpMVertex), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpMVertex), (void*)(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpMVertex), (void*)(offsetof(AssimpMVertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AssimpMVertex), (void*)(offsetof(AssimpMVertex, uvs)));
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(AssimpMVertex), (void*)(offsetof(AssimpMVertex, boneIDs)));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(AssimpMVertex), (void*)(offsetof(AssimpMVertex, weights)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindVertexArray(0); // Unbind the VAO before any other buffer.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

AssimpModel::AssimpModel(const char* filepath, uint32_t flags)
{
	load(filepath, flags);
}

void AssimpModel::render(ShaderProgram* shader)
{
	for (AssimpMesh& mesh : meshes)
	{
		mesh.render(shader);
	}
}

void AssimpModel::clean()
{
	// TODO.
}

void AssimpModel::load(const char* filepath, uint32_t flags)
{
	Assimp::Importer importer;
	std::string fp = filepath;

	// More post-processing options:
	// 
	//	aiProcess_GenNormals: creates normal vectors for each vertex if the model doesn't contain normal vectors.
	//	aiProcess_SplitLargeMeshes: splits large meshes into smaller sub-meshes (which is useful if your rendering has a maximum number of vertices allowed and can only process smaller meshes).
	//	aiProcess_OptimizeMeshes: does the reverse by trying to join several meshes into one larger mesh (reducing drawing calls for optimization).
	// 
	// http://assimp.sourceforge.net/lib_html/postprocess_8h.html
	//
	const aiScene* scene = importer.ReadFile(fp, flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "[ERROR] ASSIMP MODEL: " << "\n" << importer.GetErrorString() << std::endl;

		return;
	}

	directory = fp.substr(0, fp.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

uint32_t AssimpModel::loadTexture(const char* filepath)
{
	stbi_set_flip_vertically_on_load(true);

	uint32_t ID;
	int width, height, colorChannels, format = GL_RED;
	stbi_uc* data = stbi_load(filepath, &width, &height, &colorChannels, 0);

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	// WARNING!
	// 
	// We are expecting an image with only 3 or 4 color channels.
	// Any other format may cause an OpenGL error.
	//
	switch (colorChannels)
	{
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		format = GL_RGB;

		break;

	case 4:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		format = GL_RGBA;

		break;

	default:
		std::cerr << "[ERROR] ASSIMP MODEL: Number of color channels not supported." << std::endl;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "[ERROR] ASSIMP MODEL: Failed to load texture \"" << filepath << "\"." << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	return ID;
}

std::vector<AssimpMTexture> AssimpModel::loadMaterialTextures(aiMaterial* material, aiTextureType type)
{
	std::vector<AssimpMTexture> textures;

	for (uint32_t i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString buffer;
		material->GetTexture(type, i, &buffer);

		std::string filepath = buffer.C_Str();
		bool skip = false;

		for (uint32_t j = 0; j < loadedTextures.size(); j++)
		{
			if (filepath == loadedTextures[j].filepath)
			{
				textures.push_back(loadedTextures[j]);
				skip = true;

				break;
			}
		}

		if (!skip)
		{
			AssimpMTexture texture;

			texture.ID = loadTexture((directory + "/" + filepath).c_str());
			texture.filepath = filepath;
			
			if (type == aiTextureType_DIFFUSE)
			{
				texture.type = AssimpMTexture::Type::DIFFUSE;
			}

			if (type == aiTextureType_SPECULAR)
			{
				texture.type = AssimpMTexture::Type::SPECULAR;
			}

			textures.push_back(texture);
		}
	}

	return textures;
}

void AssimpModel::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any).
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		meshes.push_back(processMesh(mesh, scene));
	}

	// Then do the same for each of its children.
	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

AssimpMesh AssimpModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<AssimpMVertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<AssimpMTexture> textures;

	// Process vertex positions, normals and texture coordinates.
	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		AssimpMVertex vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			vertex.uvs.x = mesh->mTextureCoords[0][i].x;
			vertex.uvs.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.uvs = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	// Process indices.
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (uint32_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process material.
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<AssimpMTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<AssimpMTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	// Process bone weights.
	extractBoneWeights(mesh, scene, vertices);

	return AssimpMesh(vertices, indices, textures);
}

void AssimpModel::extractBoneWeights(aiMesh* mesh, const aiScene* scene, std::vector<AssimpMVertex>& vertices)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			AssimpBoneInfo boneInfo;

			boneID = boneInfoMap.size();

			boneInfo.ID = boneID;
			boneInfo.offsetMatrix = AssimpGLMHelpers::convertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
			
			boneInfoMap[boneName] = boneInfo;
		}
		else
		{
			boneID = boneInfoMap[boneName].ID;
		}

		assert(boneID != -1);

		aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; weightIndex++)
		{
			int vertexID = weights[weightIndex].mVertexId;

			assert(vertexID <= vertices.size());

			vertices[vertexID].pushBoneData(boneID, weights[weightIndex].mWeight);
		}
	}
}

AssimpAnimation::AssimpAnimation(const char* filepath, AssimpModel& model, uint32_t flags)
	: duration(0.0f), ticksPerSecond(0.0f)
{
	Assimp::Importer importer;
	std::string fp = filepath;

	const aiScene* scene = importer.ReadFile(fp, flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "[ERROR] ASSIMP ANIMATION: " << "\n" << importer.GetErrorString() << std::endl;

		return;
	}

	const aiAnimation* animation = scene->mAnimations[0];

	duration = animation->mDuration;
	ticksPerSecond = animation->mTicksPerSecond;

	//aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	//globalTransformation = globalTransformation.Inverse();

	readHierarchyData(scene->mRootNode, rootNode);
	readMissingBones(animation, model);
}

AssimpBone* AssimpAnimation::findBone(const std::string& name)
{
	auto iterator = std::find_if(bones.begin(), bones.end(),
		[&](AssimpBone& bone)
		{
			return bone.getName() == name;
		}
	);

	if (iterator == bones.end()) return nullptr;
	else return &(*iterator);
}

void AssimpAnimation::readMissingBones(const aiAnimation* animation, AssimpModel& model)
{
	std::map<std::string, AssimpBoneInfo>& modelBoneInfoMap = model.getBoneInfoMap();

	for (uint32_t i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (modelBoneInfoMap.find(boneName) == modelBoneInfoMap.end())
		{
			modelBoneInfoMap[boneName].ID = modelBoneInfoMap.size();
		}

		bones.push_back(AssimpBone(modelBoneInfoMap[boneName].ID, boneName, channel));
	}

	boneInfoMap = modelBoneInfoMap;
}

void AssimpAnimation::readHierarchyData(const aiNode* source, AssimpNodeData& destination)
{
	destination.name = source->mName.data;
	destination.transformation = AssimpGLMHelpers::convertMatrixToGLMFormat(source->mTransformation);

	for (uint32_t i = 0; i < source->mNumChildren; i++)
	{
		AssimpNodeData data;

		readHierarchyData(source->mChildren[i], data);

		destination.children.push_back(data);
	}
}

AssimpAnimator::AssimpAnimator(AssimpAnimation* animation)
	: currAnimation(animation), currTime(0.0f)
{
	bonesMatrices.reserve(100);

	for (int i = 0; i < 100; i++)
	{
		bonesMatrices.push_back(glm::mat4(1.0f));
	}
}

void AssimpAnimator::update(float deltaTime)
{
	if (currAnimation)
	{
		currTime = currTime + currAnimation->getTicksPerSecond() * deltaTime;
		currTime = fmod(currTime, currAnimation->getDuration());

		calcBoneTransform(currAnimation->getRootNode(), glm::mat4(1.0f));
	}
}

void AssimpAnimator::play(AssimpAnimation* animation)
{
	currAnimation = animation;
	currTime = 0.0f;
}

void AssimpAnimator::calcBoneTransform(const AssimpNodeData& node, const glm::mat4& parentTransform)
{
	AssimpBone* Bone = currAnimation->findBone(node.name);
	glm::mat4 nodeTransform = node.transformation;

	if (Bone)
	{
		Bone->update(currTime);

		nodeTransform = Bone->getLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	std::map<std::string, AssimpBoneInfo>& boneInfoMap = currAnimation->getBoneInfoMap();

	if (boneInfoMap.find(node.name) != boneInfoMap.end())
	{
		int index = boneInfoMap[node.name].ID;

		bonesMatrices[index] = globalTransformation * boneInfoMap[node.name].offsetMatrix;
	}

	for (const AssimpNodeData& childNode : node.children)
	{
		calcBoneTransform(childNode, globalTransformation);
	}
}
