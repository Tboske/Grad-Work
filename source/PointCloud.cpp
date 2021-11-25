#include "pch.h"
#include "PointCloud.h"
#include "IOFiles.h"
#include "SceneGraph.h"

PointCloud::PointCloud(ID3D11Device* pDevice, const std::string& meshName, const std::vector<std::vector<std::vector<float>>>& pointCloud, const FPoint3& pos)
	: BaseObject(pDevice, meshName, pos, L"Resources/PointShader.fx")
	, m_PointCloud{ pointCloud }
{
	// everything containing the rubbish value is not a piece of the pointcloud
	float rubbishVal{ pointCloud[0][0][0] };
	m_RenderPoints.reserve(pointCloud.size());

	const uint32_t zSize = pointCloud.size();
	const uint32_t ySize = pointCloud[0].size();
	const uint32_t xSize = pointCloud[0][0].size();

	for (uint32_t t = 0; t < 1; ++t)	// for now we just use 1 time frame
	{
		for (uint32_t z = 0; z < zSize; ++z)
		{
			for (uint32_t y = 0; y < ySize; ++y)
			{
				for (uint32_t x = 0; x < xSize; ++x)
				{
					// this calculates the 1d array position
					float val = m_PointCloud[z][y][x];

					// if the coordinate has a lower value then the rubbish value, dont add it to the pointcloud
					if (val <= rubbishVal)
						continue;

					m_RenderPoints.emplace_back(float(x), float(y), float(z));
				}
			}
		}
	}

	Initialize(pDevice);
}



PointCloud::~PointCloud()
{
	m_pVertexBuffer->Release();
	m_pVertexBuffer = nullptr;

	m_pVertexLayout->Release();
	m_pVertexLayout = nullptr;
}

void PointCloud::Render(ID3D11DeviceContext* pDeviceContext) const
{
	BaseObject::Render(pDeviceContext);

	// Set vertex buffer
	const UINT stride = sizeof(FPoint3);
	const UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set index buffer
	//pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the input layour
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// Set the primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Render a triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(m_VertexCount, 0);
		//pDeviceContext->DrawIndexed((uint32_t)m_Indices.size(), 0, 0);
	}
}

void PointCloud::Update()
{
}

HRESULT PointCloud::Initialize(ID3D11Device* pDevice)
{
	m_VertexCount = UINT(m_RenderPoints.size());

	// Create Vertex Layout
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
	result = pDevice->CreateInputLayout(vertexDesc, numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pVertexLayout);
	if (FAILED(result))
		return result;

	// Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(FPoint3) * (uint32_t)m_RenderPoints.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = m_RenderPoints.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return result;
	
	return result;
}
