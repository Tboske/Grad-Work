#include "pch.h"
#include "UI.h"
#include "IOFiles.h"
#include <thread>
#include "SceneGraph.h"
#include "Progress.h"
#include "Renderer.h"


UI::UI(SDL_Window* pWindow, ID3D11DeviceContext* pDeviceContext)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForD3D(pWindow);
	ImGui_ImplDX11_Init(Renderer::GetDevice(), pDeviceContext);

	// Style
	ImGuiStyle& style = ImGui::GetStyle();
	style.TabRounding = 0.f;
	style.WindowBorderSize = 0.f;
}

UI::~UI()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void UI::RenderUI(float height)
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	ImGui::Begin("test", NULL, m_WindowFlags);
	{
		Progress::LoadingPopUp();

		ImGui::SetWindowPos({ 0, 0 });
		ImGui::SetWindowSize({ 300, height });

		// import related
		ImGui::TextColored({0,1,0,1}, "Import");
		ImGui::TextColored({1,1,0,1},"available filetypes: .var .vtk");
		ImGui::TextWrapped("for .vtk files, make sure it includes the .pts and .surf in the same directory!");
		ImGui::TextColored({ 1,0,0,1 }, "Copy and paste the file path!");
		ImGui::InputTextWithHint("Filename", "VoxelFile", m_ImportFile, IM_ARRAYSIZE(m_ImportFile));
		ImGui::InputTextWithHint("MeshName", "Optional", m_MeshName, IM_ARRAYSIZE(m_MeshName));
		if (ImGui::Button("Import!", { -1, 25 }))
		{
			std::thread thr(IOFiles::ImportFile, m_ImportFile, m_MeshName, FPoint3{ 0,0,0 });
			thr.detach();
		}

		// mesh settings
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::TextColored({ 0,1,0,1 }, "MeshList");
		const auto& objects = SceneGraph::GetInstance()->GetObjects();
		for (BaseObject* object : objects)
		{
			if (ImGui::TreeNode(object, object->GetMeshName().c_str()))
			{
				object->RenderUI();
				ImGui::TreePop();
			}
		}

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
