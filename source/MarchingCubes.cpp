#include "pch.h"
#include "MarchingCubes.h"
#include "MarchTable.h"
#include <bitset>
#include "SceneGraph.h"
#include "Progress.h"
#include "ERenderer.h"

MarchingCubes::MarchingCubes(const PointCloud* pPointCloud)
	: m_pPointCloud{ pPointCloud }
	, m_Points{ pPointCloud->GetPoints() }
	, m_Shape{ pPointCloud->GetShape() }
	, m_RubbishValue{ m_Points[0] }
{
}

bool MarchingCubes::GenerateMesh()
{
	std::vector<Mesh::Vertex_Input> vertices;
	

	for (uint32_t z = 0; z < m_Shape[1] - 1; ++z)			// using the size of points -1 because we want 
	{													// to march through this with cubes and not points
		for (uint32_t y = 0; y < m_Shape[2] - 1; ++y)
		{
			for (uint32_t x = 0; x < m_Shape[3] - 1; ++x)
			{
				unsigned char cube = GetCubeFillID(z, y, x);

			}
		}
	}

	
	SceneGraph::GetInstance()->AddObject(new Mesh(m_MeshName + "_Isosurface", vertices));

	return true;
}

unsigned char MarchingCubes::GetCubeFillID(uint32_t z, uint32_t y, uint32_t x)
{
	std::bitset<8> c;
		c[0] = m_Points[GetIndex(z    , y    , x    )] > m_RubbishValue;
		c[1] = m_Points[GetIndex(z    , y    , x + 1)] > m_RubbishValue;
		c[2] = m_Points[GetIndex(z    , y + 1, x    )] > m_RubbishValue;
		c[3] = m_Points[GetIndex(z    , y + 1, x + 1)] > m_RubbishValue;
		c[4] = m_Points[GetIndex(z + 1, y    , x    )] > m_RubbishValue;
		c[5] = m_Points[GetIndex(z + 1, y    , x + 1)] > m_RubbishValue;
		c[6] = m_Points[GetIndex(z + 1, y + 1, x    )] > m_RubbishValue;
		c[7] = m_Points[GetIndex(z + 1, y + 1, x + 1)] > m_RubbishValue;

	return static_cast<unsigned char>(c.to_ulong());
}

uint32_t MarchingCubes::GetIndex(uint32_t z, uint32_t y, uint32_t x)
{
	return uint32_t((z * m_Shape[2] * m_Shape[3]) + (y * m_Shape[3]) + x);
}
