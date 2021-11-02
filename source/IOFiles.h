#pragma once
#include <string>
#include <vector>
#include "Mesh.h"

using namespace Elite;

class IOFiles final
{
public:
	~IOFiles();
	static void Initialize() { GetInstance(); }

	static float GetProgess() { return GetInstance()->m_Progress; }
	static bool ExportMesh(Mesh* mesh, const std::string& fileName = "ExportedMesh", const std::string& location = "Resources/Export/");
	static void ImportFile(ID3D11Device* pDevice, const std::string& file, const std::string& name, const FPoint3& pos = {0,0,0});

private:
	IOFiles() = default;
	static IOFiles* GetInstance();

	void AddVertexAndAssignIndex(std::vector<FPoint3>& vector, const FPoint3& vertex, int& index);

	void ImportOBJData(const std::string& file, std::vector<Mesh::Vertex_Input>& vertices, std::vector<uint32_t>& indices);
	void ImportVTKData(const std::string& file, std::vector<Mesh::Vertex_Input>& vertices, std::vector<uint32_t>& indices);

	float m_Progress = 0.f;
};

