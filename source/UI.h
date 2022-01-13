#pragma once

class UI final
{
public:
	UI(SDL_Window* pWindow, ID3D11DeviceContext* pDeviceContext);
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

	char m_ImportFile[128]{"test"};
	char m_MeshName[64]{};
	int m_SelectedFileType = 3;
	const char* m_pFileTypes[4]{ ".obj", ".vtk", ".vvtk", ".var"};
	float m_Pos[3] = { 0,0,0 };

	int m_SelectedAlgorithm = 0;
	const char* m_pAlgorithms[2]{ "Marching Cubes", "Dual Contouring" };

// Functions
	void ImportTab();
};

