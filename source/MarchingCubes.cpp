#include "pch.h"
#include "MarchingCubes.h"
#include "MarchTable.h"

MarchingCubes::MarchingCubes(PointCloud* pPointCloud)
	: m_pPointCloud{ pPointCloud }
{
}

void MarchingCubes::GenerateMesh()
{
	if (!m_pPointCloud)
		return;



}
