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

#include "Window.h"
#include "Graphics.h"
#include "Utils.h"
#include "Windowsx.h"
#include <directxmath.h>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

using namespace DirectX;

/**
 * Your ray tracing application!
 */
class DXRApplication
{
public:
	
	void Init(ConfigInfo &config) 
	{		
		// Create a new window
		HRESULT hr = Window::Create(config.width, config.height, config.instance, window, L"Introduction to DirectX Raytracing (DXR)");
		Utils::Validate(hr, L"Error: failed to create window!");

		d3d.width = config.width;
		d3d.height = config.height;
		d3d.vsync = config.vsync;

		// Load a model
		//Utils::LoadModel(config.model, model, material); // config.model is a string, model is a Model
		Utils::LoadModel(config.model, model, material, myMaterialVector); // config.model is a string, model is a Model
	//	Utils::CustomModel(model); // config.model is a string, model is a Model

		// Initialize the shader compiler
		D3DShaders::Init_Shader_Compiler(shaderCompiler);

		// Initialize D3D12
#ifdef _PROFILE
	//	D3D12::Load_PIX();
#endif
		D3D12::Create_Device(d3d);
		D3D12::Create_Command_Queue(d3d);
		D3D12::Create_Command_Allocator(d3d);
		D3D12::Create_Fence(d3d);		
		D3D12::Create_SwapChain(d3d, window);
		D3D12::Create_CommandList(d3d);
		D3D12::Reset_CommandList(d3d);

		// Create common resources
		D3DResources::Create_Descriptor_Heaps(d3d, resources);
		D3DResources::Create_BackBuffer_RTV(d3d, resources);
		D3DResources::Create_Vertex_Buffer(d3d, resources, model);
		D3DResources::Create_Index_Buffer(d3d, resources, model);
//		D3DResources::Create_Texture(d3d, resources, material);
		D3DResources::Create_View_CB(d3d, resources);
		D3DResources::Create_Material_CB(d3d, resources, material);
		D3DResources::Create_My_Material_CB(d3d, resources, myMaterialVector, myMaterialVector.size());
		D3DResources::Create_MiscBuffer_CB(d3d, resources);
		
		// Create DXR specific resources
		DXR::Create_Bottom_Level_AS(d3d, dxr, resources, model);
		DXR::Create_Top_Level_AS(d3d, dxr, resources);
		DXR::Create_DXR_Output(d3d, resources);
		DXR::Create_DXR_Acc_Buffer(d3d, resources);
		DXR::Create_Descriptor_Heaps(d3d, dxr, resources, model);	
		DXR::Create_RayGen_Program(d3d, dxr, shaderCompiler);
		DXR::Create_Miss_Program(d3d, dxr, shaderCompiler);
		DXR::Create_Closest_Hit_Program(d3d, dxr, shaderCompiler);
		DXR::Create_Shadow_Miss_Program(d3d, dxr, shaderCompiler);
		DXR::Create_Shadow_Any_Hit_Program(d3d, dxr, shaderCompiler);
		DXR::Create_Pipeline_State_Object(d3d, dxr);
		DXR::Create_Shader_Table(d3d, dxr, resources);

		d3d.cmdList->Close();
		ID3D12CommandList* pGraphicsList = { d3d.cmdList };
		d3d.cmdQueue->ExecuteCommandLists(1, &pGraphicsList);

		D3D12::WaitForGPU(d3d);
		D3D12::Reset_CommandList(d3d);
	}
	
	void Update(float movement, float mousex, float mousey, float mousez) 
	{
		D3DResources::Update_View_CB(d3d, resources, movement, mousex, mousey, mousez);


		D3DResources::Update_Misc_CB(d3d, resources);

	}

	void Render() 
	{		
		DXR::Build_Command_List(d3d, dxr, resources);
		D3D12::Present(d3d);
		D3D12::MoveToNextFrame(d3d);
		D3D12::Reset_CommandList(d3d);
	}

	void Cleanup() 
	{
		D3D12::WaitForGPU(d3d);
		CloseHandle(d3d.fenceEvent);

		DXR::Destroy(dxr);
		D3DResources::Destroy(resources);		
		D3DShaders::Destroy(shaderCompiler);
		D3D12::Destroy(d3d);

		DestroyWindow(window);
	}
	
	HWND& getWindow() {
		return this->window;
	}

private:
	HWND window;
	Model model;
//	ModelNorms model;
	Material material;

	std::vector<MyMaterialCB> myMaterialVector;

	DXRGlobal dxr = {};
	D3D12Global d3d = {};
	D3D12Resources resources = {};
	D3D12ShaderCompilerInfo shaderCompiler;
};

struct vec3 {
	float x;
	float y;
	float z;
};

/**
 * Program entry point.
 */
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) 
{	
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HRESULT hr = EXIT_SUCCESS;
	{
		MSG msg = { 0 };

		// Get the application configuration
		ConfigInfo config;
		hr = Utils::ParseCommandLine(lpCmdLine, config); // this sets the model str to the path in cmdline
		if (hr != EXIT_SUCCESS) return hr;

		// Initialize
		DXRApplication app;
		app.Init(config);
		float movement = 0;
		float mousex = 0;
		float mousey = 0;
		float mousez = 0;
		float sensitivity = 0.001f;

		float xoffset{};
		float yoffset{};

		bool mouseDown = false;
		bool firstMouse = true;

		int moving = 0;

		int	width = 640;
		int	height = 360;

		int xPos = 640;
		int yPos = 360;
		POINT point{};
		POINT pointStart{};
		POINT pointEnd{};

		//vec3 direction{};

		DirectX::XMVECTOR direction{};
		DirectX::XMVECTOR finalDirection{};

		XMFLOAT4 tempvec{};
		XMFLOAT4 tempvec_2{};

		float yaw{260};
		float pitch{10};

		// Main loop
		// https://gamedev.stackexchange.com/questions/192288/why-the-mouse-doesnt-move-in-mouse-look-feature-of-rpg-fps-games
		while (WM_QUIT != msg.message) 
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			switch (msg.message){
			case WM_KEYDOWN:
				moving++;
				if (msg.wParam == 0x57) movement = 4; // W
				if (msg.wParam == 0x41) movement = 3; // A
				if (msg.wParam == 0x53) movement = 2; // S
				if (msg.wParam == 0x44) movement = 1; // D
				if (msg.wParam == 0x45) movement = 5; // E
				if (msg.wParam == 0x51) movement = 6; // Q
				if (msg.wParam == VK_ESCAPE) PostQuitMessage(0); // Q
				if (msg.wParam == VK_SPACE) movement = 0; // Q
				break;
			case WM_KEYUP:
				moving--;
				if (moving <= 0) {
					movement = 0;
				}
				break;
			case WM_RBUTTONDOWN:
				mouseDown = true;
			//	SetCapture(app.getWindow());
			//	pointStart = MAKEPOINTS(msg.lParam);
				GetCursorPos(&pointStart);
				xPos = point.x;
				yPos = point.y;
				//xPos = GET_X_LPARAM(msg.lParam);
				//yPos = GET_Y_LPARAM(msg.lParam);
				break;
			case WM_RBUTTONUP:
				mouseDown = false;
				break;
			case WM_MOUSEMOVE:
				if (mouseDown) {
					if (firstMouse)
					{
						pointStart.x = xPos;
						pointStart.y = yPos;
						//pointStart.x = pointEnd.x;
						//pointStart.y = pointEnd.y;
						firstMouse = false;
					}

					GetCursorPos(&pointEnd);
					xoffset = pointStart.x - pointEnd.x;
					yoffset = pointStart.y - pointEnd.y;

					xoffset *= sensitivity;
					yoffset *= sensitivity;

					yaw += xoffset;
					pitch += yoffset;

					if (pitch > 89.0f) {
						pitch = 89.0f;
					}
					if (pitch < -89.0f) {
						pitch = -89.0f;
					}
				}
				break;
			default:
				break;
			}

			// convert to correct view



			tempvec.x = cos(DirectX::XMConvertToRadians(yaw)) * cos(DirectX::XMConvertToRadians(pitch));
			tempvec.y = sin(DirectX::XMConvertToRadians(pitch));
			if (firstMouse)
				tempvec.z = -1.f;
			else 
				tempvec.z = sin(DirectX::XMConvertToRadians(yaw)) * cos(DirectX::XMConvertToRadians(pitch));

			direction = XMLoadFloat4(&tempvec);

			finalDirection= XMVector3Normalize(direction);

			XMStoreFloat4(&tempvec_2, finalDirection);

			mousex = tempvec_2.x;
			mousey = tempvec_2.y;
			mousez = tempvec_2.z;
			
			app.Update(movement, mousex, mousey, mousez);
			app.Render();
		}

		app.Cleanup();
	}

#if defined _CRTDBG_MAP_ALLOC
	_CrtDumpMemoryLeaks();
#endif
	//https://learn.microsoft.com/en-us/cpp/c-runtime-library/find-memory-leaks-using-the-crt-library?view=msvc-170

	return hr;
}