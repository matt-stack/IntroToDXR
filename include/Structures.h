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

#include "Common.h"
#include <memory>

#if defined(USE_NSIGHT_AFTERMATH)
#include "NsightAftermathGpuCrashTracker.h"
#include <array>
#endif

//--------------------------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------------------------

static bool CompareVector3WithEpsilon(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
	const DirectX::XMFLOAT3 vector3Epsilon = DirectX::XMFLOAT3(0.00001f, 0.00001f, 0.00001f);
	return DirectX::XMVector3NearEqual(DirectX::XMLoadFloat3(&lhs), DirectX::XMLoadFloat3(&rhs), DirectX::XMLoadFloat3(&vector3Epsilon)) == TRUE;
}

static bool CompareVector2WithEpsilon(const DirectX::XMFLOAT2& lhs, const DirectX::XMFLOAT2& rhs)
{
	const DirectX::XMFLOAT2 vector2Epsilon = DirectX::XMFLOAT2(0.00001f, 0.00001f);
	return DirectX::XMVector3NearEqual(DirectX::XMLoadFloat2(&lhs), DirectX::XMLoadFloat2(&rhs), DirectX::XMLoadFloat2(&vector2Epsilon)) == TRUE;
}

//--------------------------------------------------------------------------------------
// Global Structures
//--------------------------------------------------------------------------------------

struct ConfigInfo 
{
	int				width = 640;
	int				height = 360;
	bool			vsync = false;
	std::string		model = "";
	HINSTANCE		instance = NULL;
};

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMINT2 materialId; // this is added! Throwing off normals! 4 bytes * 2?
	DirectX::XMFLOAT2 uv;

	bool operator==(const Vertex &v) const 
	{
		if (CompareVector3WithEpsilon(position, v.position)) 
		{
			if (CompareVector2WithEpsilon(uv, v.uv)) return true;
			return true;
		}
		return false;
	}

	Vertex& operator=(const Vertex& v) 
	{
		position = v.position;
		uv = v.uv;
		return *this;
	}
};

struct VertexNorms
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
//	DirectX::XMFLOAT2 uv;

	bool operator==(const VertexNorms &v) const 
	{
		if (CompareVector3WithEpsilon(position, v.position)) 
		{
			//if (CompareVector2WithEpsilon(uv, v.uv)) return true;
			if (CompareVector3WithEpsilon(normal, v.normal)) return true;
			return true;
		}
		return false;
	}

	VertexNorms& operator=(const VertexNorms& v) 
	{
		position = v.position;
		normal = v.normal;
		//uv = v.uv;
		return *this;
	}
};


struct Material_Info 
{
	DirectX::XMFLOAT3 albedo;
};

struct Material_Def 
{
	std::vector<Material_Info> info;
};

struct Material 
{
	std::string name = "defaultMaterial";
	std::string texturePath = "";
	float  textureResolution = 512;
};

struct Model
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

struct ModelNorms
{
	std::vector<VertexNorms> vertices;
	std::vector<uint32_t> indices;
};

struct TextureInfo
{
	std::vector<UINT8> pixels;
	int width = 0;
	int height = 0;
	int stride = 0;
	int offset = 0;
};

struct MaterialCB 
{
	DirectX::XMFLOAT4 resolution;
};

struct ViewCB
{
	DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT4 viewOriginAndTanHalfFovY = DirectX::XMFLOAT4(0, 0.f, 0.f, 0.f);
	DirectX::XMFLOAT2 resolution = DirectX::XMFLOAT2(1280, 720);
};

// mstack version 
struct MyMaterialCB
{
	//DirectX::XMFLOAT3 diffuse = {0, 0, 0}; //RGB
	DirectX::XMFLOAT4 diffuse = {0, 0, 0, 0}; //RGB + pad
};


struct miscBuffer
{
	DirectX::XMFLOAT4 frame_counter = { 0., 0., 0., 0.};
	DirectX::XMFLOAT2 has_moved = { 0, 0};
	DirectX::XMFLOAT3 rgB = { 0, 0, 1};
	DirectX::XMFLOAT2 rGe = { 0, 1 };
// random unpacked stuct to test non-array cbuffers
};

struct missShaderBuffer
{
	DirectX::XMFLOAT4 pack = {0.1, 0.5, 0.5, 0.,};
};

struct LightPos
{
	DirectX::XMFLOAT4 light = { 0., 0., 0., 0. };
};

//--------------------------------------------------------------------------------------
// D3D12
//--------------------------------------------------------------------------------------

struct D3D12BufferCreateInfo
{
	UINT64 size = 0;
	UINT64 alignment = 0;
	D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

	D3D12BufferCreateInfo() {}

	D3D12BufferCreateInfo(UINT64 InSize, D3D12_RESOURCE_FLAGS InFlags) : size(InSize), flags(InFlags) {}

	D3D12BufferCreateInfo(UINT64 InSize, D3D12_HEAP_TYPE InHeapType, D3D12_RESOURCE_STATES InState) :
		size(InSize),
		heapType(InHeapType),
		state(InState) {}

	D3D12BufferCreateInfo(UINT64 InSize, D3D12_RESOURCE_FLAGS InFlags, D3D12_RESOURCE_STATES InState) :
		size(InSize),
		flags(InFlags),
		state(InState) {}

	D3D12BufferCreateInfo(UINT64 InSize, UINT64 InAlignment, D3D12_HEAP_TYPE InHeapType, D3D12_RESOURCE_FLAGS InFlags, D3D12_RESOURCE_STATES InState) :
		size(InSize),
		alignment(InAlignment),
		heapType(InHeapType),
		flags(InFlags),
		state(InState) {}
};

struct D3D12ShaderCompilerInfo 
{
	dxc::DxcDllSupport		DxcDllHelper;
	IDxcCompiler*			compiler = nullptr;
	IDxcLibrary*			library = nullptr;
};

struct D3D12ShaderInfo 
{
	LPCWSTR		filename = nullptr;
	LPCWSTR		entryPoint = nullptr;
	LPCWSTR		targetProfile = nullptr;
	//LPCWSTR*	arguments = nullptr;
	//std::unique_ptr<LPCWSTR>	arguments = nullptr;
	std::vector<LPCWSTR> arguments;
	DxcDefine*	defines = nullptr;
	UINT32		argCount = 0; // unused, instead use (uint32)arguments.size()
	UINT32		defineCount = 0;

	D3D12ShaderInfo() {}
	D3D12ShaderInfo(LPCWSTR inFilename, LPCWSTR inEntryPoint, LPCWSTR inProfile)
	{
		filename = inFilename;
		entryPoint = inEntryPoint;
		targetProfile = inProfile;
	}


	// mstack adding constructor to pass debug
	D3D12ShaderInfo(LPCWSTR inFilename, LPCWSTR inEntryPoint, LPCWSTR inProfile, bool debug)
	{
		filename = inFilename;
		entryPoint = inEntryPoint;
		targetProfile = inProfile;
		//arguments = std::make_unique<LPCWSTR>(inDebug);
		arguments.push_back(L"-Zi");
		arguments.push_back(L"-Od");
		arguments.push_back(L"-T");
		arguments.push_back(L"ps_6_6");
		arguments.push_back(L"-Fo");
		arguments.push_back(L"C:\\Users\\matts\\source\\repos\\IntroToDXR_my_fork\\test\\test.bin");
		argCount = static_cast<UINT32>(arguments.size()); // not sure if this is needed
	}
};

struct D3D12Resources
{
	ID3D12Resource* DXROutput;
	ID3D12Resource* DXRAccBuffer;

	ID3D12Resource* vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW						vertexBufferView;
	ID3D12Resource* indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW							indexBufferView;

	ID3D12Resource* viewCB = nullptr;
	ViewCB											viewCBData;
	UINT8* viewCBStart = nullptr;

	ID3D12Resource* materialCB = nullptr;
	MaterialCB										materialCBData;
	UINT8* materialCBStart = nullptr;

	ID3D12Resource* myMaterialCB = nullptr;
	//MyMaterialCB myMaterialCBData; // original, wasnt making sense for the vec approach
	size_t myMaterialCBDataSize; // stores the size of the std::Vector of MyMaterialCB's
	UINT* myMaterialCBStart = nullptr;

	ID3D12Resource* miscBufferCB = nullptr;
	miscBuffer miscBufferData{}; // default init
	UINT* miscBufferCBStart = nullptr; // mapped gpu address


	ID3D12Resource* missShaderBufferCB = nullptr;
	missShaderBuffer missShaderBufferData{}; // default init
	UINT* missShaderBufferCBStart = nullptr; // mapped gpu address, CPU address to write to GPU memory with write-combining
	D3D12_GPU_DESCRIPTOR_HANDLE missShaderOffset;

	ID3D12DescriptorHeap* rtvHeap = nullptr;
	ID3D12DescriptorHeap* descriptorHeap = nullptr;

	ID3D12Resource* texture = nullptr;
	ID3D12Resource* textureUploadResource = nullptr;

	UINT											rtvDescSize = 0;

	float											translationOffset = 0;
	float											rotationOffset = 0;
	DirectX::XMFLOAT3								eyeAngle;
	DirectX::XMFLOAT3								eyePosition;

	ID3D12Resource* lightBufferCB {nullptr};
	LightPos										light_pos{};
	UINT* lightBufferCBStart{ nullptr }; // mapped gpu address
};

struct D3D12Global
{
	IDXGIFactory4*									factory = nullptr;
	IDXGIAdapter1*									adapter = nullptr;
	ID3D12Device5*									device = nullptr;
	ID3D12GraphicsCommandList4*						cmdList = nullptr;
	ID3D12CommandQueue*								cmdQueue = nullptr;
	ID3D12CommandAllocator*							cmdAlloc[2] = { nullptr, nullptr };

	IDXGISwapChain3*								swapChain = nullptr;
	ID3D12Resource*									backBuffer[2] = { nullptr, nullptr };

	ID3D12Fence*									fence = nullptr;
	UINT64											fenceValues[2] = { 0, 0 };
	HANDLE											fenceEvent;
	UINT											frameIndex = 0;

	// for acculumataltion
	//DirectX::XMMATRIX								last_matrix;
	DirectX::XMVECTOR								temp_cameraPos;
	DirectX::XMVECTOR								temp_cameraFront;
	float											has_moved = 0; // 1 for moved, 0 for not moved
	float											acc_counter = 0;


	int												width = 640;
	int												height = 360;
	bool											vsync = false;


#if defined(USE_NSIGHT_AFTERMATH)
	// App-managed marker functionality
	UINT64 m_frameCounter = 0; // techincally there is already a frame counter above
	GpuCrashTracker::MarkerMap m_markerMap;

	// Nsight Aftermath instrumentation
	GFSDK_Aftermath_ContextHandle m_hAftermathCommandListContext;
	GpuCrashTracker m_gpuCrashTracker{ m_markerMap };
#endif
};

//--------------------------------------------------------------------------------------
//  DXR
//--------------------------------------------------------------------------------------

struct AccelerationStructureBuffer
{
	ID3D12Resource* pScratch = nullptr;
	ID3D12Resource* pResult = nullptr;
	ID3D12Resource* pInstanceDesc = nullptr;	// only used in top-level AS
};

struct RtProgram
{
	D3D12ShaderInfo			info = {};
	IDxcBlob*				blob = nullptr;
	ID3D12RootSignature*	pRootSignature = nullptr;

	D3D12_DXIL_LIBRARY_DESC	dxilLibDesc;
	D3D12_EXPORT_DESC		exportDesc;
	D3D12_STATE_SUBOBJECT	subobject;
	std::wstring			exportName;

	RtProgram()
	{
		exportDesc.ExportToRename = nullptr;
	}

	RtProgram(D3D12ShaderInfo shaderInfo)
	{
		info = shaderInfo;
		//info = std::move(shaderInfo);
		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
		exportName = shaderInfo.entryPoint;
		exportDesc.ExportToRename = nullptr;
		exportDesc.Flags = D3D12_EXPORT_FLAG_NONE;
	}

	void SetBytecode()
	{
		exportDesc.Name = exportName.c_str();

		dxilLibDesc.NumExports = 1;
		dxilLibDesc.pExports = &exportDesc;
		dxilLibDesc.DXILLibrary.BytecodeLength = blob->GetBufferSize();
		dxilLibDesc.DXILLibrary.pShaderBytecode = blob->GetBufferPointer();

		subobject.pDesc = &dxilLibDesc;
	}

};

struct HitProgram
{
	RtProgram ahs;
	RtProgram chs;

	std::wstring exportName;
	D3D12_HIT_GROUP_DESC desc = {};
	D3D12_STATE_SUBOBJECT subobject = {};

	HitProgram() {}
	HitProgram(LPCWSTR name) : exportName(name)
	{
		desc = {};
		desc.HitGroupExport = exportName.c_str();
		subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
		subobject.pDesc = &desc;
	}

	void SetExports(bool anyHit)
	{
		desc.HitGroupExport = exportName.c_str();
		if (anyHit) desc.AnyHitShaderImport = ahs.exportDesc.Name;
		desc.ClosestHitShaderImport = chs.exportDesc.Name;
	}

};

struct DXRGlobal
{
	AccelerationStructureBuffer						TLAS;
	AccelerationStructureBuffer						BLAS;
	uint64_t										tlasSize;

	ID3D12Resource*									shaderTable = nullptr;
	uint32_t										shaderTableRecordSize = 0;

	RtProgram										rgs;
	RtProgram										miss;
	HitProgram										hit;

	// for shadows
	RtProgram										shadowmiss;
	HitProgram										shadowhit;

	ID3D12StateObject*								rtpso = nullptr;
	ID3D12StateObjectProperties*					rtpsoInfo = nullptr;
};
