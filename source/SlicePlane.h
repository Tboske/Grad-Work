#pragma once

#include "BaseObject.h"

class SlicePlane final : public BaseObject
{
public:
	SlicePlane(const std::string& meshName);
	virtual ~SlicePlane();

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const override;
	virtual void RenderUI() override;

private:
	bool m_Active = false;
};


