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

#include "Common.hlsl"

// ---[ Closest Hit Shader ]---

[shader("closesthit")]
void ClosestHit(inout HitInfo payload, Attributes attrib)
{
	float3 finalColor;
	uint triangleIndex = PrimitiveIndex();
	float3 barycentrics = float3((1.0f - attrib.uv.x - attrib.uv.y), attrib.uv.x, attrib.uv.y); // w, u, v 
	VertexAttributes vertex = GetVertexAttributes(triangleIndex, barycentrics);
	TriangleVertex triVerts = GetVertexNormals(triangleIndex);
	float3 barycentric_normal = barycentricNormal(attrib.uv.x, attrib.uv.y, triVerts);

	float3 worldRayHit = worldHitPosition();

	int mat = GetMaterialId(triangleIndex);
	float3 RGB = GetMaterialDiffuse(mat);

	float3 camera_dir = WorldRayDirection();
	float DI = calcDI(barycentric_normal, camera_dir);

	RGB *= DI;

	// Likely you have a pool of canidate lights, then randomly sample one to launch a ray towards,
	// you implictly have the direction, and you launch a ray to check if anything is in the way of the light
	// ray (occulusion). If it is visible to the light source, then using the incoming light direction you 
	// can 

	// whether lights just points (like this case), or they have vertices, they can have extra info like
	// intensity, color, a texture, etc. You query this when you chose the light sample. 

	float occlusion_val = traceShadow();

	float AO_val = traceAO(barycentric_normal);

	//RGB + shadow contribution

	if (occlusion_val == 1.f) {
		finalColor =  float3( RGB.x * 1.f, RGB.y * 1.f, RGB.z * 1.f );
	}
	else {
		finalColor = float3( RGB.x * 0.5f, RGB.y * 0.5f, RGB.z * 0.5f );
	}

	// Adding AO contribution
	finalColor *= AO_val;

	// visualize normals
//	float3 normals = (barycentric_normal + 1) / 2;

	// visualize RayTCurrent, good for debugging
//	finalColor = float3((RayTCurrent() / 10.f), 0.f, 0.f);

	//float3 temp_color = my_lights.light_pos.xyz;

	//temp_color.x *= 10.0;
//	if (my_lights.y == -20.f) {
//		finalColor.xyz = 0.1f;
//	}

	payload.ShadedColorAndHitT = float4(finalColor.xyz, RayTCurrent());
	//payload.ShadedColorAndHitT = float4(temp_color.xyz, RayTCurrent());
//	payload.ShadedColorAndHitT = float4(AO_val.xyz, RayTCurrent());
//	payload.ShadedColorAndHitT = float4(normals.xyz, RayTCurrent());
}