#pragma once
#include "Objects.h"

class MarchingCubes// : public IsoSurface
{
public:
	MarchingCubes(const PointCloud* pc);
	MarchingCubes(MarchingCubes&&) = delete;
	MarchingCubes(const MarchingCubes&) = delete;
	MarchingCubes& operator=(MarchingCubes&&) = delete;
	MarchingCubes& operator=(const MarchingCubes&) = delete;

	void GenerateMesh();

private:
	const PointCloud* m_pPointCloud = nullptr;
	const float m_RubbishValue;

	unsigned char GetCubeFillID(uint32_t z, uint32_t y, uint32_t x);
};

