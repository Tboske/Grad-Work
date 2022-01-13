#include "pch.h"
#include "SlicePlane.h"

#include "Renderer.h"

SlicePlane::SlicePlane(const std::string& meshName)
	: BaseObject(meshName, {0,0,0}, L"Resources/SlicePlaneShader.fx")
{
	Initialize();
}

SlicePlane::~SlicePlane()
{
	m_pVertexBuffer->Release();
	m_pVertexBuffer = nullptr;

	m_pVertexLayout->Release();
	m_pVertexLayout = nullptr;
}

void SlicePlane::Render(ID3D11DeviceContext* pDeviceContext) const
{
	if (!m_IsActive)
		return;

	BaseObject::Render(pDeviceContext);

	// Set vertex buffer
	const UINT stride = sizeof(FPoint3);
	const UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set the input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// Set the primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render a mesh	
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffect->GetTechnique()->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);

		pDeviceContext->Draw(UINT(m_PlanePoints.size()), 0);
	}
}

void SlicePlane::RenderUI()
{
	ImGui::Checkbox("Enable SlicePlane", &m_IsActive);

	if (!m_IsActive)
		return;

	if (ImGui::TreeNode(this, "Slicing Plane"))
	{
		BaseObject::RenderUI();

		ImGui::TreePop();
	}
}

FVector3 SlicePlane::GetOrientation() const
{




	return FVector3();
}

HRESULT SlicePlane::Initialize()
{
	HRESULT result = S_OK;

	static const uint32_t numElements{ 1 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	m_pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = Renderer::GetDevice()->CreateInputLayout(vertexDesc
		, numElements
		, passDesc.pIAInputSignature
		, passDesc.IAInputSignatureSize
		, &m_pVertexLayout);
	if (FAILED(result))
		return result;

	// Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(FPoint3) * (uint32_t)m_PlanePoints.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = m_PlanePoints.data();
	result = Renderer::GetDevice()->CreateBuffer(&bd, &initData, &m_pVertexBuffer);

	return result;
}
