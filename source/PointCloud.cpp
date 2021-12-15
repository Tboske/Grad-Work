#include "pch.h"
#include "PointCloud.h"
#include "MarchingCubes.h"
#include "Renderer.h"
#include <thread>
#include "MarchTable.h"

PointCloud::PointCloud(const std::string& meshName, const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape, const FPoint3& pos)
	: BaseObject(meshName, pos, L"Resources/MarchingCubesShader.fx")	// if you change this to the marching cubes shader, dont forget to change topology in the render function
	, m_PointCloud{ pointCloud }		// MarchingCubesShader
	, m_Shape{ shape }
{
	InitPointCloud(pointCloud, shape);
	Initialize();

	//StartMarchingCubes();
}

PointCloud::PointCloud(const std::string& meshName, const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape, const FMatrix4& transform)
	: BaseObject(meshName, transform, L"Resources/PointShader.fx")
	, m_PointCloud{ pointCloud }
	, m_Shape{ shape }
{
	InitPointCloud(pointCloud, shape);
	Initialize();
}

PointCloud::~PointCloud()
{
	delete m_pTriangulationLUT;
	m_pTriangulationLUT = nullptr;

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
	const UINT stride = sizeof(CubeInfo);
	const UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set the input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// Set the primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render a triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(UINT(m_RenderPoints.size()), 0);
	}
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
	HRESULT result = S_OK;
	// set the variables
	m_pColorEffectVariable = m_pEffect->GetEffect()->GetVariableByName("gColor")->AsVector();
	m_pColorEffectVariable->SetFloatVector(&m_PointColor.r);

	// create lookup table texture
	D3D11_TEXTURE2D_DESC lutTriangulationDesc;
		lutTriangulationDesc.Width = 256;
		lutTriangulationDesc.Height = 16;
		lutTriangulationDesc.Format = DXGI_FORMAT_R32_SINT;
		lutTriangulationDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lutTriangulationDesc.SampleDesc.Count = 1;
		lutTriangulationDesc.SampleDesc.Quality = 0;
		lutTriangulationDesc.Usage = D3D11_USAGE_DEFAULT;
		lutTriangulationDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		lutTriangulationDesc.ArraySize = 1;
		lutTriangulationDesc.MipLevels = 1;
		lutTriangulationDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA lutTriangulationData;
		lutTriangulationData.pSysMem = &Table::Triangulation[0][0];
		lutTriangulationData.SysMemPitch = 16 * sizeof(Table::Triangulation[0][0]);
		lutTriangulationData.SysMemSlicePitch = 16 * 256 * sizeof(Table::Triangulation[0][0]);
	
	m_pTriangulationLUT = new Texture(Renderer::GetDevice(), lutTriangulationDesc, lutTriangulationData);

	m_pEffect->GetEffect()->GetVariableByName("gTriangulationLUT")->AsShaderResource()->SetResource(m_pTriangulationLUT->GetSRV());



	// Create Vertex Layout
	static const uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "CUBEID";
	vertexDesc[1].Format = DXGI_FORMAT_R32_UINT;
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
	bd.ByteWidth = sizeof(CubeInfo) * (uint32_t)m_RenderPoints.size();
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

void PointCloud::InitPointCloud(const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape)
{
	// everything containing the rubbish value is not a piece of the pointcloud
	m_RenderPoints.reserve(pointCloud.size());

	for (uint32_t t = 0; t < 1; ++t)	// for now we just use 1 time frame
	{
		m_RubbishValue = pointCloud[t * shape[1] * shape[2] * shape[3]];
		for (uint32_t z = 0; z < shape[1] - 1; ++z)
		{
			for (uint32_t y = 0; y < shape[2] - 1; ++y)
			{
				for (uint32_t x = 0; x < shape[3] - 1; ++x)
				{
					std::bitset<8> corners;
						corners[0] = GetValue(t, z    , y    , x    ) > m_RubbishValue;
						corners[1] = GetValue(t, z    , y    , x + 1) > m_RubbishValue;
						corners[2] = GetValue(t, z    , y + 1, x + 1) > m_RubbishValue;
						corners[3] = GetValue(t, z    , y + 1, x    ) > m_RubbishValue;
						corners[4] = GetValue(t, z + 1, y    , x    ) > m_RubbishValue; 
						corners[5] = GetValue(t, z + 1, y    , x + 1) > m_RubbishValue;
						corners[6] = GetValue(t, z + 1, y + 1, x + 1) > m_RubbishValue; 
						corners[7] = GetValue(t, z + 1, y + 1, x    ) > m_RubbishValue;

					CubeInfo ci;
						ci.pos = FPoint3{ float(x), float(y), float(z) };
						ci.cubeID = UINT(corners.to_ulong());

					if (!ci.ContainsActiveCorner())
						continue;

					m_RenderPoints.push_back(ci);
				}
			}
		}
	}
}

void PointCloud::StartMarchingCubes() const
{
	MarchingCubes* pMarchingCubes = new MarchingCubes(this);
	std::thread thr(&MarchingCubes::GenerateMesh, pMarchingCubes);
	thr.detach();
}
