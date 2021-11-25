#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "Objects.h"

using namespace Elite;
using namespace std::chrono;

class IOFiles final
{
public:
	~IOFiles();
	static void Initialize(ID3D11Device* pDevice) { GetInstance()->m_pDevice = pDevice; }

	static void LoadingPopUp() { GetInstance()->LoadingPopUpImpl(); }
	static bool ExportMesh(Mesh* mesh, const std::string& fileName = "ExportedMesh", const std::string& location = "Resources/Export/");
	static void ImportFile(const std::string& file, std::string name = "", const FPoint3& pos = {0,0,0});


private:
	IOFiles() = default;
	static IOFiles* GetInstance();
	ID3D11Device* m_pDevice = nullptr;

	void AddVertexAndAssignIndex(std::vector<FPoint3>& vector, const FPoint3& vertex, int& index);

	void ImportOBJData(const std::string& file, const std::string& fileName, const FPoint3& pos);
	void ImportVTKData(const std::string& file, const std::string& fileName, const FPoint3& pos);
	void ImportVoxelData(const std::string& file, const std::string& fileName, const FPoint3& pos);
	void ImportIthildinFile(const std::string& file, const std::string& fileName, const FPoint3& pos);
	void ReadVarHeader(std::ifstream& f, std::vector<uint32_t>& shape) const;

	void LoadingPopUpImpl() const;
	struct Progress
	{
		bool active = false;
		float value = 0.f;
		std::string description;
		steady_clock::time_point startTime;

		void Start(const std::string& s) 
		{ 
			active = true;
			startTime = high_resolution_clock::now();
			ResetProgress(s);
		}
		void ResetProgress(const std::string& s) 
		{
			value = 0.f;
			description = s;
		}
	};
	Progress m_Progress;
};

