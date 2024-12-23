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
	animNodes.clear();
}

Animator::Animator()
	: currAnimation(0), globalTransformation(1.0f)
{
	bonesMatrices.reserve(MAX_NUM_BONES);

	for (uint32_t i = 0; i < MAX_NUM_BONES; i++)
	{
		bonesMatrices.push_back(glm::mat4(1.0f));
	}
}

void Animator::processModelNodes(const aiScene* scene)
{
	globalTransformation = AssimpGLMHelpers::getGLMMat4(scene->mRootNode->mTransformation.Inverse());

	readModelNodeHierarchy(scene->mRootNode, rootModelNode);
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
			assert(vertices.size() >= boneWeights[j].mVertexId);

			vertices[boneWeights[j].mVertexId].addBoneData(boneID, boneWeights[j].mWeight);
		}
	}
}

void Animator::processMissingBones(const aiScene* scene)
{
	for (uint32_t i = 0; i < scene->mNumAnimations; i++)
	{
		for (uint32_t j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
		{
			std::string boneName = scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str();

			if (bones.find(boneName) == bones.end())
			{
				Bone bone;

				bone.ID = bones.size();
				bone.offsetMatrix = glm::mat4(1.0f);

				bones[boneName] = bone;
			}
		}
	}
}

void Animator::update(float deltaTime)
{
	if (animations.size() > 0)
	{
		animations[currAnimation].update(deltaTime);

		calcBoneTransformation(rootModelNode, glm::mat4(1.0f));
	}
}

void Animator::clean()
{
	for (uint32_t i = 0; i < animations.size(); i++)
	{
		animations[i].clean();
	}

	bones.clear();
	animations.clear();
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

void Animator::readModelNodeHierarchy(const aiNode* source, ModelNode& destination)
{
	destination.name = source->mName.data;
	destination.transformation = AssimpGLMHelpers::getGLMMat4(source->mTransformation);

	for (uint32_t i = 0; i < source->mNumChildren; i++)
	{
		ModelNode data;

		readModelNodeHierarchy(source->mChildren[i], data);

		destination.children.push_back(data);
	}
}

void Animator::calcBoneTransformation(ModelNode& modelNode, const glm::mat4& parentTransformation)
{
	glm::mat4 currTransformation = parentTransformation;
	std::map<std::string, AnimNode>& animNodes = animations[currAnimation].getAnimNodes();

	if (animNodes.find(modelNode.name) != animNodes.end())
	{
		currTransformation = currTransformation * animNodes[modelNode.name].transformation;
	}
	else
	{
		currTransformation = currTransformation * modelNode.transformation;
	}

	if (bones.find(modelNode.name) != bones.end())
	{
		bonesMatrices[bones[modelNode.name].ID] = currTransformation * bones[modelNode.name].offsetMatrix; // FIXME: should I multiply by the scene "globalTransformation"?
	}

	for (ModelNode& childNode : modelNode.children)
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
		switch (texture.type)
		{
		case MeshTexture::Type::DIFFUSE:
			shader->setUniform1i("uMaterial.diffuseMap", unit);
			break;
		case MeshTexture::Type::SPECULAR:
			shader->setUniform1i("uMaterial.specularMap", unit);
			break;
		case MeshTexture::Type::EMISSION:
			shader->setUniform1i("uMaterial.emissionMap", unit);
			break;
		case MeshTexture::Type::NORMAL:
			shader->setUniform1i("uMaterial.normalMap", unit);
			break;
		default:
			break;
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
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);

	for (const MeshTexture& texture : textures)
	{
		glDeleteTextures(1, &texture.ID);
	}

	vertices.clear();
	indices.clear();
	textures.clear();
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
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(MeshVertex), (void*)(offsetof(MeshVertex, boneIDs))); // glVertexAttribPointer(3, 4, GL_INT, GL_FALSE, sizeof(MeshVertex), (void*)(offsetof(MeshVertex, boneIDs)));
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
	for (Mesh& mesh : meshes)
	{
		mesh.clean();
	}

	animator.clean();

	loadedTextures.clear();
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

	std::cout << "[LOG] MODEL: Loading model \"" << fp << "\"." << std::endl;

	directory = fp.substr(0, fp.find_last_of('/'));

	animator.processModelNodes(scene);
	animator.processAnimations(scene);

	processNode(scene->mRootNode, scene);

	animator.processMissingBones(scene); // FIXME: really necessary?
}

uint32_t Model::loadTexture(const char* filepath, bool gammaCorrection)
{
	stbi_set_flip_vertically_on_load(true);

	uint32_t ID;
	int width, height, colorChannels, internalFormat = GL_RED, format = GL_RED;
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

		internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
		format = GL_RGB;

		break;

	case 4:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
		format = GL_RGBA;

		break;

	default:
		std::cerr << "[ERROR] MODEL: Number of color channels not supported." << std::endl;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

			switch (type)
			{
			case aiTextureType_DIFFUSE:
				texture.type = MeshTexture::Type::DIFFUSE;
				break;
			case aiTextureType_SPECULAR:
				texture.type = MeshTexture::Type::SPECULAR;
				break;
			case aiTextureType_AMBIENT:
				// In most cases, same as DIFFUSE texture.
				break;
			case aiTextureType_EMISSIVE:
				texture.type = MeshTexture::Type::EMISSION;
				break;
			case aiTextureType_HEIGHT:
				texture.type = MeshTexture::Type::NORMAL;
				break;
			default:
				break;
			}

			std::cout << '\t' << "[LOG] MODEL: Loading material texture \"" << texture.filepath << "\"." << std::endl;

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

		std::vector<MeshTexture> emissionMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE);
		textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());

		std::vector<MeshTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	animator.processBones(mesh, vertices);

	return Mesh(vertices, indices, textures);
}
