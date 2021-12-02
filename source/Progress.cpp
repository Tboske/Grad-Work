#include "pch.h"
#include "Progress.h"
#include "imgui.h"
#include <string>

void Progress::LoadingPopUpImpl() const
{
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize({ 200, 95 });

	if (m_Active)
		ImGui::OpenPopup("LoadingPopUp");

	if (ImGui::BeginPopupModal("LoadingPopUp", &GetInstance()->m_Active, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(m_Description.c_str());
		ImGui::Separator();
		ImGui::ProgressBar(m_Value);

		auto elapsed = duration_cast<seconds>(high_resolution_clock::now() - m_StartTime).count();
		ImGui::Text(std::string{ "Elapsed time: " + std::to_string(elapsed) + "s" }.c_str());

		ImGui::EndPopup();
	}
}

void Progress::StartImpl(const std::string& s, float maxVal) {
	m_Active = true;
	m_StartTime = high_resolution_clock::now();
	m_Value = 0.f;
	m_MaxValue = maxVal;
	m_Description = s;
}

void Progress::ResetProgressImpl(float maxVal, const std::string& s)
{
	m_Value = 0.f;
	m_Description = s;
	m_MaxValue = maxVal;
}

void Progress::SetValueImpl(float value)
{
	m_Value = value / m_MaxValue;
}
