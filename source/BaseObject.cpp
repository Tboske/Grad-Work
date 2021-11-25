#include "pch.h"
#include "BaseObject.h"

BaseObject::BaseObject(ID3D11Device* pDevice, const std::string& meshName, const FPoint3& pos, const std::wstring& shader)
	: m_pEffect{ new Effect(pDevice, shader) }
	, m_Transform{ Elite::FMatrix4::Identity() }
	, m_MeshName{ meshName }
{
	// world matrix
	m_Transform[3].xyz = FVector3(pos);

	if (m_MeshName.empty())
		m_MeshName = "Default";
}

BaseObject::~BaseObject()
{
	delete m_pEffect;
	m_pEffect = nullptr;
}

void BaseObject::Render(ID3D11DeviceContext* pDeviceContext) const
{
	m_pEffect->UpdateMatrix(m_Transform);
}

void BaseObject::RenderUI()
{
	if (ImGui::TreeNode(this, m_MeshName.c_str()))
	{
		// Position of the object
		ImGui::PushID((char*)this + 'p');
			ImGui::Text("Position: ");
			ImGui::SameLine();
			ImGui::DragFloat3("", &m_Transform[3].x, 0.05f, -FLT_MAX, +FLT_MAX);
		ImGui::PopID();

		// scale of the object
		ImGui::PushID((char*)this + 's');
			float scale[4] = { m_Transform[0].x, m_Transform[1].y, m_Transform[2].z, 1.f };
			ImGui::Text("Scale: ");
			ImGui::SameLine();
			ImGui::DragFloat4("", scale, 0.01f, -FLT_MAX, +FLT_MAX);

			m_Transform[0].x = scale[0] * scale[3];
			m_Transform[1].y = scale[1] * scale[3];
			m_Transform[2].z = scale[2] * scale[3];
		ImGui::PopID();
		ImGui::TreePop();
	}
}
