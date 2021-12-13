#include "pch.h"
#include "BaseObject.h"

BaseObject::BaseObject(const std::string& meshName, const FPoint3& pos, const std::wstring& shader)
	: m_pEffect{ new Effect(shader) }
	, m_Transform{ Elite::FMatrix4::Identity() }
	, m_MeshName{ meshName }
{
	// world matrix
	m_Transform[3].xyz = FVector3(pos);

	if (m_MeshName.empty())
		m_MeshName = "Default";
}

BaseObject::BaseObject(const std::string& meshName, const FMatrix4& transform, const std::wstring& shader)
	: m_pEffect{ new Effect(shader) }
	, m_Transform{ transform }
	, m_MeshName{ meshName }
{
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
		// Position of the object
		ImGui::PushID(this + 'p');
			ImGui::Text("Position: ");
			ImGui::SameLine();
			ImGui::DragFloat3(" ", &m_Transform[3].x, 0.05f, -FLT_MAX, +FLT_MAX);
		ImGui::PopID();

		// rotation of the object
		ImGui::PushID(this + 'r');
			ImGui::Text("Rotation: ");
			ImGui::SameLine();
			static float l[3] = {0.f,0.f,0.f};
			ImGui::DragFloat3(" ", l);
		ImGui::PopID();

		// scale of the object
		ImGui::PushID(this + 's');
			float scale[3] = { m_Transform[0].x, m_Transform[1].y, m_Transform[2].z };
			ImGui::Text("Scale: ");
			ImGui::SameLine();
			if (ImGui::DragFloat3(" ", scale, 0.01f, -FLT_MAX, +FLT_MAX))
			{
				m_Transform[0].x = scale[0];
				m_Transform[1].y = scale[1];
				m_Transform[2].z = scale[2];
			}
		ImGui::PopID();
}
