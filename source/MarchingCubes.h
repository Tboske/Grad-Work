#pragma once
#include "Objects.h"

class MarchingCubes// : public IsoSurface
{
public:
	MarchingCubes() = default;
	MarchingCubes(PointCloud* pc);

	void SetPointCloud(PointCloud* pc) { m_pPointCloud = pc; }
	Mesh* GetMesh() const { return m_pMesh; }
	void GenerateMesh();

private:
	PointCloud* m_pPointCloud = nullptr;
	Mesh* m_pMesh = nullptr;
};

