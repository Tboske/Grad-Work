#include "pch.h"
//#undef main

//Standard includes
#include <iostream>

//Project includes
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_dx11.h"

#include "ETimer.h"
#include "Renderer.h"
#include "SceneGraph.h"
#include <vector>
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "IOFiles.h"

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 1280;
	const uint32_t height = 800;
	SDL_Window* pWindow = SDL_CreateWindow(
		"Grad Work - De Meutter Jeff",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	auto pTimer{ std::make_unique<Elite::Timer>() };

	// initialize input output files
	Renderer::Initialize(pWindow);
	IOFiles::Initialize();

	// Initialize SceneGraph
	SceneGraph* scene = SceneGraph::GetInstance();
	scene->SetCamera(new Camera(Elite::FPoint3(0.f, 10.f, 100.f), Elite::FVector3(0.f, 0.f, 1.f), (float)width / (float)height, 90.f, 0.1f, 10000.f)); // in degrees

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;

	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);
				
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				break;
			}
		}

		if (!ImGui::IsAnyItemActive())
			scene->GetCamera()->CheckInputs(pTimer->GetElapsed());

		//--------- Render ---------
		Renderer::Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}
	}
	pTimer->Stop();

	// cleanup scenegraph
	delete SceneGraph::GetInstance();
	Renderer::Cleanup();

	//Shutdown "framework"
	ShutDown(pWindow);
	return 0;
}