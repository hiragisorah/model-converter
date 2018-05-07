#pragma once

#include <vector>

#include <DirectXMath.h>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position_;
	XMFLOAT3 normal_;
	XMFLOAT2 uv_;
};

struct Material
{
	XMFLOAT4 ambient_;
	XMFLOAT4 diffuse_;
	XMFLOAT4 specular_;
};

struct Mesh
{
	Material material_;
	std::vector<Vertex> vertices_;
	std::vector<unsigned int> indices_;
};

struct Model
{
	std::vector<Mesh> meshes_;
};