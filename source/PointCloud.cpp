#include "pch.h"
#include "PointCloud.h"
#include "Renderer.h"
#include <thread>
#include "MarchTable.h"
#include "Progress.h"
#include "IOFiles.h"
#include <numeric>

PointCloud::PointCloud(const std::string& meshName, const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape, const FPoint3& pos)
	: BaseObject(meshName, pos, L"Resources/MarchingCubesShader.fx")
	, m_PointCloud{ pointCloud }		// MarchingCubesShader
	, m_Shape{ shape }
	, m_pPointCloudEffect{ new Effect(L"Resources/PointShader.fx") }
	, m_pSlicePlane{ new SlicePlane("Slicepane") }
{
	m_pSlicePlane->SetParent(this);
	InitPointCloud(pointCloud, shape);
	Initialize();
}

PointCloud::PointCloud(const std::string& meshName, const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape, const FMatrix4& transform)
	: BaseObject(meshName, transform, L"Resources/MarchingCubesShader.fx")
	, m_PointCloud{ pointCloud }
	, m_Shape{ shape }
	, m_pPointCloudEffect{ new Effect(L"Resources/PointShader.fx") }
	, m_pSlicePlane{ new SlicePlane("Slicepane")}
{
	m_pSlicePlane->SetParent(this);
	InitPointCloud(pointCloud, shape);
	Initialize();
}

PointCloud::~PointCloud()
{
	delete m_pPointCloudEffect;
	m_pPointCloudEffect = nullptr;

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
	m_pPointCloudEffect->UpdateMatrix(GetTransform());

	// Set vertex buffer
	const UINT stride = sizeof(CubeInfo);
	const UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set the input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// Set the primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Render a mesh	
	D3DX11_TECHNIQUE_DESC techDesc;
	if (m_IsPreviewVisible)
		m_pEffect->GetTechnique()->GetDesc(&techDesc);
	else
		m_pPointCloudEffect->GetTechnique()->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		if (m_IsPreviewVisible)
			m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		else 
			m_pPointCloudEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);

		pDeviceContext->Draw(UINT(m_RenderPoints.size()), 0);
	}
	m_pSlicePlane->Render(pDeviceContext);
}

void PointCloud::RenderUI()
{
	BaseObject::RenderUI();

	ImGui::PushID(this + 'c');
		if (ImGui::ColorEdit3("Color", &m_PointColor.r))
		{
			m_pColorEffectVariable->SetFloatVector(&m_PointColor.r);
			m_pColorEffectVariable2->SetFloatVector(&m_PointColor.r);
		}
	ImGui::PopID();
	ImGui::Spacing();

		ImGui::Checkbox("Render Preview", &m_IsPreviewVisible);

	ImGui::Spacing();

	m_pSlicePlane->RenderUI();

	ImGui::Spacing();
	ImGui::Spacing();

		ImGui::InputTextWithHint("FileName", "DefaultExport", m_Filename, IM_ARRAYSIZE(m_Filename));
		if (ImGui::Button("Export", { -1, 25 }))
	{
		std::thread thr(&PointCloud::StartExport, this);
		thr.detach();
	}

}

HRESULT PointCloud::Initialize()
{
	HRESULT result = S_OK;
	// set the variables
	m_pColorEffectVariable = m_pEffect->GetEffect()->GetVariableByName("gColor")->AsVector();
	m_pColorEffectVariable->SetFloatVector(&m_PointColor.r);
	m_pColorEffectVariable2 = m_pPointCloudEffect->GetEffect()->GetVariableByName("gColor")->AsVector();
	m_pColorEffectVariable2->SetFloatVector(&m_PointColor.r);

	// create lookup table texture
	D3D11_TEXTURE2D_DESC lutTriangulationDesc;
		lutTriangulationDesc.Width = 16;
		lutTriangulationDesc.Height = 256;
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

	return CreateVertexBuffer();
}

void PointCloud::InitPointCloud(const std::vector<float>& pointCloud, const std::vector<uint32_t>& shape)
{
	// everything containing the rubbish value is not a piece of the pointcloud
	m_RenderPoints.reserve(pointCloud.size());
	
	Progress::Start("Initializing"
		, "Generating Cube info"
		, float(GetPos(shape[0], shape[1], shape[2], shape[3]))
	);

	for (uint32_t t = 0; t < 1; ++t)	// for now we just use 1 time frame
	{
		m_RubbishValue = pointCloud[t * shape[1] * shape[2] * shape[3]];
		for (uint32_t z = 0; z < shape[1] - 1; ++z)
		{
			for (uint32_t y = 0; y < shape[2] - 1; ++y)
			{
				for (uint32_t x = 0; x < shape[3] - 1; ++x)
				{
					CubeInfo ci;
						ci.pos = FPoint3{ float(x), float(y), float(z) };
						ci.cubeID = UINT(GetCubeFillID(z, y, x));

					if (!ci.ContainsActiveCorner())
						continue;

					m_RenderPoints.push_back(ci);
					
					Progress::SetProgress(float(GetPos(t, z, y, x)));
				}
			}
		}
	}

	Progress::End();
}

HRESULT PointCloud::CreateVertexBuffer()
{
	// Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(CubeInfo) * (uint32_t)m_RenderPoints.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = m_RenderPoints.data();
	return Renderer::GetDevice()->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
}

void PointCloud::StartExport() const
{
	std::vector<Mesh::Vertex_Input> vertices;

	Progress::Start("Marching Cubes", "Running Algorithm", float(m_Shape[1] * m_Shape[2] * m_Shape[3]));
	FPoint3 pos;
	for (uint32_t z = 0; z < m_Shape[1] - 1; ++z)			// using the size of points -1 because we want 
	{														// to march through this with cubes and not points
		pos.z = (float)z;
		for (uint32_t y = 0; y < m_Shape[2] - 1; ++y)
		{
			pos.y = (float)y;
			for (uint32_t x = 0; x < m_Shape[3] - 1; ++x)
			{
				pos.x = (float)x;
				const unsigned short trIdx = GetCubeFillID(z, y, x);
				const int(&vertexIndices)[16] = Table::Triangulation[trIdx];

				for (int vertIdx = 0; vertexIndices[vertIdx] != -1; vertIdx += 3)
				{
					const FPoint3 p0 = pos + Table::VertexOffsets[vertexIndices[vertIdx]];
					const FPoint3 p1 = pos + Table::VertexOffsets[vertexIndices[vertIdx + 1]];
					const FPoint3 p2 = pos + Table::VertexOffsets[vertexIndices[vertIdx + 2]];

					// normal Calculation
					FVector3 normal{ Cross(p1 - p0, p2 - p0) };
					Normalize(normal);

					vertices.emplace_back(p0, -normal);
					vertices.emplace_back(p1, -normal);
					vertices.emplace_back(p2, -normal);

					Progress::SetProgress(float((z * m_Shape[2] * m_Shape[3]) + (y * m_Shape[3]) + x));
				}
			}
		}
	}
	Progress::End();

	IOFiles::ExportMesh(new Mesh(
		GetMeshName() + "_Isosurface"
		, vertices
		, GetTransform()
	), m_Filename);
}

float PointCloud::GetValue(uint32_t t, uint32_t z, uint32_t y, uint32_t x) const
{
	// return rubbish value in case one of the indices are out of bounds
	if (z >= m_Shape[1] || y >= m_Shape[2] || x >= m_Shape[3])
		return m_RubbishValue;

	return m_PointCloud[GetPos(t,z,y,x)];
}

unsigned char PointCloud::GetCubeFillID(uint32_t z, uint32_t y, uint32_t x) const
{
	std::bitset<8> c;													/*			 6 +----------+	7																	*/                                    
		c[0] = GetValue( 0, z    , y    , x    ) > m_RubbishValue;	/*			   |\         |\																	*/
		c[1] = GetValue( 0, z    , y    , x + 1) > m_RubbishValue;	/*			   | \        | \																	*/
		c[2] = GetValue( 0, z    , y + 1, x + 1) > m_RubbishValue;	/*			   |2 +----------+ 3																*/
		c[3] = GetValue( 0, z    , y + 1, x    ) > m_RubbishValue;	/*			   |  |       |  |																	*/
		c[4] = GetValue( 0, z + 1, y    , x    ) > m_RubbishValue;	/*			 4 +--|-------+ 5|																	*/
		c[5] = GetValue( 0, z + 1, y    , x + 1) > m_RubbishValue;	/*			    \ |        \ |																	*/
		c[6] = GetValue( 0, z + 1, y + 1, x + 1) > m_RubbishValue;	/*			     \|         \|																	*/
		c[7] = GetValue( 0, z + 1, y + 1, x    ) > m_RubbishValue;	/*			    0 +----------+ 1																*/
																		
	return static_cast<unsigned char>(c.to_ulong());
}

void PointCloud::SlicePointCloud(const FVector3& orientation)
{
	for (CubeInfo& cube : m_RenderPoints)
	{
		auto vec1 = cube.pos - m_pSlicePlane->GetPosition();

		if (Dot(vec1, orientation) < 0)
			cube.pos.x += float(*std::ranges::max_element(m_Shape)) * 1.5f;
	}

	CreateVertexBuffer();
}
