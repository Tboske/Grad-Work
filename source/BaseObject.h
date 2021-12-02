#pragma once
#include "pch.h"
#include <vector>
#include "Texture.h"
#include "Effect.h"

using namespace Elite;

class BaseObject
{
public:
	BaseObject(const std::string& meshName, const FPoint3& pos, const std::wstring& shader);
	virtual ~BaseObject();

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const;
	virtual void RenderUI();

	const std::string& GetMeshName() const { return m_MeshName; }
	Elite::FPoint3 GetPosition() const { return Elite::FPoint3(m_Transform[3].xyz); }

protected:
	Effect* m_pEffect = nullptr;
	Elite::FMatrix4 m_Transform;
private:
	std::string m_MeshName;
};