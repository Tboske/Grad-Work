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
	[[nodiscard]] FVector3 GetOrientation() const
	{
		return FVector3();
	}

private:
	ID3D11InputLayout* m_pVertexLayout = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3DX11EffectVectorVariable* m_pColorEffectVariable = nullptr;
	FPoint4 m_PlaneColor = {1.f, 0.f, 0.f, 0.75f};
	inline static const std::array<FPoint3, 6> m_PlanePoints
	{
		FPoint3{-100, -100, 0},	 // 1 	  2 +-----------+ 3,4
		FPoint3{-100,  100, 0},	 // 2 		|         / |
		FPoint3{ 100,  100, 0},	 // 3 		|       /   |
									  	 //	    	|     /     |
		FPoint3{ 100,  100, 0},	 // 4 		|   /       |
		FPoint3{ 100, -100, 0},	 // 5 		| /         |
		FPoint3{-100, -100, 0}	 // 6 	1,6 +-----------+ 5
	};

	bool m_IsActive = false;

	HRESULT Initialize();
};


