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
	//float3 materials[9]; this was under 16 bytes and thus the bits were messed up
	float4 materials[9];
};

cbuffer miscBuffer : register(b3)
{
	float4 frame_counter;// = { framecounter, 0, 0, 0};
	float2 has_moved;// = { 0, 1} 
	//x: 1 for moved, dont acculmulate, 0 for has not moved, accululatate
	//y: acc counter
	float3 rgB;// = { 0, 0, 1 };
//	float1 one;
	float2 rGe;// = { 0, 1 };
};

//ByteAddressBuffer materials : register(b2);

// ---[ Resources ]---

RWTexture2D<float4> RTOutput				: register(u0);
RWTexture2D<float4> RTAccBuffer				: register(u1);
RaytracingAccelerationStructure SceneBVH	: register(t0);

ByteAddressBuffer indices					: register(t1);
ByteAddressBuffer vertices					: register(t2); // where does this come from?
Texture2D<float4> albedo					: register(t3);

// ---[ Helper Functions ]---

bool equals(float3 t1, float3 t2) {
	return (t1.x == t2.x && t1.y == t2.y && t1.z == t2.z);

}

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
		//int address = (indices[i] * 5) * 4;
		int address = (indices[i] * 10) * 4;
		v.position += asfloat(vertices.Load3(address)) * barycentrics[i];
		//address += (3 * 4); // this wont work anymore for uv because we swapped the Vertex structure
		address += (8 * 4); // this wont work anymore for uv because we swapped the Vertex structure
		v.uv += asfloat(vertices.Load2(address)) * barycentrics[i];
	}

	return v;
}


TriangleVertex GetVertexNormals(uint triangleIndex)
// visualize vertex normals
{
	uint3 indices = GetIndices(triangleIndex);
	TriangleVertex v;

	int testing_address1 = indices[0] * (10 * 4) + (3*4); // 10 floats (size of the vertex struct) + (offset into the Normals member)
	int testing_address2 = indices[1] * (10 * 4) + (3*4); // these offset into the Normals field!
	int testing_address3 = indices[2] * (10 * 4) + (3*4);
	TriangleVertex vn;
	vn.firstVert =  asfloat(vertices.Load3(testing_address1));
	vn.secondVert = asfloat(vertices.Load3(testing_address2));
	vn.thirdVert =  asfloat(vertices.Load3(testing_address3));

	return vn;
}

float3 barycentricNormal(float u, float v, TriangleVertex vn) {
	// uv should be from Attributes from CHS, split up
	// u, v, 1-u-v
	return (u * vn.firstVert + v * vn.secondVert + (1 - u - v) * vn.thirdVert);
}


int GetMaterialId(uint triangleIndex)
// visualize vertex normals
{
	uint3 indices = GetIndices(triangleIndex);

	//int testing_address1 = indices[0] * (10 * 4) + (3*4) + (3*4); // 8 floats (size of the vertex struct) + (offset into the mat ID member)
	int testing_address1 = indices[0] * (10 * 4) + (3*4) + (3*4); // 8 floats (size of the vertex struct) + (offset into the mat ID member)
	//int testing_address1 = indices[0] + (3*4) + (3*4); // 8 floats (size of the vertex struct) + (offset into the mat ID member)
	int matID =  asint(vertices.Load(testing_address1));

	return matID;
}
// something is worng with the materials, either indexing wrong or the values are bad,
// try using a DX12 debugger again?

float4 GetMaterialDiffuse(int matID) 
{
	float4 RGB = materials[matID]; // working one, commented out for miscBuffer testing
//	float4 RGB = float4(rGe, 0, 0); // testing
	//float4 RGB = float4(rgB, 0); // testing

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

	return normal;
}

// ---Shadow Stuff---

struct ShadowInfo {
	float4 isVis; // 1 for visible, 0 for in shadow
	// this is currently float4 when it does not have to be, because testing
};

struct Light {
	float4 posAndIntensity; // first 3 for x,y,z fourth for intensity
};


/*
Notes-
Btw, this looks like a good Vulkan tutorial if ever needed
https://github.com/SaschaWillems/Vulkan/tree/master/data/shaders/glsl/raytracingshadows

*/



// next four functions are academically stolen from Chris Wymans Intro to DXR presentation :)
// Generates a seed for a random number generator from 2 inputs plus a backoff
uint initRand(uint val0, uint val1, uint backoff = 16)
{
	uint v0 = val0, v1 = val1, s0 = 0;

	[unroll]
	for (uint n = 0; n < backoff; n++)
	{
		s0 += 0x9e3779b9;
		v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
		v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
	}
	return v0;
}

// Takes our seed, updates it, and returns a pseudorandom float in [0..1]
float nextRand(inout uint s)
{
	s = (1664525u * s + 1013904223u);
	return float(s & 0x00FFFFFF) / float(0x01000000);
}


float2 getJitter() {
	uint2 launchIndex = DispatchRaysIndex();
	uint2 launchDim = DispatchRaysDimensions();
	uint rand_seed = initRand(launchIndex.x + launchIndex.y * launchDim.x, frame_counter.x, 16);
	float2 rand_val = float2(nextRand(rand_seed), nextRand(rand_seed)); //rand_val = [0..1]
	rand_val -= 0.5f; // we want rand_val = [-0.5..0.5]
	return rand_val;
}

// Utility function to get a vector perpendicular to an input vector 
//    (from "Efficient Construction of Perpendicular Vectors Without Branching")
float3 getPerpendicularVector(float3 u)
{
	float3 a = abs(u);
	uint xm = ((a.x - a.y) < 0 && (a.x - a.z) < 0) ? 1 : 0;
	uint ym = (a.y - a.z) < 0 ? (1 ^ xm) : 0;
	uint zm = 1 ^ (xm | ym);
	return cross(u, float3(xm, ym, zm));
}

// Get a cosine-weighted random vector centered around a specified normal direction.
float3 getCosHemisphereSample(inout uint randSeed, float3 hitNorm)
{
	// Get 2 random numbers to select our sample with
	float2 randVal = float2(nextRand(randSeed), nextRand(randSeed));

	// new coord system is normal, bitangent and tangent as axes

	// Cosine weighted hemisphere sample from RNG
	float3 bitangent = getPerpendicularVector(hitNorm);
	float3 tangent = cross(bitangent, hitNorm);
	float r = sqrt(randVal.x);
	float phi = 2.0f * 3.14159265f * randVal.y;

	// Get our cosine-weighted hemisphere lobe sample direction
	return tangent * (r * cos(phi).x) + bitangent * (r * sin(phi)) + hitNorm.xyz * sqrt(1 - randVal.x);
}

// ----------------- [SHADOWS]-------------------

// traceShadow return float (from shadowpayload) because we only want to know if its occluded
float traceShadow() {
	float occlusion_val;

	// Setup Shadow Ray
	RayDesc shadowRay;
	shadowRay.Origin = worldHitPosition();
	//shadowRay.Origin = float3(0.f, 0.f, 0.f);
	//float4 mainLight = float4(10.f, 10.f, 10.f, 1.f);
	//float4 mainLight = float4(2.f, 5.f, 0.f, 1.f); // this is good front on light from angle
	float4 mainLight = float4(5.f, -20.f, 0.f, 1.f); 
	shadowRay.Direction = normalize(mainLight.xyz - shadowRay.Origin);
	//shadowRay.Direction = float3(5.f, -5.f, 5.f);

	shadowRay.TMin = 0.00001f;
	shadowRay.TMax = 100.f;	
	
	// Trace the ray
	ShadowInfo shadowPayload;
	shadowPayload.isVis = float4(0.f, 0.f, 0.f, 0.f);

	TraceRay(
		SceneBVH,
		RAY_FLAG_SKIP_CLOSEST_HIT_SHADER |
		RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH,
		0xFF,
		1, // this does not actually use a hit group, because Opaque geom and null CHS
		2,
		1,
		shadowRay,
		shadowPayload);

	occlusion_val = shadowPayload.isVis.x;

	return occlusion_val;
}

// ------------------- [AO]----------------------
// uses shadow miss and any_hit (it doesnt actually use any_hit / hit group because )
float traceAO(float3 barcentric_normal, int num_rays=32, float AO_range=2.f) {
//float3 traceAO(float3 barcentric_normal, int num_rays=32, float AO_range=2.f) {
	float AO_val = 0.f;

	// Where is this thread's ray on screen? similar to threadIdx.x and BlockDim.x?
	uint2 launchIndex = DispatchRaysIndex();
	uint2 launchDim = DispatchRaysDimensions();

	// Initialize a random seed, per-pixel, based on a screen position and temporally varying count
	uint randSeed = initRand(launchIndex.x + launchIndex.y * launchDim.x, frame_counter.x, 16);

//	float3 worldDir_T = getCosHemisphereSample(randSeed, barcentric_normal);
	for (int i = 0; i < num_rays; i++) {

		float3 worldDir = getCosHemisphereSample(randSeed, barcentric_normal);

		// Setup AO Ray
		RayDesc AORay;
		AORay.Origin = worldHitPosition();
		AORay.Direction = worldDir;
		//AORay.Direction = float3(5.f, -15.f, 0.f);
		//AORay.Direction = normalize(worldDir - AORay.Origin); // WorldDir is already a direction, dont need this

		AORay.TMin = 0.000001f;
		AORay.TMax = AO_range;	
	
		// Trace the ray
		ShadowInfo AOPayload;
		AOPayload.isVis = float4(0.f, 0.f, 0.f, 0.f);

		TraceRay(
			SceneBVH,
			RAY_FLAG_SKIP_CLOSEST_HIT_SHADER |
			RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH,
			0xFF,
			1, // this does not actually use a hit group, because Opaque geom and null CHS
			2,
			1,
			AORay,
			AOPayload);

		AO_val += AOPayload.isVis.x;
	}

	AO_val /= (float)num_rays;
	//AO_val = 0.2f;

//	float3 vis_rand_direction = worldDir_T;

//	return (vis_rand_direction + 1) / 2;

	return AO_val; // normalized by num_rays
}
