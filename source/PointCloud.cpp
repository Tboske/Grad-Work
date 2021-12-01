#include "pch.h"
#include "PointCloud.h"
#include "IOFiles.h"
#include "SceneGraph.h"
#include "MarchingCubes.h"
#include "Renderer.h"

PointCloud::PointCloud(const std::string& meshName, const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape, const FPoint3& pos)
	: BaseObject(meshName, pos, L"Resources/PointShader.fx")
	, m_PointCloud{ pointCloud }
	, m_Shape{ shape }
{
	// everything containing the rubbish value is not a piece of the pointcloud
	m_RenderPoints.reserve(pointCloud.size());

	for (uint32_t t = 0; t < 1; ++t)	// for now we just use 1 time frame
	{
		const float rubbishVal = pointCloud[t * shape[1] * shape[2] * shape[3]];
		for (uint32_t z = 0; z < shape[1]; ++z)
		{
			for (uint32_t y = 0; y < shape[2]; ++y)
			{
				for (uint32_t x = 0; x < shape[3]; ++x)
				{
					// this calculates the 1d array position
					const uint32_t p = (t * shape[1] * shape[2] * shape[3]) + (z * shape[2] * shape[3]) + (y * shape[3]) + x;

					// this should be removed eventually
					if (rubbishVal >= pointCloud[p])
						continue;

					m_RenderPoints.emplace_back(float(x), float(y), float(z));
				}
			}
		}
	}
	Initialize();
}


PointCloud::~PointCloud()
{
	m_pVertexBuffer->Release();
	m_pVertexBuffer = nullptr;

	m_pVertexLayout->Release();
	m_pVertexLayout = nullptr;

	m_pColorEffectVariable->Release();
	m_pColorEffectVariable = nullptr;
}

void PointCloud::Render(ID3D11DeviceContext* pDeviceContext) const
{
	BaseObject::Render(pDeviceContext);

	// Set vertex buffer
	const UINT stride = sizeof(FPoint3);
	const UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set the input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// Set the primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Render a triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(UINT(m_RenderPoints.size()), 0);
	}
}

void PointCloud::Update()
{
}

void PointCloud::RenderUI()
{
	BaseObject::RenderUI();

	ImGui::PushID((char*)this + 'c');
		ImGui::Text("Color: ");
		ImGui::SameLine();
		if (ImGui::ColorEdit3(" ", &m_PointColor.r))
			m_pColorEffectVariable->SetFloatVector(&m_PointColor.r);
	ImGui::PopID();
	if (ImGui::Button("GenerateMesh"))
		StartMarchingCubes();
}

HRESULT PointCloud::Initialize()
{
	// set the variables
	m_pColorEffectVariable = m_pEffect->GetEffect()->GetVariableByName("gColor")->AsVector();
	m_pColorEffectVariable->SetFloatVector(&m_PointColor.r);

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
	bd.ByteWidth = sizeof(FPoint3) * (uint32_t)m_RenderPoints.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = m_RenderPoints.data();
	result = Renderer::GetDevice()->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return result;

	return result;
}

void PointCloud::StartMarchingCubes() const
{
	MarchingCubes* pMarchingCubes = new MarchingCubes(this);
	if (!pMarchingCubes->GenerateMesh())
		std::cout << "Marching Cubes Generation failed";

}
