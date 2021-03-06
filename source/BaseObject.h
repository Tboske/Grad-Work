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
	BaseObject(const std::string& meshName, const FMatrix4& transform, const std::wstring& shader);
	virtual ~BaseObject();

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const;
	virtual void RenderUI();

	[[nodiscard]] const std::string& GetMeshName() const { return m_MeshName; }
	[[nodiscard]] FPoint3 GetPosition() const { return FPoint3(m_Transform[3].xyz); }
	[[nodiscard]] const FMatrix4& GetTransform() const { return m_Transform; }

protected:
	Effect* m_pEffect = nullptr;
	FVector3 m_Rotation {0.f,0.f,0.f };
private:
	FMatrix4 m_Transform;
	std::string m_MeshName;

};