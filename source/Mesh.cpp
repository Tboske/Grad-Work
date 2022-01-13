#include "pch.h"
#include "Mesh.h"
#include "IOFiles.h"
#include "SceneGraph.h"
#include "Renderer.h"
#include <thread>

Mesh::Mesh(const std::string& meshName, const std::vector<Vertex_Input>& vertices, const FPoint3& pos, const RGBColor color)
	: BaseObject(meshName, pos, L"Resources/Shader.fx")
	, m_Color{ color }
	, m_Vertices{ vertices }
{
	Initialize();
}

Mesh::Mesh(const std::string& meshName, const std::vector<Vertex_Input>& vertices, const FMatrix4& transform)
	: BaseObject(meshName, transform, L"Resources/Shader.fx")
	, m_Color{ 0.75f, 0.4f, 0.4f }
	, m_Vertices{ vertices }
{
	Initialize();
}

Mesh::~Mesh()
{
	m_pVertexBuffer->Release();
	m_pVertexBuffer = nullptr;

	m_pVertexLayout->Release();
	m_pVertexLayout = nullptr;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext) const
{
	BaseObject::Render(pDeviceContext);

	// Set vertex buffer
	UINT stride = sizeof(Vertex_Input);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

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
		pDeviceContext->Draw(UINT(m_Vertices.size()), 0);
	}
}

void Mesh::RenderUI()
{
	BaseObject::RenderUI();

	ImGui::PushID((char*)this + 'c');
		if (ImGui::ColorEdit3("Color", &m_Color.r))
			m_pColorEffectVariable->SetFloatVector(&m_Color.r);
	ImGui::PopID();

	ImGui::PushID((char*)this + 'e');
		ImGui::InputTextWithHint("Location", "Enter location to export to", m_ExportLocation, IM_ARRAYSIZE(m_ExportLocation));
		if (ImGui::Button("Export as .obj!", { -1, 25 }))
		{
			std::thread thr(IOFiles::ExportMesh, this, m_ExportLocation);
			thr.detach();
		}
	ImGui::PopID();
}

HRESULT Mesh::Initialize()
{
	// set the variables
	m_pColorEffectVariable = m_pEffect->GetEffect()->GetVariableByName("gColor")->AsVector();
	m_pColorEffectVariable->SetFloatVector(&m_Color.r);

	// Create Vertex Layout
	HRESULT result = S_OK;
	static const uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "NORMAL";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

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
	bd.ByteWidth = sizeof(Vertex_Input) * (uint32_t)m_Vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = m_Vertices.data();
	result = Renderer::GetDevice()->CreateBuffer(&bd, &initData, &m_pVertexBuffer);

	return result;
}


