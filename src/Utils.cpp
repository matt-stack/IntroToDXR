/* Copyright (c) 2018-2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once


#include "Utils.h"

#include "MathExtras.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <fstream>
#include <shellapi.h>
#include <unordered_map>

#include <comdef.h>


namespace std
{
	void hash_combine(size_t &seed, size_t hash)
	{
		hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hash;
	}

	template<> struct hash<Vertex> 
	{
		size_t operator()(Vertex const &vertex) const 
		{
			size_t seed = 0;
			hash<float> hasher;
			hash_combine(seed, hasher(vertex.position.x));
			hash_combine(seed, hasher(vertex.position.y));
			hash_combine(seed, hasher(vertex.position.z));

			hash_combine(seed, hasher(vertex.uv.x));
			hash_combine(seed, hasher(vertex.uv.y));

			return seed;
		}
	};
}

using namespace std;
using DirectX::XMFLOAT3;

namespace Utils
{

//--------------------------------------------------------------------------------------
// Command Line Parser
//--------------------------------------------------------------------------------------

HRESULT ParseCommandLine(LPWSTR lpCmdLine, ConfigInfo &config)
{
	LPWSTR* argv = NULL;
	int argc = 0;

	argv = CommandLineToArgvW(GetCommandLine(), &argc);
	if (argv == NULL)
	{
		MessageBox(NULL, L"Unable to parse command line!", L"Error", MB_OK);
		return E_FAIL;
	}

	if (argc > 1)
	{
		char str[256];
		int i = 1;
		while (i < argc)
		{
			wcstombs(str, argv[i], 256);

			if (strcmp(str, "-width") == 0)
			{
				i++;
				wcstombs(str, argv[i], 256);
				config.width = atoi(str);
				i++;
				continue;
			}

			if (strcmp(str, "-height") == 0)
			{
				i++;
				wcstombs(str, argv[i], 256);
				config.height = atoi(str);
				i++;
				continue;
			}

			if (strcmp(str, "-vsync") == 0)
			{
				i++;
				wcstombs(str, argv[i], 256);
				config.vsync = (atoi(str) > 0);
				i++;
				continue;
			}

			if (strcmp(str, "-model") == 0)
			{
				i++;
				wcstombs(str, argv[i], 256);
				config.model = str;
				i++;
				continue;
			}

			i++;
		}
	}
	else 
	{
		MessageBox(NULL, L"Incorrect command line usage!", L"Error", MB_OK);
		return E_FAIL;
	}

	LocalFree(argv);
	return S_OK;
}

//--------------------------------------------------------------------------------------
// Error Messaging
//--------------------------------------------------------------------------------------

void Validate(HRESULT hr, LPWSTR msg)
{
	if (FAILED(hr))
	{


		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		//MessageBox(NULL, msg, L"Error", MB_OK);
		MessageBox(NULL, errMsg, L"Error", MB_OK);
		PostQuitMessage(EXIT_FAILURE);
	}
}

//--------------------------------------------------------------------------------------
// File Reading
//--------------------------------------------------------------------------------------

vector<char> ReadFile(const string &filename) 
{
	ifstream file(filename, ios::ate | ios::binary);

	if (!file.is_open()) 
	{
		throw std::runtime_error("Error: failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

//--------------------------------------------------------------------------------------
// Model Loading
//--------------------------------------------------------------------------------------

//void LoadModel(string filepath, Model &model, Material &material)
void LoadModel(string filepath, Model &model, Material &material, std::vector<MyMaterialCB>& materialVec, D3D12Resources& resources) 
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	// Load the OBJ and MTL files
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str(), "materials\\")) 
	{
		throw std::runtime_error(err);
	}

	// Get the first material
	// Only support a single material right now
	material.name = materials[0].name;
	material.texturePath = materials[0].diffuse_texname;

	// create a std::vector of My_Material somewhere and populate it.
	// then create a CBV, and map memcpy over

	for (auto it : materials) 
	{
		MyMaterialCB temp_MMCB {};
		//DirectX::XMFLOAT3 temp_DF = { it.diffuse[0], it.diffuse[1], it.diffuse[2] };
		DirectX::XMFLOAT4 temp_DF = { it.diffuse[0], it.diffuse[1], it.diffuse[2], 0.f };
		//DirectX::XMFLOAT4 temp_DF = { 0.f, 0.f, 1.f , 0.0f};
		temp_MMCB.diffuse = temp_DF;
		materialVec.emplace_back(temp_MMCB);
	}


	printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
	printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
	printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
	printf("# of materials = %d\n", (int)materials.size());
	printf("# of shapes    = %d\n", (int)shapes.size());

	 

	// Parse the model and store the unique vertices
	unordered_map<Vertex, uint32_t> uniqueVertices = {};

	std::vector<Vertex> light_average_pos{};

	for (const auto &shape : shapes) 
	{
		int material_index = 0;
		int material_offset = 0;
		int get_material = 0;
		for (const auto &index : shape.mesh.indices) 
		{
			Vertex vertex = {};
			vertex.position = 
			{
				attrib.vertices[3 * index.vertex_index + 2], // https://vulkan-tutorial.com/Loading_models
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 0]
			};
		//	if (index.normal_index > 0) {
				vertex.normal =
				{
					attrib.normals[3 * index.normal_index + 2],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 0]
				};
		//	}
			
			vertex.uv = 
			{
				attrib.texcoords[2 * index.texcoord_index + 0],
				1 - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			// add MartialID to Vertex struct,
			// found out how to id the material for the index
			// create CBV for Materials

			if ((material_index % 3) == 0) { // new triangle, get material 0, 1, 2, ..
				get_material = shape.mesh.material_ids[material_offset];
			}
			/*
			if (get_material == 8) {
				vertex.position.x /= 4;
				vertex.position.y /= 4;
				vertex.position.y += 1;
				vertex.position.z /= 4;

				light_average_pos.push_back(vertex);
			}
			*/
			vertex.materialId = { get_material, 0 };

			material_index++;

			// Fast find unique vertices using a hash
			if (uniqueVertices.count(vertex) == 0) 
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(model.vertices.size());
				model.vertices.push_back(vertex);
			}

				model.indices.push_back(uniqueVertices[vertex]);
			
		}
	}
/*
	Vertex light_center{};
	for (Vertex v : light_average_pos) {
		light_center.position += v.position;
	}
	light_center.position /= light_average_pos.size();
*/
	resources.light_pos.light.x = 0.0930555537f;
	resources.light_pos.light.y = 1.37499993f;
	resources.light_pos.light.z = 0.0818055421f;

}

//--------------------------------------------------------------------------------------
// Custom Modeling
//--------------------------------------------------------------------------------------
// Using VertexNorms
 
void CustomModel(ModelNorms& model) {
	unordered_map<Vertex, uint32_t> uniqueVertices = {};

	// Cube indices.
	uint32_t indices [] =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};

	// Cube vertices positions and corresponding triangle normals.
	VertexNorms vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};

	for (const auto& i : indices) {	
		model.indices.push_back(i);
	}

	for (const auto& v : vertices) {
		model.vertices.push_back(v);
	}

}

//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------

/**
* Format the loaded texture into the layout we use with D3D12.
*/
void FormatTexture(TextureInfo &info, UINT8* pixels)
{
	const UINT numPixels = (info.width * info.height);
	const UINT oldStride = info.stride;
	const UINT oldSize = (numPixels * info.stride);

	const UINT newStride = 4;				// uploading textures to GPU as DXGI_FORMAT_R8G8B8A8_UNORM
	const UINT newSize = (numPixels * newStride);
	info.pixels.resize(newSize);

	for (UINT i = 0; i < numPixels; i++)
	{
		info.pixels[i * newStride]		= pixels[i * oldStride];		// R
		info.pixels[i * newStride + 1]	= pixels[i * oldStride + 1];	// G
		info.pixels[i * newStride + 2]	= pixels[i * oldStride + 2];	// B
		info.pixels[i * newStride + 3]	= 0xFF;							// A (always 1)
	}	
	
	info.stride = newStride;
}

/**
* Load an image
*/
TextureInfo LoadTexture(string filepath) 
{
	TextureInfo result = {};

	// Load image pixels with stb_image
	UINT8* pixels = stbi_load(filepath.c_str(), &result.width, &result.height, &result.stride, STBI_default);
	if (!pixels)
	{
		throw runtime_error("Error: failed to load image!");
	}

	FormatTexture(result, pixels);
	stbi_image_free(pixels);
	return result;
}

}
