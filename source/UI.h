#pragma once
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_dx11.h"

struct SDL_Window;
struct ID3D11DeviceContext;
class UI final
{
public:
	UI(SDL_Window* pWindow, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	~UI();

	void RenderUI(float);

private:
	static const ImGuiWindowFlags m_WindowFlags{
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse 
	};

	char m_ImportFile[128]{"testcube"};
	char m_MeshName[64]{};
	int m_SelectedFileType = 0;
	const char* m_pFileTypes[2]{ ".obj", ".vtk" };
	float m_Pos[3] = { 0,0,0 };

	int m_SelectedAlgorithm = 0;
	const char* m_pAlgorithms[2]{ "Marching Cubes", "Dual Contouring" };


	char m_ExportLocation[128]{};


	int m_SelectedMesh = 0;

// Functions
	void ImportTab();
	void ExportTab();
	void SettingsTab();
	void MeshTab() const;
	void LoadingPopUp() const;
};

