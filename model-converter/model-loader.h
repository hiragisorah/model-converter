#pragma once

#pragma warning(disable: 4819) // åxçêñ≥å¯

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <DirectXMath.h>

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

#pragma comment(lib, "assimp.lib")

using namespace DirectX;

struct Vertex
{
	DirectX::XMFLOAT3 position_;
	DirectX::XMFLOAT3 normal_;
	DirectX::XMFLOAT2 texcoord_;
};

struct Mesh
{
	std::vector<Vertex> vertices_;
	std::vector<unsigned int> indices_;
	std::string texture_;
};

class ModelLoader
{
private:
	Assimp::Importer importer_;

private:
	std::vector<Mesh> meshes_;

public:
	bool Load(std::string file_path)
	{
		auto scene = this->importer_.ReadFile(file_path.c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

		if (scene == nullptr)
		{
			std::cout << this->importer_.GetErrorString() << std::endl;
			return false;
		}

		this->meshes_.clear();
		
		this->ProcessNode(scene->mRootNode, scene);
		return true;
	}

	void Save(std::string file_path)
	{
		std::ofstream file;

		file.open(file_path, std::ios::out | std::ios::binary);

		unsigned int mesh_cnt = this->meshes_.size();

		file.write(reinterpret_cast<char*>(&mesh_cnt), sizeof(unsigned int));

		for (unsigned int m = 0; m < mesh_cnt; ++m)
		{
			auto & mesh = this->meshes_[m];

			unsigned int vtx_cnt = mesh.vertices_.size();

			file.write(reinterpret_cast<char*>(&vtx_cnt), sizeof(unsigned int));

			for (unsigned int v = 0; v < vtx_cnt; ++v)
			{
				auto & vtx = mesh.vertices_[v];

				file.write(reinterpret_cast<char*>(&vtx), sizeof(Vertex));
			}

			unsigned int index_cnt = mesh.indices_.size();

			file.write(reinterpret_cast<char*>(&index_cnt), sizeof(unsigned int));

			for (unsigned int i = 0; i < index_cnt; ++i)
			{
				auto & index = mesh.indices_[i];

				file.write(reinterpret_cast<char*>(&index), sizeof(unsigned int));
			}

			unsigned texture_str_cnt = mesh.texture_.size();

			file.write(reinterpret_cast<char*>(&texture_str_cnt), sizeof(unsigned int));

			if (texture_str_cnt > 0)
			{
				file.write(mesh.texture_.c_str(), sizeof(char) * texture_str_cnt);
			}
		}

		file.close();
	}

	void LoadFromFile(std::string file_path)
	{
		this->meshes_.clear();

		std::ifstream file;

		file.open(file_path, std::ios::in | std::ios::binary);

		unsigned int mesh_cnt;

		file.read(reinterpret_cast<char*>(&mesh_cnt), sizeof(unsigned int));

		this->meshes_.resize(mesh_cnt);

		for (unsigned int m = 0; m < mesh_cnt; ++m)
		{
			auto & mesh = this->meshes_[m];

			unsigned int vtx_cnt = 0;

			file.read(reinterpret_cast<char*>(&vtx_cnt), sizeof(unsigned int));

			mesh.vertices_.resize(vtx_cnt);

			for (unsigned int v = 0; v < vtx_cnt; ++v)
			{
				auto & vtx = mesh.vertices_[v];

				file.read(reinterpret_cast<char*>(&vtx), sizeof(Vertex));
			}

			unsigned int index_cnt = 0;

			file.read(reinterpret_cast<char*>(&index_cnt), sizeof(unsigned int));

			mesh.indices_.resize(index_cnt);

			for (unsigned int i = 0; i < index_cnt; ++i)
			{
				auto & index = mesh.indices_[i];

				file.read(reinterpret_cast<char*>(&index), sizeof(unsigned int));
			}

			unsigned texture_str_cnt = 0;

			file.read(reinterpret_cast<char*>(&texture_str_cnt), sizeof(unsigned int));

			mesh.texture_.resize(texture_str_cnt);

			if (texture_str_cnt > 0)
			{
				char * texture_str = new char[texture_str_cnt + 1];
				file.read(&texture_str[0], sizeof(char) * texture_str_cnt);
				texture_str[texture_str_cnt] = '\0';
				mesh.texture_ = texture_str;
				mesh.texture_ = mesh.texture_.substr(mesh.texture_.rfind("\\") + 1, mesh.texture_.size() - mesh.texture_.rfind("\\"));

				std::cout << mesh.texture_.c_str() << std::endl;

				delete[] texture_str;
			}
		}

		file.close();
	}

private:
	void ProcessNode(aiNode * node, const aiScene * scene)
	{
		for (unsigned int n = 0; n < node->mNumMeshes; ++n)
		{
			auto mesh = scene->mMeshes[node->mMeshes[n]];
			this->meshes_.emplace_back(this->ProcessMesh(mesh, scene));
		}

		for (unsigned int n = 0; n < node->mNumChildren; ++n)
			this->ProcessNode(node->mChildren[n], scene);
	}

	Mesh ProcessMesh(aiMesh * mesh, const aiScene * scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::string texture;

		for (unsigned int n = 0; n < mesh->mNumVertices; ++n)
		{
			Vertex vertex;

			vertex.position_.x = mesh->mVertices[n].x;
			vertex.position_.y = mesh->mVertices[n].y;
			vertex.position_.z = mesh->mVertices[n].z;

			if (mesh->mNormals)
			{
				vertex.normal_.x = (float)mesh->mNormals[n].x;
				vertex.normal_.y = (float)mesh->mNormals[n].y;
				vertex.normal_.z = (float)mesh->mNormals[n].z;
			}

			if (mesh->mTextureCoords[0])
			{
				vertex.texcoord_.x = (float)mesh->mTextureCoords[0][n].x;
				vertex.texcoord_.y = (float)mesh->mTextureCoords[0][n].y;
			}

			vertices.emplace_back(vertex);
		}

		for (unsigned int n = 0; n < mesh->mNumFaces; ++n)
		{
			aiFace face = mesh->mFaces[n];

			for (unsigned int i = 0; i < face.mNumIndices; ++i)
				indices.emplace_back(face.mIndices[i]);
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial * mat = scene->mMaterials[mesh->mMaterialIndex];
			aiString str;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
			texture = str.C_Str();
		}

		Mesh ret;

		ret.vertices_ = vertices;
		ret.indices_ = indices;
		ret.texture_ = texture;

		return ret;
	}
};