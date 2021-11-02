#include "pch.h"
#include "Mesh.h"
#include "IOFiles.h"
#include "SceneGraph.h"

Mesh::Mesh(ID3D11Device* pDevice, const std::string& meshName, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices, const Elite::FPoint3& pos)
	: m_pEffect{ new Effect(pDevice, L"Resources/Shader.fx")}
	, m_Transform{ Elite::FMatrix4::Identity() }
	, m_MeshName{ meshName }
{
	Initialize(pDevice, vertices, indices);

	// world matrix
	m_Transform[3].xyz = FVector3(pos);

	if (m_MeshName.empty())
		m_MeshName = "Default";
}

Mesh::~Mesh()
{
	m_pIndexBuffer->Release();
	m_pIndexBuffer = nullptr;

	m_pVertexBuffer->Release();
	m_pVertexBuffer = nullptr;

	m_pVertexLayout->Release();
	m_pVertexLayout = nullptr;

	delete m_pEffect;
	m_pEffect = nullptr;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext) const
{
	m_pEffect->UpdateMatrix(m_Transform);

	// Set vertex buffer
	UINT stride = sizeof(Vertex_Input);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the input layour
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// Set the primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render a triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed((uint32_t)m_Indices.size(), 0, 0);
	}
}

void Mesh::Update()
{
}

HRESULT Mesh::Initialize(ID3D11Device* pDevice, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices)
{
	m_Vertices = vertices;
	m_Indices = indices;

	// Create Vertex Layout
	HRESULT result = S_OK;
	static const uint32_t numElements{ 3 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "NORMAL";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "COLOR";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

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
	bd.ByteWidth = sizeof(Vertex_Input) * (uint32_t)vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = vertices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return result;

	// Create index buffer
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * (uint32_t)indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return result;

	return result;
}
