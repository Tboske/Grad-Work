#include "pch.h"
#include "SlicePlane.h"

SlicePlane::SlicePlane(const std::string& meshName)
	: BaseObject(meshName, {0,0,0}, L"Resources/SlicePlaneShader.fx")
{
}

SlicePlane::~SlicePlane()
{
}

void SlicePlane::Render(ID3D11DeviceContext* pDeviceContext) const
{
	BaseObject::Render(pDeviceContext);
}

void SlicePlane::RenderUI()
{
	BaseObject::RenderUI();
}
