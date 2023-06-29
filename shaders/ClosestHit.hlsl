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
	uint triangleIndex = PrimitiveIndex();
	float3 barycentrics = float3((1.0f - attrib.uv.x - attrib.uv.y), attrib.uv.x, attrib.uv.y); // w, u, v 
	VertexAttributes vertex = GetVertexAttributes(triangleIndex, barycentrics);
	TriangleVertex triVerts = GetVertexNormals(triangleIndex);
	float3 barcentric_normal = barycentricNormal(attrib.uv.x, attrib.uv.y, triVerts);

	float3 worldRayHit = worldHitPosition();

	int mat = GetMaterialId(triangleIndex);
	float3 RGB = GetMaterialDiffuse(mat);

	float occlusion_val = traceShadow();

	float AO_val = traceAO(barcentric_normal);

	float3 finalColor;

	//RGB + shadow contribution

	//if (shadowPayload.isVis.x == 1.f) {
	if (occlusion_val == 1.f) {
		finalColor =  float3( RGB.x * 1.f, RGB.y * 1.f, RGB.z * 1.f );
	}
	else {
		finalColor = float3( RGB.x * 0.5f, RGB.y * 0.5f, RGB.z * 0.5f );
	}

	finalColor *= AO_val;
		
	
	payload.ShadedColorAndHitT = float4(finalColor.xyz, RayTCurrent());
//	payload.ShadedColorAndHitT = float4(RGB.xyz, RayTCurrent());
}