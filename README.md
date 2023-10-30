## notes

Currently getting this error:
```
D3D12 ERROR: ID3D12Device::CreateComputeShader: Shader must be cs_6_5, cs_6_4, cs_6_3, cs_6_2, cs_6_1, cs_6_0, cs_5_1, cs_5_0, cs_4_1, cs_4_0. Shader version provided: cs_6_3. [ STATE_CREATION ERROR #881: CREATECOMPUTEPIPELINESTATE_INVALID_SHADER]
```
Which needs some understanding. I dont think ID3D12DEVICE has a CreateComputeShader

# NRD- Sigma shadow denoising

Workflow of the NRD Sample

[NRDSample.cpp](https://github.com/matt-stack/NRDSample/blob/ec5bb2605650f6c54d0b9cd4e39ad1feac1ef507/Source/NRDSample.cpp#L4331)
```
NrdIntegration_SetResource(userPool, nrd::ResourceType::IN_SHADOWDATA, {&GetState(Texture::Unfiltered_ShadowData), GetFormat(Texture::Unfiltered_ShadowData)});
NrdIntegration_SetResource(userPool, nrd::ResourceType::IN_SHADOW_TRANSLUCENCY, {&GetState(Texture::Unfiltered_Shadow_Translucency), GetFormat(Texture::Unfiltered_Shadow_Translucency)});
NrdIntegration_SetResource(userPool, nrd::ResourceType::OUT_SHADOW_TRANSLUCENCY, {&GetState(Texture::Shadow), GetFormat(Texture::Shadow)});
```
[NRDSample.cpp](https://github.com/matt-stack/NRDSample/blob/ec5bb2605650f6c54d0b9cd4e39ad1feac1ef507/Source/NRDSample.cpp#L4552)
```
{Texture::Unfiltered_ShadowData, nri::AccessBits::SHADER_RESOURCE_STORAGE, nri::TextureLayout::GENERAL},
{Texture::Unfiltered_Shadow_Translucency, nri::AccessBits::SHADER_RESOURCE_STORAGE, nri::TextureLayout::GENERAL},
...
nri::TransitionBarrierDesc transitionBarriers = {nullptr, optimizedTransitions.data(), 0, BuildOptimizedTransitions(transitions, helper::GetCountOf(transitions), optimizedTransitions)};
NRI.CmdPipelineBarrier(commandBuffer, &transitionBarriers, nullptr, nri::BarrierDependency::ALL_STAGES);
...
NRI.CmdSetPipeline(commandBuffer, *Get(Pipeline::TraceOpaque));
...
NRI.CmdDispatch(commandBuffer, rectGridWmod, rectGridHmod, 1);
```
Transition two shadow textures to a writable state, then launch TraceOpaque which will edit them

[TraceOpaque.cs.hlsl](https://github.com/matt-stack/NRDSample/blob/ec5bb2605650f6c54d0b9cd4e39ad1feac1ef507/Shaders/TraceOpaque.cs.hlsl#L882)
```
// outputs
NRI_RESOURCE( RWTexture2D<float2>, gOut_ShadowData, u, 7, 1 );
NRI_RESOURCE( RWTexture2D<float4>, gOut_Shadow_Translucency, u, 8, 1 );

...

float2 shadowData0 = SIGMA_FrontEnd_PackShadow( viewZ, shadowHitDist == INF ? NRD_FP16_MAX : shadowHitDist, gTanSunAngularRadius, shadowTranslucency, shadowData1 );

gOut_ShadowData[ pixelPos ] = shadowData0;
gOut_Shadow_Translucency[ pixelPos ] = shadowData1;
```
`SIGMA_FrontEnv_PackShadow` call

[NRDSample.cpp](https://github.com/matt-stack/NRDSample/blob/ec5bb2605650f6c54d0b9cd4e39ad1feac1ef507/Source/NRDSample.cpp#L4571)
```
{ // Shadow denoising
helper::Annotation annotation(NRI, commandBuffer, "Shadow denoising");

 nrd::SigmaSettings shadowSettings = {};
 nrd::Identifier denoiser = NRD_ID(SIGMA_SHADOW_TRANSLUCENCY);

 m_NRD.SetDenoiserSettings(denoiser, &shadowSettings);
 m_NRD.Denoise(&denoiser, 1, commandBuffer, userPool, NRD_ALLOW_DESCRIPTOR_CACHING);

 //RestoreBindings(commandBuffer, frame); // Bindings will be restored in the next section
}
```
[NRDSample.cpp](https://github.com/matt-stack/NRDSample/blob/ec5bb2605650f6c54d0b9cd4e39ad1feac1ef507/Source/NRDSample.cpp#L4744)
```
{Texture::Shadow, nri::AccessBits::SHADER_RESOURCE, nri::TextureLayout::SHADER_RESOURCE},
...
nri::TransitionBarrierDesc transitionBarriers = {nullptr, optimizedTransitions.data(), 0, BuildOptimizedTransitions(transitions, helper::GetCountOf(transitions), optimizedTransitions)};
NRI.CmdPipelineBarrier(commandBuffer, &transitionBarriers, nullptr, nri::BarrierDependency::ALL_STAGES);

NRI.CmdSetPipeline(commandBuffer, *Get(Pipeline::Composition));
NRI.CmdSetDescriptorSet(commandBuffer, 1, *Get(DescriptorSet::Composition1), &kDummyDynamicConstantOffset);

NRI.CmdDispatch(commandBuffer, rectGridW, rectGridH, 1);
```
Transition new denoised shadow texture to texture layout from writeable form, then launch Composition (compute) shader

[Composition.cs.hlsl](https://github.com/matt-stack/NRDSample/blob/ec5bb2605650f6c54d0b9cd4e39ad1feac1ef507/Shaders/Composition.cs.hlsl#L67)
```
float4 shadowData = gIn_Shadow[ pixelPos ];
shadowData = SIGMA_BackEnd_UnpackShadow( shadowData );
float3 shadow = lerp( shadowData.yzw, 1.0, shadowData.x );
```
`SIGMA_BackEnd_UnpackShadow()` call here, use the denoised shadow texture in normal deferred fashion from here out!

Done :)

# Introduction To DirectX Raytracing

A barebones application to get you jump started with DirectX Raytracing (DXR)! Unlike other tutorials, this sample code _does not create or use any abstractions_ on top of the DXR Host API, and focuses on highlighting exactly what is new and different with DXR using the raw API calls. 

The code sample traces primary rays and includes a simple hit shader that samples a texture. 

![Release Mode](https://github.com/matt-stack/IntroToDXR/tree/master/img/AO.png "Release Mode Output")

## Requirements

* Windows 10 v1809, "October 2018 Update" (RS5) or later
* Windows 10 SDK v1809 (10.0.17763.0) or later. [Download it here.](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive) 
* Visual Studio 2017, 2019, or VS Code

## Code Organization

Data is passed through the application using structs. These structs are defined in `Structures.h` and are organized into three categories: 

* Global
* Standard D3D12
* DXR


Rendering code lives in `Graphics.h/cpp` and is broken into four namespaces to separate new DXR functionality from existing D3D12 functionality. 

### D3DResources
```c++
namespace D3DResources 
{
	void Create_Buffer(D3D12Global &d3d, D3D12BufferCreateInfo &info, ID3D12Resource** ppResource);
	void Create_Texture(D3D12Global &d3d, D3D12Resources &resources, Material &material);
	void Create_Vertex_Buffer(D3D12Global &d3d, D3D12Resources &resources, Model &model);
	void Create_Index_Buffer(D3D12Global &d3d, D3D12Resources &resources, Model &model);
	...
}
```
Contains common functions to create D3D12 resources like (vertex, index, constant) buffers, textures, samplers, and heaps.  


### D3DShaders
```c++
namespace D3DShaders 
{
	void Init_Shader_Compiler(D3D12ShaderCompilerInfo &shaderCompiler);
	void Compile_Shader(D3D12ShaderCompilerInfo &compilerInfo, RtProgram &program);
	...
}
```
Contains functions to initialize the DXCompiler as well as load and compile shaders (including - of course - the new DXR ray tracing shaders).

### D3D12
```c++
namespace D3D12 
{	
	void Create_Device(D3D12Global &d3d);
	void Create_CommandList(D3D12Global &d3d);
	...
}
```
Contains functions for common D3D12 setup operations like Device, Command List, Command Queue, Command Allocator, Fence, Swap Chain, and Root Signature creation. Note that small differences exist in Device and Command List creation when using DXR.

### DXR
```c++
namespace DXR
{
	void Create_Bottom_Level_AS(D3D12Global &d3d, DXRGlobal &dxr, D3D12Resources &resources, Model &model);
	void Create_Top_Level_AS(D3D12Global &d3d, DXRGlobal &dxr, D3D12Resources &resources);
	void Create_RayGen_Program(D3D12Global &d3d, DXRGlobal &dxr, D3D12ShaderCompilerInfo &shaderCompiler);
	void Create_Miss_Program(D3D12Global &d3d, DXRGlobal &dxr, D3D12ShaderCompilerInfo &shaderCompiler);
	void Create_Closest_Hit_Program(D3D12Global &d3d, DXRGlobal &dxr, D3D12ShaderCompilerInfo &shaderCompiler);
	void Create_Pipeline_State_Object(D3D12Global &d3d, DXRGlobal &dxr);
	void Create_Shader_Table(D3D12Global &d3d, DXRGlobal &dxr, D3D12Resources &resources);	
	...
}
```
Contains new functionality specific to DirectX Raytracing. This includes acceleration structure creation, shader table creation and update, ray tracing pipeline state object (RTPSO) creation, and ray tracing shader loading and compilation. 

## Command Line Arguments

* `-width [integer]` specifies the width (in pixels) of the rendering window
* `-height [integer]` specifies the height (in pixels) of the rendering window
* `-vsync [0|1]` specifies whether vsync is enabled or disabled
* `-model [path]` specifies the file path to a OBJ model

## Suggested Exercises
After building and running the code, first thing I recommend you do is load up the Nsight Graphics project file (IntroToDXR.nsight-gfxproj), and capture a frame of the application running. This will provide a clear view of exactly what is happening as the application is running. [You can download Nsight Graphics here](https://developer.nvidia.com/nsight-graphics).

Once you have a good understanding of how the application works, I encourage you to dig deeper into DXR by removing limitations of the current code and adding new rendering features. I suggest:

* Add antialiasing by casting multiple rays per pixel.
* Add loading and rendering of models with multiple materials (only a single material is supported now)
* Add realistic lighting and shading (lighting is currently baked!)
* Add ray traced shadows. _Extra credit:_ use Any-Hit Shaders for shadow rendering
* Add ray traced ambient occlusion.
* Add ray traced reflections for mirror-like materials.
* Add camera translation and rotation mapped to keyboard and mouse inputs.

If you've implemented the above features, you'll be just a few steps away from your very own DXR path tracer!

* Implement [Ray Tracing In One Weekend](https://www.amazon.com/Ray-Tracing-Weekend-Minibooks-Book-ebook/dp/B01B5AODD8/ref=sr_1_1?ie=UTF8&qid=1540494705&sr=8-1&keywords=ray+tracing+in+one+weekend)

## Licenses and Open Source Software

The code uses two dependencies:
* [TinyObjLoader](https://github.com/syoyo/tinyobjloader-c/blob/master/README.md), provided with an MIT license. 
* [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h), provided with an MIT license.

The repository includes assets for use when testing the renderer:
* [Statue Image](https://pixabay.com/en/statue-sculpture-figure-1275469/), by Michael Gaida, licensed under a [CC0 1.0 Creative Commons Universal Public Domain Dedication License](https://creativecommons.org/publicdomain/zero/1.0/deed.en). 
* [Peerless Magnarc Cinema Projectors](https://sketchfab.com/models/62046af7d4f84b4ebe01d44f54970bc1), by Miguel Bandera, licensed under a [Creative Commons Attribution 4.0 International License](https://creativecommons.org/licenses/by/4.0/). 

