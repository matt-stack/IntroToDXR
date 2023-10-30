[numthreads(16, 16, 1)]
void main( int2 pixelPos : SV_DispatchThreadID )
{
	// composition will take:
	// in: gBuffer, shadowTex as SRV, Texture2d
	// out: CSOutput as UAV, RWtexture2d

}