#pragma once

#include "BaseObject.h"
#include <array>

class SlicePlane final : public BaseObject
{
public:
	SlicePlane(const std::string& meshName);
	virtual ~SlicePlane();

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const override;
	virtual void RenderUI() override;

	[[nodiscard]] bool* ActivePointer() { return &m_IsActive; }
	[[nodiscard]] FVector3 GetOrientation() const;

private:
	ID3D11InputLayout* m_pVertexLayout = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	inline static const std::array<FPoint3, 6> m_PlanePoints
	{
		FPoint3{-100, -100, 0},	 //	  2 +-----------+ 3,4
		FPoint3{-100,  100, 0},	 //		|         / |
		FPoint3{ 100,  100, 0},	 //		|       /   |
										 //		|     /     |
		FPoint3{ 100,  100, 0},	 //		|   /       |
		FPoint3{ 100, -100, 0},	 //		| /         |
		FPoint3{-100, -100, 0}	 //	1,6 +-----------+ 5
	};

	bool m_IsActive = false;

	HRESULT Initialize();
};


