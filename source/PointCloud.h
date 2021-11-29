#pragma once
#include "BaseObject.h"
#include "Texture.h"
#include "Effect.h"

using namespace Elite;

class PointCloud final : public BaseObject
{
public:
	PointCloud(ID3D11Device* pDevice
		, const std::string& meshName
		, const std::vector<float>& pointCloud
		, const std::vector<uint32_t>& shape
		, const FPoint3& pos = { 0,0,0 }
	);
	~PointCloud();

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const override;
	virtual void Update() override;
	virtual void RenderUI() override;

private:
	ID3D11InputLayout* m_pVertexLayout = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr; 
	ID3DX11EffectVectorVariable* m_pColorEffectVariable = nullptr;

	RGBColor m_PointColor = { 0.f, 1.f, 1.f };
	std::vector<FPoint3> m_RenderPoints; // the 4th value contains the value of the point

	HRESULT Initialize(ID3D11Device* pDevice);
};

