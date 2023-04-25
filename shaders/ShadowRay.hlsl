#include "Common.hlsl"

// Shadow Ray
// Miss shader and AnyHit shader with RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH

// ---[ Closest Hit Shader ]---


[shader("anyhit")]
void ShadowHit(inout ShadowInfo payload, Attributes attrib)
{
	uint triangleIndex = PrimitiveIndex();

	float3 worldRayHit = worldHitPosition();
//	if (isTransparent(attrib, triangleIndex)) {
//		ignoreHit();
//	}
	payload.isVis = 0;
}

// ---[ Miss Shader ]---

[shader("miss")]
void Miss(inout ShadowInfo payload)
{
    //payload.ShadedColorAndHitT = float4(0.2f, 0.2f, 0.2f, -1.f);
	payload.isVis = 1.f;
}