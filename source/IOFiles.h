#pragma once
#include <string>
#include <vector>
#include "Objects.h"

using namespace Elite;

class IOFiles final
{
public:
	~IOFiles();
	static void Initialize(ID3D11Device* pDevice) { GetInstance()->m_pDevice = pDevice; }

	static bool ExportMesh(Mesh* mesh, const std::string& fileName = "ExportedMesh", const std::string& location = "Resources/Export/");
	static void ImportFile(const std::string& file, std::string name = "", const FPoint3& pos = {0,0,0});


private:
	IOFiles() = default;
	static IOFiles* GetInstance()
	{
		static IOFiles* pInstance{ new IOFiles() };
		return pInstance;
	}
	ID3D11Device* m_pDevice = nullptr;

	void AddVertexAndAssignIndex(std::vector<FPoint3>& vector, const FPoint3& vertex, int& index);

	void ImportOBJData(const std::string& file, const std::string& fileName, const FPoint3& pos);
	void ImportVTKData(const std::string& file, const std::string& fileName, const FPoint3& pos);
	void ImportVoxelData(const std::string& file, const std::string& fileName, const FPoint3& pos);
	void ImportIthildinFile(const std::string& file, const std::string& fileName, const FPoint3& pos);
	void ReadVarHeader(std::ifstream& f, std::vector<uint32_t>& shape) const;
};

