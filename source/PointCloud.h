#pragma once
#include "BaseObject.h"
#include "Texture.h"
#include "Effect.h"

using namespace Elite;

class PointCloud final : public BaseObject
{
public:
	PointCloud(const std::string& meshName
		, const std::vector<float>& pointCloud
		, const std::vector<uint32_t>& shape
		, const FPoint3& pos = { 0,0,0 }
	);
	virtual ~PointCloud();

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const override;
	virtual void RenderUI() override;

	const std::vector<float>& GetPoints() const { return m_PointCloud; }
	const std::vector<uint32_t>& GetShape() const { return m_Shape; }

private:
	ID3D11InputLayout* m_pVertexLayout = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr; 
	ID3DX11EffectVectorVariable* m_pColorEffectVariable = nullptr;
	RGBColor m_PointColor = { 0.f, 1.f, 1.f };

	std::vector<FPoint3> m_RenderPoints;
	const std::vector<float> m_PointCloud;
	const std::vector<uint32_t> m_Shape;

	HRESULT Initialize();
	void StartMarchingCubes() const;
};

