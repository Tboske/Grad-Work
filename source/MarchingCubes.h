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

	bool GenerateMesh();

private:
	const PointCloud* m_pPointCloud = nullptr;
	const std::vector<float>& m_Points;
	const std::vector<uint32_t>& m_Shape;
	const float m_RubbishValue;
	const std::string m_MeshName;

	unsigned char GetCubeFillID(uint32_t z, uint32_t y, uint32_t x);
	uint32_t GetIndex(uint32_t z, uint32_t y, uint32_t x);
};

