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

// ---[ Structures ]---

struct HitInfo
{
	float4 ShadedColorAndHitT;
};

struct Attributes 
{
	float2 uv;
};

// ---[ Constant Buffers ]---
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-variable-register
cbuffer ViewCB : register(b0) // b means constant buffer
{
	matrix view;
	float4 viewOriginAndTanHalfFovY;
	float2 resolution;
};

cbuffer MaterialCB : register(b1)
{
	float3 textureResolution;
};



cbuffer MyMaterialCB : register(b2)
{
	//float3 materials[9];
	float4 materials[9];
};

//ByteAddressBuffer materials : register(b2);

// ---[ Resources ]---

RWTexture2D<float4> RTOutput				: register(u0);
RaytracingAccelerationStructure SceneBVH	: register(t0);

ByteAddressBuffer indices					: register(t1);
ByteAddressBuffer vertices					: register(t2); // where does this come from?
Texture2D<float4> albedo					: register(t3);

// ---[ Helper Functions ]---

struct VertexAttributes
{
	float3 position;
	float2 uv;
};

struct TriangleVertex 
{
	float3 firstVert;
	float3 secondVert;
	float3 thirdVert;
};

uint3 GetIndices(uint triangleIndex)
{
	uint baseIndex = (triangleIndex * 3);
	int address = (baseIndex * 4);
	return indices.Load3(address);
}

VertexAttributes GetVertexAttributes(uint triangleIndex, float3 barycentrics)
//VertexAttributes GetVertexAttributesWithBarycentrics(uint triangleIndex, float3 barycentrics)
{
	uint3 indices = GetIndices(triangleIndex);
	VertexAttributes v;
	v.position = float3(0, 0, 0);
	v.uv = float2(0, 0);

	for (uint i = 0; i < 3; i++)
	{
		int address = (indices[i] * 5) * 4;
		v.position += asfloat(vertices.Load3(address)) * barycentrics[i];
		address += (3 * 4); // this wont work anymore for uv because we swapped the Vertex structure
		v.uv += asfloat(vertices.Load2(address)) * barycentrics[i];
	}

	return v;
}


TriangleVertex GetVertexPos(uint triangleIndex)
// visualize vertex normals
{
	uint3 indices = GetIndices(triangleIndex);
	TriangleVertex v;

	int testing_address1 = indices[0] * (10 * 4) + (3*4); // 8 floats (size of the vertex struct) + (offset into the Normals member)
	int testing_address2 = indices[1] * (10 * 4) + (3*4); // these offset into the Normals field!
	int testing_address3 = indices[2] * (10 * 4) + (3*4);
	TriangleVertex vn;
	vn.firstVert =  asfloat(vertices.Load3(testing_address1));
	vn.secondVert = asfloat(vertices.Load3(testing_address2));
	vn.thirdVert =  asfloat(vertices.Load3(testing_address3));

	return vn;
}


int GetMaterialId(uint triangleIndex)
// visualize vertex normals
{
	uint3 indices = GetIndices(triangleIndex);

	int testing_address1 = indices[0] * (10 * 4) + (3*4) + (3*4); // 8 floats (size of the vertex struct) + (offset into the mat ID member)
	//int testing_address1 = indices[0] + (3*4) + (3*4); // 8 floats (size of the vertex struct) + (offset into the mat ID member)
	int matID =  asint(vertices.Load(testing_address1));

	return matID;
}
// something is worng with the materials, either indexing wrong or the values are bad,
// try using a DX12 debugger again?

float4 GetMaterialDiffuse(int matID) 
{
	float4 RGB = materials[matID];
	//float3 RGB = float3(0.0f, 0.f, 0.f);
	//float4 RGB = float4(0.0f, 0.f, 1.f, 0.f);

	return RGB;

}

float3 worldHitPosition() {

	float3 worldRayHit = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();

	return worldRayHit;
}

float3 CalculateSurfaceNormal(TriangleVertex tri) {
	float3 u = tri.secondVert - tri.firstVert;
	float3 v = tri.thirdVert - tri.firstVert;

	float3 normal = normalize(cross(u, v));
	//float3 normal = cross(u, v);
//	float3 normal = cross(v, u);

	return normal;
}