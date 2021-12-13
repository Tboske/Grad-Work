#pragma once
#include "BaseObject.h"
#include "Texture.h"
#include "Effect.h"

using namespace Elite;

class PointCloud final : public BaseObject
{
public:
	PointCloud(const std::string& meshName, const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape, const FPoint3& pos = { 0,0,0 });
	PointCloud(const std::string& meshName, const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape, const FMatrix4& transform);
	virtual ~PointCloud();

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const override;
	virtual void RenderUI() override;

	const std::vector<float>& GetPoints() const { return m_PointCloud; }
	const std::vector<uint32_t>& GetShape() const { return m_Shape; }
	float GetRubbishValue() const { return m_RubbishValue; }

	float GetValue(uint32_t t, uint32_t z, uint32_t y, uint32_t x) const
	{
		// return rubbish value in case one of the indices are out of bounds
		if (z >= m_Shape[1] || y >= m_Shape[2] || x >= m_Shape[3])
			return m_RubbishValue;

		uint32_t c = (t * m_Shape[1] * m_Shape[2] * m_Shape[3])
			+ (z * m_Shape[2] * m_Shape[3])
			+ (y * m_Shape[3])
			+ x;

		return m_PointCloud[c];
	}
private:
	struct CubeInfo 
	{
		CubeInfo(FPoint3 p, FPoint4 v, FPoint4 v2) : pos{ p }, values{ v }, values2{ v2 } {}
		FPoint3 pos;
		FPoint4 values;
		FPoint4 values2;
	};

	ID3D11InputLayout* m_pVertexLayout = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr; 
	ID3DX11EffectVectorVariable* m_pColorEffectVariable = nullptr;
	RGBColor m_PointColor = { 0.f, 1.f, 1.f };
	ID3DX11EffectScalarVariable* m_pRubbishEffectVariable = nullptr;
	float m_RubbishValue = 0.f;

	std::vector<CubeInfo> m_RenderPoints;
	std::vector<float> m_PointCloud;
	std::vector<uint32_t> m_Shape;

	HRESULT Initialize();
	void InitPointCloud(const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape);
	void StartMarchingCubes() const;
};

