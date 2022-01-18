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

	char m_ImportFile[128]{"C:\\School\\Grad Work\\Project\\source\\Resources\\Import\\test.var"};
	char m_MeshName[64]{};
};

