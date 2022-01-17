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
		ImGui::DragFloat3("Position", m_Transform[3].data, 0.05f, -FLT_MAX, +FLT_MAX);
	ImGui::PopID();

	// rotation of the object
	ImGui::PushID(this + 'r');
		const FVector3 lastRotation = m_Rotation;
		if (ImGui::DragFloat3("Rotation", m_Rotation.data, 0.005f))
		{
			const FVector3 newRotation = m_Rotation - lastRotation;
			m_Transform *= FMatrix4(MakeRotationZYX(newRotation.z, newRotation.y, newRotation.x));
		}
	ImGui::PopID();
}

