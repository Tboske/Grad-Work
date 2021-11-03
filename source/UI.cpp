#include "pch.h"
#include "UI.h"
#include "IOFiles.h"
#include <thread>
#include "SceneGraph.h"

UI::UI(SDL_Window* pWindow, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice{ pDevice }
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForD3D(pWindow);
	ImGui_ImplDX11_Init(pDevice, pDeviceContext);

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

	ImGui::ShowDemoWindow();

	ImGui::Begin("test", NULL, m_WindowFlags);
	{
		ImGui::SetWindowPos({ 0, 0 });
		ImGui::SetWindowSize({ 400, height });
		if (ImGui::BeginTabBar("Tab"))
		{
			ImportTab();
			SettingsTab();
			ExportTab();
			MeshTab();

			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UI::ImportTab()
{
	if (ImGui::BeginTabItem("Import"))
	{
		ImGui::Text("For now paste the files into Resources/Input");
		ImGui::TextColored({ 1,0,0,1 }, "Enter the Filename");
		ImGui::Combo("FileType", &m_SelectedFileType, m_pFileTypes, IM_ARRAYSIZE(m_pFileTypes));
		ImGui::InputTextWithHint("Filename", "VoxelFile", m_ImportFile, IM_ARRAYSIZE(m_ImportFile));
		ImGui::InputTextWithHint("MeshName", "Default", m_MeshName, IM_ARRAYSIZE(m_MeshName));
		if (ImGui::Button("Import!", { ImGui::GetContentRegionAvailWidth(), 25 }))
		{
			IOFiles::ImportFile(m_pDevice, std::string() + m_ImportFile + m_pFileTypes[m_SelectedFileType], m_MeshName);
			ImGui::OpenPopup("LoadingPopUp");
		}

		LoadingPopUp();

		ImGui::EndTabItem();
	}
}

void UI::ExportTab()
{
	if (ImGui::BeginTabItem("Export"))
	{
		ImGui::InputTextWithHint("Location", "Enter location to export to", m_ExportLocation, IM_ARRAYSIZE(m_ExportLocation));
		ImGui::TextColored({ 1,0,0,1 }, "Include the extension!");
		if (ImGui::Button("Export!", { -1, 25 }))
			IOFiles::ExportMesh(SceneGraph::GetInstance()->GetMeshes()[0], m_ExportLocation);

		ImGui::EndTabItem();
	}
}

void UI::SettingsTab()
{
	if (ImGui::BeginTabItem("Settings"))
	{
		ImGui::Combo("Algorithm", &m_SelectedAlgorithm, m_pAlgorithms, IM_ARRAYSIZE(m_pAlgorithms));
		//if (ImGui::Button("Generate", { -1, 25 }));
		//	call the generate function here;
		ImGui::EndTabItem();
	}
}

void UI::MeshTab() const
{
	if (ImGui::BeginTabItem("MeshTab"))
	{
		auto& meshList = SceneGraph::GetInstance()->GetMeshes();
		for (int i = 0; i < meshList.size(); i++)
		{
			FMatrix4& transform = meshList[i]->GetTransform();
			ImGui::PushID(i * 2);
				ImGui::Text(meshList[i]->GetMeshName().c_str());
				ImGui::Text("Position: ");
				ImGui::SameLine();
				ImGui::DragFloat3("", &transform[3].x, 0.05f, -FLT_MAX, +FLT_MAX);
			ImGui::PopID();
		}

		ImGui::EndTabItem();
	}
}

void UI::LoadingPopUp() const
{
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("LoadingPopUp", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("The mesh is loading");
		ImGui::Separator();
		float progress = IOFiles::GetProgess();
		ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
		if (progress >= 1.f)
			ImGui::CloseCurrentPopup();
		if (ImGui::Button("ok"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}
