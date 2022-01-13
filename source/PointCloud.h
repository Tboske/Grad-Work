#pragma once
#include "BaseObject.h"
#include "Texture.h"
#include "Effect.h"
#include <bitset>
#include "SlicePlane.h"

using namespace Elite;

class PointCloud final : public BaseObject
{
public:
	PointCloud(const std::string& meshName, const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape, const FPoint3& pos = { 0,0,0 });
	PointCloud(const std::string& meshName, const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape, const FMatrix4& transform);
	virtual ~PointCloud() override;

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const override;
	virtual void RenderUI() override;

	[[nodiscard]] const std::vector<float>& GetPoints() const { return m_PointCloud; }
	[[nodiscard]] const std::vector<uint32_t>& GetShape() const { return m_Shape; }
	[[nodiscard]] float GetRubbishValue() const { return m_RubbishValue; }

private:
	struct CubeInfo 
	{
		FPoint3 pos = {0,0,0};
		UINT cubeID = 0;

		// if all corners are filled or empty, this cube doesnt matter, and doesnt need any computation
		bool ContainsActiveCorner() const
		{
			// excludes the cubes that have no or all active corners
			return cubeID != 0 && cubeID != 255;
		}
	};

	Effect* m_pPointCloudEffect = nullptr;
	bool m_IsPreviewVisible = false;

	ID3D11InputLayout* m_pVertexLayout = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr; 
	ID3DX11EffectVectorVariable* m_pColorEffectVariable = nullptr;
	ID3DX11EffectVectorVariable* m_pColorEffectVariable2 = nullptr;
	RGBColor m_PointColor = { 0.f, 1.f, 1.f };
	float m_RubbishValue = 0.f;
	Texture* m_pTriangulationLUT;

	std::vector<CubeInfo> m_RenderPoints;
	std::vector<float> m_PointCloud;
	std::vector<uint32_t> m_Shape;

	SlicePlane* m_pSlicePlane = nullptr;

	char m_Filename[50];
	HRESULT Initialize();
	void InitPointCloud(const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape);

	void StartExport() const;
	[[nodiscard]] float GetValue(uint32_t t, uint32_t z, uint32_t y, uint32_t x) const;
	[[nodiscard]] unsigned char GetCubeFillID(uint32_t z, uint32_t y, uint32_t x) const;
	[[nodiscard]] uint32_t GetPos(uint32_t t, uint32_t z, uint32_t y, uint32_t x) const
	{
		return (t * m_Shape[1] * m_Shape[2] * m_Shape[3])
		+ (z * m_Shape[2] * m_Shape[3])
		+ (y * m_Shape[3])
		+ x;
	}
};

