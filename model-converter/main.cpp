#pragma warning(disable: 4819) // �x������

#include <fstream>
#include <iostream>
#include <vector>

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

#include "model.h"

#pragma comment(lib, "assimp.lib")

void GetVertices(aiMesh * mesh, std::vector<Vertex> & vertices)
{
	if (!mesh->HasFaces()) return;

	unsigned int vertices_cnt = mesh->mNumFaces * 3;
	vertices.resize(vertices_cnt);

	std::cout << "���_��: " << vertices_cnt << std::endl;

	for (unsigned int n = 0; n < mesh->mNumFaces; ++n)
	{
		auto indices = mesh->mFaces[n].mIndices;

		for (int i = 0; i < 3; ++i)
		{
			vertices[n * 3 + i].position_.x = mesh->mVertices[indices[i]].x;
			vertices[n * 3 + i].position_.y = mesh->mVertices[indices[i]].y;
			vertices[n * 3 + i].position_.z = mesh->mVertices[indices[i]].z;

			vertices[n * 3 + i].normal_.x = mesh->mNormals[indices[i]].x;
			vertices[n * 3 + i].normal_.y = mesh->mNormals[indices[i]].y;
			vertices[n * 3 + i].normal_.z = mesh->mNormals[indices[i]].z;

			vertices[n * 3 + i].uv_.x = mesh->mTextureCoords[0][indices[i]].x;
			vertices[n * 3 + i].uv_.y = mesh->mTextureCoords[0][indices[i]].y;

			//std::cout << vertices[n * 3 + i].uv_.x << std::endl;
		}
	}
}

void GetMeshes(const aiScene * scene, std::vector<Mesh> & meshes)
{
	meshes.clear();

	std::cout << "���b�V����: " << scene->mNumMeshes << std::endl;

	meshes.resize(scene->mNumMeshes);

	for (unsigned int n = 0; n < scene->mNumMeshes; ++n)
		GetVertices(scene->mMeshes[n], meshes[n].vertices_);
}

void Convert(std::vector<Mesh> & meshes, std::string file_name)
{
	Assimp::Importer importer;
	const aiScene * assimp_scene = nullptr;
	std::vector<aiMesh*> assimp_mesh;

	assimp_scene = importer.ReadFile(file_name, aiProcess_Triangulate);

	if (!assimp_scene)
	{
		std::cout << "�V�[�������Ɏ��s���܂���" << std::endl;
		return;
	}
	else
	{
		std::cout << "�V�[�������ɐ������܂���" << std::endl;
	}

	if (!assimp_scene->HasMeshes())
	{
		std::cout << "���b�V����������܂���ł���" << std::endl;
		return;
	}
	else
	{
		std::cout << "���b�V����������܂���" << std::endl;
	}

	GetMeshes(assimp_scene, meshes);
}

void Write(std::vector<Mesh> & meshes, std::string file_name)
{
	std::ofstream file;
	file.open(file_name, std::ios::out | std::ios::binary);
	
	file << meshes.size();

	for (unsigned int n = 0; n < meshes.size(); ++n)
	{
		auto vtx_cnt = meshes[n].vertices_.size();
		file.write((char*)&vtx_cnt, sizeof(unsigned int));
		for (unsigned int i = 0; i < vtx_cnt; ++i)
		{
			file.write((char*)&meshes[n].vertices_[i], sizeof(Vertex));
		}
	}

	file.close();
}

void Read(std::vector<Mesh> & meshes, std::string file_name)
{
	meshes.clear();

	std::ifstream file;
	file.open(file_name, std::ios::in | std::ios::binary);

	unsigned int mesh_cnt = 0;
	file >> mesh_cnt;

	meshes.resize(mesh_cnt);

	for (unsigned int n = 0; n < mesh_cnt; ++n)
	{
		unsigned int vtx_cnt = 0;
		file.read((char*)&vtx_cnt, sizeof(unsigned int));
		
		meshes[n].vertices_.resize(vtx_cnt);
		for (unsigned int i = 0; i < vtx_cnt; ++i)
		{
			file.read((char*)&meshes[n].vertices_[i], sizeof(Vertex));
		}
	}

	file.close();
}

void Show(std::vector<Mesh> & meshes)
{
	for (auto & mesh : meshes)
	{
		for (auto & vtx : mesh.vertices_)
		{
			std::cout << vtx.uv_.x << std::endl;
		}
	}
}

int main(int cnt, char** str)
{
	std::vector<Mesh> meshes;

	for (int n = 1; n < cnt; ++n)
	{
		std::cout << str[n] << std::endl;

		std::string strx = str[n];
		Convert(meshes, strx);
		strx.replace(strx.rfind("."), strx.size() - strx.rfind("."), ".geometry");
		Write(meshes, strx);
	}

	rewind(stdin);
	getchar();
	return 0;
}