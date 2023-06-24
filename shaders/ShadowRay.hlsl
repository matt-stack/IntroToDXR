#include "Common.hlsl"

// Shadow Ray
// Miss shader and AnyHit shader with RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH

// ---[ Closest Hit Shader ]---


[shader("anyhit")]
void ShadowHit(inout ShadowInfo payload, Attributes attrib)
{
//	uint triangleIndex = PrimitiveIndex();

//	float3 worldRayHit = worldHitPosition();
//	if (isTransparent(attrib, triangleIndex)) {
//		ignoreHit();
//	}
	payload.isVis = float4(0.1f, 0.3f, 0.3f, 0.f);
	//	IgnoreHit();
//	AcceptHitAndEndSearch();
}

// ---[ Miss Shader ]---

[shader("miss")]
void ShadowMiss(inout ShadowInfo payload)
{
    //payload.ShadedColorAndHitT = float4(0.2f, 0.2f, 0.2f, -1.f);
	payload.isVis = float4(1.f, 1.f, 0.f, 1.f);
}