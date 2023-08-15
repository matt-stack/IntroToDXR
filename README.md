# PBR study

File format docs: https://pbrt.org/fileformat-v4#describing-the-scene

cmd/pbrt.cpp, [line 282](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/cmd/pbrt.cpp#L282)

* Main, and where the Initialization happens. BasicScene, BasicSceneBuilder are created here, and called in ParseFile. After, the scene is rendered with RenderWaveFront or RenderCPU


parser.cpp, [line 1842](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/parser.cpp#L1842) to write PLY files

scene.cpp, [line 261]((https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/scene.cpp#L261) if PLY files exist

* The two Shape functions: Following ParseFile -> Parse -> switch to Attribute S for Shape -> there are two paths, depending what was called from Main. If the object files have already been created (.ply) then it passes `BasicSceneBuilder` through as the `Parse`'s `ParserTarget`. Then we encounter [Shape()](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/scene.cpp#L261) for BasicSceneBuilder. Which adds to the new entity to the ongoing `shape` vector in the BasicSceneBuilder [here](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/scene.cpp#L305).  Otherwise back in main if the ply files arent there, then it passes FormattingParseTarget, which ... basicParamListEntryPoint lambda, you get to the call that starts the triangle mesh. FormattingParserTarget is a child class of ParserTarget, so it can be called in basicParamListEntry's (ParserTarget::*apiFunc). The line in main that says "Parse provided scene description files" is for the .ply files, because if you follow through the FormattingParserTarget::Shape [here](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/parser.cpp#L1843) then you see it calls the WritePLY function, and generally talks about creating PLY files. The geometry pbrt file `geometry.pbrt` includes the calls to the `geometry/mesh_0001.ply` and other mesh information. When it takes the `else` in main, it is expecting to use the pbrt files, which reference individual `.ply` files. The point of the `.ply` files is to remove the geom data from the pbrt file and store it in a file, to remove bulk for better readability

scene.cpp, [line 305](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/scene.cpp#L305) 

* Adding new shapes to the ongoing shape vector for the BasicSceneBuilder. These shapes are referenced later in the aggregate file, which creates and owns the BLAS GPU memory of the triangles.

wavefront/wavefront.cpp, [line 14](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/wavefront/wavefront.cpp#L14)

* This is the RenderWaveFront() function that is called by main, and it owns the creation of the WavefrontPathIntegrator (and the constructor [here](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/wavefront/integrator.cpp#L80) calls to aggregiate, which iits own constructor has many important functions like creating the Cuda memory) and actually calls integrator->render()

wavefront/integrator.cpp, [line 80](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/wavefront/integrator.cpp#L80)

* WavefrontPathIntegrator constructor, which creates textures, lights, materials, etc, and critcially calls the OptiXAggregate constructor [here](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/wavefront/integrator.cpp#L162) on the GPU path

gpu/aggregate.cpp, [line 1159](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/gpu/aggregate.cpp#L1159)

* OptiXAggregate constructor, which calls buildBVHForTriangles on [line 1374](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/gpu/aggregate.cpp#L1374), which goes to same file

gpu/aggregate.cpp, [line 339](https://github.com/mmp/pbrt-v4/blob/f94d39f8d908752513104d815e66188f5585f446/src/pbrt/gpu/aggregate.cpp#L339)

* Building BVH and Creating GPU memory for both bounding b's and meshes. IMPORTANT: reponsible for cycling through mesh vector and creating the GPU memory

gpu/aggregate.cpp [line ](https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/gpu/aggregate.cpp#L460)

* The device memory is created with cudaMalloc and the pointer `point3f *pGPU` is pushed into the vector `std::vector<CUdeviceptr> pDeviceDevicePtrs(nMeshes)` via a `CUdeviceptr` call of pGPU

* In regards to reading the PLY meshes and getting indices/vertex, (ReadPLY)[https://github.com/mmp/pbrt-v4/blob/ce95acb9b94fdd2f4918bca643310ccfd481b1fa/src/pbrt/util/mesh.cpp#L322] as part of `TriQuadMesh` is the source of parsing the ply files and reading out to mesh. It uses the rply header functions calls. The ReadPLY meshis called from the PreparePLYMeshes in gpu/aggregate (here)[https://github.com/mmp/pbrt-v4/blob/ce95acb9b94fdd2f4918bca643310ccfd481b1fa/src/pbrt/gpu/aggregate.cpp#L266]. `TriQuadMesh` is defined in mesh.h (here)[https://github.com/mmp/pbrt-v4/blob/ce95acb9b94fdd2f4918bca643310ccfd481b1fa/src/pbrt/util/mesh.h#L82] and has vectors for points, normals, and indices.  

There are two places where ReadPLY is called that make sense, on is during `Shape::Create(..)` and the other is in `PreparePLYMesh`. The `PreparePLYMesh` is the one that is in the main flow, the other call seems revelant but is called in the chain for `buildBVHForQuadrics`, where the other one is called during the ocnstructor for OptiXAggregate, which is in the flow (here)[https://github.com/mmp/pbrt-v4/blob/ce95acb9b94fdd2f4918bca643310ccfd481b1fa/src/pbrt/gpu/aggregate.cpp#L1383]. 


Key data structures are 
BasicSceneBuilder - this is used as the original state when parsing. Holds `GraphicsState graphicsState` and `std::vector<ShapeSceneEntity> shapes`, plus the camera and integrator as `SceneEntity`. BasicSceneBuilder has a member function BasicSceneBuilder::Shape that is called when parsing [here](https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/parser.cpp#L906)

ShapeSceneEntity - this is like a descriptor (in dx12) to a shape, it holds all the infomation that was passed from Parse via the `ParsedParameterVector params`. The member `entity` of type `ShapeSceneEnity` is created in Shape() and passed to the `shapes` vector owned in BasicSceneBuilder

TriangleMesh - Contains info like [points](https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/util/mesh.h#L41), normals, uv's. Very important. In aggregate's BuildBVHForTriangle(), a TriangleMesh `mesh` is create via [Triangle::CreateMesh](https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/shapes.cpp#L372) and called [here](https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/gpu/aggregate.cpp#L372) using the ShapeSceneEntity vector `shapes`. the `mesh` is pushed onto a `std::Vector<TriangleMesh *> meshes` [here](https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/gpu/aggregate.cpp#L430). Then later that `meshes` vector is cycled through and the GPU memory is created for vertex and indices [here](https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/gpu/aggregate.cpp#L457)

RenderTarget- BasicSceneBuilder extends from the abstract class RenderTarget. RenderTarget has many pure virutal functions that BasicSceneBuilder needs to implmenet so that it can be a non-abstract class like used in main. 

CUdeviceptr - after pDeviceDevicePtrs
 

Random Notes

Transformations are done via the Transform class, but the owner of the matrix is actually graphicsState cta (member of BasicSceneBuilder), and then that is applied to the ubnderlying tranform saved in renderFromWorld in BasicSceneBuilder. 

The flow of seeing a Tranform keyword in the .pbrt file until it is applied to the object is following:

First, in parser.cpp, the parse() function has a Tranform section id, where it hands the rest. Inside, it declares an array of 16 floats called `m` (here)[https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/parser.cpp#L938]. `m` is populated via `parseFloat()`  and then it sends `m` over to BasicSceneBuilder's Transform function (here)[https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/parser.cpp#L943]

In the implmention of BasicSceneBuilder's Transform function (here)[https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/scene.cpp#L577], all it is doing is calling the BasicSceneBuilder member function of graphicState.ForActiveTransforms, which takes a lambda, and it that returns a SquareMatrix via the (Transpose())[https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/util/math.h#L1441] given the values it recieved via parse()'s float[16] m argument and tr parameter. This SquareMatrix returned in the lambda is used in `ForActiveTransforms()` to used to set the `TransformSet cta` 

Over to BasicSceneBuilder's Shape() function, it passes along the `renderFromObject` and `objectFromRender` class Tranform variables ((these)[https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/scene.cpp#L293]), both are gotten from BasicSceneBuilders's GraphicState's RenderFromObject() function (here)[https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/scene.h#L475]. Then these variables hold the tranforms are passed to the constructor of ShapeSceneEntity entity and pushed onto the main `shapes` vector!

(Btw, when it is referenced as `class Transform` it is called an (Elaborated type specifier)[https://en.cppreference.com/w/cpp/language/elaborated_type_specifier]) 

The renderFromObject() portion of BasicSceneBuilder::Shape() has a reference to both renderFromWorld and graphicsState.ctm. (renderFromWorld)[https://github.com/mmp/pbrt-v4/blob/05ff05e1ded8299b1de0eb8ee6c11f192d0a64dd/src/pbrt/scene.h#L476] is set during the camera option, and depending on the renderingstate set it can return a default value of Transform. So renderFromWorld can be a default init Transform, and then ctm is the per object transform to that world space.

Note, a pstd::Span, like other spans, is just a ptr and size, plus some helpful functions. And you dont need C++20 like the std version. 

For Windows File opening/reading, this is the correct path syntax (after trying every combination. Docs says you can use either \ or /, but only / worked for me)
```
CreateFileW(L"C://Users/matts/source/repos/pbrt-v4-scenes/book.pbrt", GENERIC_READ, FILE_SHARE_READ, 0,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
```

When its time for textures, need to implement BasicScene::AddFloatTexture and AddSpectrumTexture (here)[https://github.com/mmp/pbrt-v4/blob/ce95acb9b94fdd2f4918bca643310ccfd481b1fa/src/pbrt/scene.cpp#L912]




# Introduction To DirectX Raytracing

A barebones application to get you jump started with DirectX Raytracing (DXR)! Unlike other tutorials, this sample code _does not create or use any abstractions_ on top of the DXR Host API, and focuses on highlighting exactly what is new and different with DXR using the raw API calls. 

The code sample traces primary rays and includes a simple hit shader that samples a texture. 

![Release Mode](https://github.com/acmarrs/IntroToDXR/blob/master/IntroToDXR.png "Release Mode Output")

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

