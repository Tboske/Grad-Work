#pragma once
#include "pch.h"
#include <vector>
#include "Texture.h"
#include "Effect.h"

using namespace Elite;

class BaseObject
{
public:
	BaseObject(ID3D11Device* pDevice, const std::string& meshName, const FPoint3& pos, const std::wstring& shader);
	~BaseObject();

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const;
	virtual void Update() = 0;
	virtual void RenderUI();

protected:
	Effect* m_pEffect = nullptr;
private:
	Elite::FMatrix4 m_Transform;
	std::string m_MeshName;
};