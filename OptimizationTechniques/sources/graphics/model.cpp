#include "model.h"

Animation::Animation(const aiAnimation* animation)
	: duration(0.0f), ticksPerSecond(0.0f), currTime(0.0f)
{
	name = animation->mName.C_Str();

	duration = animation->mDuration;
	ticksPerSecond = animation->mTicksPerSecond;

	for (uint32_t i = 0; i < animation->mNumChannels; i++)
	{
		std::string animNodeName = animation->mChannels[i]->mNodeName.C_Str();
		AnimNode animNode;

		for (uint32_t j = 0; j < animation->mChannels[i]->mNumPositionKeys; j++)
		{
			AnimKeyPosition data;

			data.value = AssimpGLMHelpers::getGLMVec3(animation->mChannels[i]->mPositionKeys[j].mValue);
			data.timeStamp = animation->mChannels[i]->mPositionKeys[j].mTime;

			animNode.positions.push_back(data);
		}

		for (uint32_t j = 0; j < animation->mChannels[i]->mNumRotationKeys; j++)
		{
			AnimKeyRotation data;

			data.value = AssimpGLMHelpers::getGLMQuat(animation->mChannels[i]->mRotationKeys[j].mValue);
			data.timeStamp = animation->mChannels[i]->mRotationKeys[j].mTime;

			animNode.rotations.push_back(data);
		}

		for (uint32_t j = 0; j < animation->mChannels[i]->mNumScalingKeys; j++)
		{
			AnimKeyScaling data;

			data.value = AssimpGLMHelpers::getGLMVec3(animation->mChannels[i]->mScalingKeys[j].mValue);
			data.timeStamp = animation->mChannels[i]->mScalingKeys[j].mTime;

			animNode.scalings.push_back(data);
		}

		animNodes[animNodeName] = animNode;
	}
}

void Animation::update(float deltaTime)
{
	currTime = std::fmod(currTime + ticksPerSecond * deltaTime, duration);

	for (std::map<std::string, AnimNode>::iterator it = animNodes.begin(); it != animNodes.end(); it++)
	{
		it->second.update(currTime);
	}
}

void Animation::clean()
{
	// TODO.
}

Animator::Animator()
	: currAnimation(0)
{
	bonesMatrices.reserve(MAX_NUM_BONES);

	for (uint32_t i = 0; i < MAX_NUM_BONES; i++)
	{
		bonesMatrices.push_back(glm::mat4(1.0f));
	}
}

void Animator::processBoneNodes(const aiScene* scene)
{
	readBoneNodeHierarchy(scene->mRootNode, rootBoneNode);
}

void Animator::processAnimations(const aiScene* scene)
{
	for (uint32_t i = 0; i < scene->mNumAnimations; i++)
	{
		animations.push_back(Animation(scene->mAnimations[i]));
	}
}

void Animator::processBones(aiMesh* mesh, std::vector<MeshVertex>& vertices)
{
	for (uint32_t i = 0; i < mesh->mNumBones; i++)
	{
		uint32_t boneID = bones.size();
		std::string boneName = mesh->mBones[i]->mName.C_Str();
		aiVertexWeight* boneWeights = mesh->mBones[i]->mWeights;

		if (bones.find(boneName) == bones.end())
		{
			Bone bone;

			bone.ID = boneID;
			bone.offsetMatrix = AssimpGLMHelpers::getGLMMat4(mesh->mBones[i]->mOffsetMatrix);

			bones[boneName] = bone;
		}
		else
		{
			boneID = bones[boneName].ID;
		}

		for (uint32_t j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			uint32_t vertexID = boneWeights[j].mVertexId;

			assert(vertexID <= vertices.size());

			addMeshVertexBoneData(vertices[vertexID], boneID, boneWeights[j].mWeight);
		}
	}
}

void Animator::update(float deltaTime)
{
	if (animations.size() > 0)
	{
		animations[currAnimation].update(deltaTime);

		calcBoneTransformation(rootBoneNode, glm::mat4(1.0f));
	}
}

void Animator::clean()
{
	// TODO.
}

void Animator::execAnimation(uint32_t number)
{
	if (animations.size() > number)
	{
		currAnimation = number;

	}
	else
	{
		std::cout << "[ERROR] ANIMATOR: Animation number " << number << " not found." << std::endl;
	}
}

void Animator::execAnimation(const std::string name)
{
	bool found = false;

	for (uint32_t i = 0; i < animations.size(); i++)
	{
		if (animations[i].getName() == name)
		{
			currAnimation = i;

			found = true;
		}
	}

	if (!found)
	{
		std::cout << "[ERROR] ANIMATOR: Animation \"" << name << "\" not found." << std::endl;
	}
}

void Animator::addMeshVertexBoneData(MeshVertex& meshVertex, uint32_t boneID, float weight)
{
	for (uint32_t i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++)
	{
		if (meshVertex.boneIDs[i] == -1)
		{
			meshVertex.boneIDs[i] = boneID;
			meshVertex.weights[i] = weight;

			break;
		}
	}

	// assert(0); // Should never get here, more bones than we have space for.
}

void Animator::readBoneNodeHierarchy(const aiNode* source, BoneNode& destination)
{
	destination.name = source->mName.data;
	destination.transformation = AssimpGLMHelpers::getGLMMat4(source->mTransformation);

	for (uint32_t i = 0; i < source->mNumChildren; i++)
	{
		BoneNode data;

		readBoneNodeHierarchy(source->mChildren[i], data);

		destination.children.push_back(data);
	}
}

void Animator::calcBoneTransformation(BoneNode& boneNode, const glm::mat4& parentTransformation)
{
	glm::mat4 currTransformation = parentTransformation;
	std::map<std::string, AnimNode>& animNodes = animations[currAnimation].getAnimNodes();

	if (animNodes.find(boneNode.name) != animNodes.end())
	{
		currTransformation = currTransformation * animNodes[boneNode.name].transformation;
	}
	else
	{
		currTransformation = currTransformation * boneNode.transformation;
	}

	if (bones.find(boneNode.name) != bones.end())
	{
		bonesMatrices[bones[boneNode.name].ID] = currTransformation * bones[boneNode.name].offsetMatrix;
	}

	for (BoneNode& childNode : boneNode.children)
	{
		calcBoneTransformation(childNode, currTransformation);
	}
}

Mesh::Mesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<MeshTexture>& textures)
	: VAO(0), VBO(0), IBO(0), vertices(vertices), indices(indices), textures(textures)
{
	load();
}

void Mesh::render(ShaderProgram* shader)
{
	int unit = 0;

	for (const MeshTexture& texture : textures)
	{
		if (texture.type == MeshTexture::Type::DIFFUSE)
		{
			shader->setUniform1i("uMaterial.diffuseMap", unit);
		}

		if (texture.type == MeshTexture::Type::SPECULAR)
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
			std::cout << "[ERROR] MESH: Failed to bind texture in unit " << unit << "." << std::endl;

			return;
		}

		unit += 1;
	}

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Mesh::clean()
{
	// TODO.
}

void Mesh::load()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(offsetof(MeshVertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(offsetof(MeshVertex, uvs)));
	//glVertexAttribIPointer(3, 4, GL_INT, sizeof(MeshVertex), (void*)(offsetof(MeshVertex, boneIDs)));
	glVertexAttribPointer(3, 4, GL_INT, GL_FALSE, sizeof(MeshVertex), (void*)(offsetof(MeshVertex, boneIDs)));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(offsetof(MeshVertex, weights)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindVertexArray(0); // Unbind the VAO before any other buffer.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Model::Model(const char* filepath, uint32_t flags)
	: animator()
{
	load(filepath, flags);
}

void Model::render(ShaderProgram* shader)
{
	for (Mesh& mesh : meshes)
	{
		mesh.render(shader);
	}
}

void Model::clean()
{
	// TODO.
}

void Model::load(const char* filepath, uint32_t flags)
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
		std::cout << "[ERROR] MODEL: " << "\n" << importer.GetErrorString() << std::endl;

		return;
	}

	directory = fp.substr(0, fp.find_last_of('/'));

	animator.processBoneNodes(scene);
	animator.processAnimations(scene);

	processNode(scene->mRootNode, scene);
}

uint32_t Model::loadTexture(const char* filepath)
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
		std::cerr << "[ERROR] MODEL: Number of color channels not supported." << std::endl;
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
		std::cerr << "[ERROR] MODEL: Failed to load texture \"" << filepath << "\"." << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	return ID;
}

std::vector<MeshTexture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type)
{
	std::vector<MeshTexture> textures;

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
			MeshTexture texture;

			texture.ID = loadTexture((directory + "/" + filepath).c_str());
			texture.filepath = filepath;

			if (type == aiTextureType_DIFFUSE)
			{
				texture.type = MeshTexture::Type::DIFFUSE;
			}

			if (type == aiTextureType_SPECULAR)
			{
				texture.type = MeshTexture::Type::SPECULAR;
			}

			textures.push_back(texture);
		}
	}

	return textures;
}

void Model::processNode(aiNode* node, const aiScene* scene)
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

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<MeshVertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<MeshTexture> textures;

	// Process vertex positions, normals and texture coordinates.
	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		MeshVertex vertex;

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

		std::vector<MeshTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<MeshTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	animator.processBones(mesh, vertices);

	return Mesh(vertices, indices, textures);
}
