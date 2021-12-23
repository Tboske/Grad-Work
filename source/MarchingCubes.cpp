#include "pch.h"
#include "MarchingCubes.h"
#include "MarchTable.h"
#include <bitset>
#include "SceneGraph.h"
#include "Progress.h"
#include "ERenderer.h"

MarchingCubes::MarchingCubes(const PointCloud* pPointCloud)
	: m_pPointCloud{ pPointCloud }
	, m_RubbishValue{ pPointCloud->GetRubbishValue() }
{
}

void MarchingCubes::GenerateMesh()
{
	std::vector<Mesh::Vertex_Input> vertices;

	const std::vector<uint32_t>& m_Shape = m_pPointCloud->GetShape();
	
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
				const int (&vertexIndices)[16] = Table::Triangulation[trIdx];

				for (int vertIdx = 0; vertexIndices[vertIdx] != -1; vertIdx += 3)
				{
					const FPoint3 p0 = pos + Table::VertexOffsets[vertexIndices[vertIdx   ]];
					const FPoint3 p1 = pos + Table::VertexOffsets[vertexIndices[vertIdx + 1]];
					const FPoint3 p2 = pos + Table::VertexOffsets[vertexIndices[vertIdx + 2]];

					// normal Calculation
					FVector3 normal{ Cross(p1 - p0, p2 - p0) };
					Normalize(normal);

					vertices.emplace_back(p0, -normal);
					vertices.emplace_back(p1, -normal);
					vertices.emplace_back(p2, -normal);

					Progress::SetValue(float((z * m_Shape[2] * m_Shape[3]) + (y * m_Shape[3]) + x));
				}
			}
		}
	}
	Progress::End();
	
	SceneGraph::GetInstance()->AddObject(new Mesh(
		m_pPointCloud->GetMeshName() + "_Isosurface"
		, vertices
		, m_pPointCloud->GetTransform()
	));
}

unsigned char MarchingCubes::GetCubeFillID(uint32_t z, uint32_t y, uint32_t x)
{
	std::bitset<8> c;																/*			 6 +----------+	7																	*/                                    
		c[0] = m_pPointCloud->GetValue( 0, z    , y    , x    ) > m_RubbishValue;	/*			   |\         |\																	*/
		c[1] = m_pPointCloud->GetValue( 0, z    , y    , x + 1) > m_RubbishValue;	/*			   | \        | \																	*/
		c[2] = m_pPointCloud->GetValue( 0, z    , y + 1, x + 1) > m_RubbishValue;	/*			   |2 +----------+ 3																*/
		c[3] = m_pPointCloud->GetValue( 0, z    , y + 1, x    ) > m_RubbishValue;	/*			   |  |       |  |																	*/
		c[4] = m_pPointCloud->GetValue( 0, z + 1, y    , x    ) > m_RubbishValue;	/*			 4 +--|-------+ 5|																	*/
		c[5] = m_pPointCloud->GetValue( 0, z + 1, y    , x + 1) > m_RubbishValue;	/*			    \ |        \ |																	*/
		c[6] = m_pPointCloud->GetValue( 0, z + 1, y + 1, x + 1) > m_RubbishValue;	/*			     \|         \|																	*/
		c[7] = m_pPointCloud->GetValue( 0, z + 1, y + 1, x    ) > m_RubbishValue;	/*			    0 +----------+ 1																*/
																		
	return static_cast<unsigned char>(c.to_ulong());
}
