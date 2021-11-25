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
		, const std::vector<std::vector<std::vector<float>>>& pointCloud
		, const FPoint3& pos = { 0,0,0 }
	);
	~PointCloud();

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const override;
	virtual void Update() override;

	const std::vector<FPoint3>& GetPoints() const { return m_RenderPoints; }

private:
	ID3D11InputLayout* m_pVertexLayout = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	UINT m_VertexCount = 0;

	const std::vector<std::vector<std::vector<float>>> m_PointCloud;
	std::vector<FPoint3> m_RenderPoints;

	HRESULT Initialize(ID3D11Device* pDevice);
};

