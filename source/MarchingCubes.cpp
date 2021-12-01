#include "pch.h"
#include "MarchingCubes.h"
#include "MarchTable.h"

MarchingCubes::MarchingCubes()
{
	Initialize();
}

MarchingCubes::MarchingCubes(PointCloud* pPointCloud)
	: m_pPointCloud{ pPointCloud }
{
	Initialize();
}

bool MarchingCubes::GenerateMesh()
{
	if (!m_pPointCloud)
		return false;

	const std::vector<FPoint3>& points = m_pPointCloud->GetPoints();
	const std::vector<uint32_t>& shape = m_pPointCloud->GetShape();




	return true;
}

void MarchingCubes::Initialize()
{
}
