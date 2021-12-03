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

		// scale of the object
		ImGui::PushID(this + 's');
			float scale[4] = { m_Transform[0].x, m_Transform[1].y, m_Transform[2].z, 1.f };
			ImGui::Text("Scale: ");
			ImGui::SameLine();
			ImGui::DragFloat4(" ", scale, 0.01f, -FLT_MAX, +FLT_MAX);

			m_Transform[0].x = scale[0] * scale[3];
			m_Transform[1].y = scale[1] * scale[3];
			m_Transform[2].z = scale[2] * scale[3];
		ImGui::PopID();

		// rotation of the object

	// rotation of the object
		ImGui::PushID(this + 'r');
			ImGui::Text("Rotation: ");
			ImGui::SameLine();
			if (ImGui::DragFloat3(" ", &m_Rotation.x, 0.001, -FLT_MAX, +FLT_MAX));
			{
				FMatrix4 result = FMatrix4::Identity();

				const float cosz = cosf(-m_Rotation.z);
				const float sinz = sinf(-m_Rotation.z);
				const float cosy = cosf(-m_Rotation.y);
				const float siny = sinf(-m_Rotation.y);
				const float cosx = cosf(-m_Rotation.x);
				const float sinx = sinf(-m_Rotation.x);

				result[0][0] = cosz * cosy;
				result[1][0] = (cosz * siny * sinx) - (sinz * cosx);
				result[2][0] = (cosz * siny * cosx) + (sinz * sinx);

				result[0][1] = sinz * cosy;
				result[1][1] = (sinz * siny * sinx) + (cosz * cosx);
				result[2][1] = (sinz * siny * cosx) - (cosz * sinx);

				result[0][2] = -siny;
				result[1][2] = cosy * sinx;
				result[2][2] = cosy * cosx;

				m_Transform = result;
			}
		ImGui::PopID();
}
