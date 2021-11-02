#include "pch.h"
#include "IOFiles.h"
#include <fstream>
#include <regex>
#include <chrono>
#include "SceneGraph.h"

using namespace std::chrono;


IOFiles::~IOFiles()
{
}

IOFiles* IOFiles::GetInstance()
{
	static IOFiles* pInstance{ new IOFiles() };
	return pInstance;
}

bool IOFiles::ExportMesh(Mesh* mesh, const std::string& fileName, const std::string& location)
{
	// check if filename is valid
	static const std::regex fileCheck{ "^([[:alpha:]].*?)(?:\\.[[:alpha:]]*)?$" };
	std::smatch m{};
	if (!std::regex_match(fileName, m, fileCheck))
	{
		std::cout << "Invalid export filename!\n";
		return false;
	}

	// convert data from buffer to stringstream
	std::stringstream vStream, fStream;
	std::vector<FPoint3> verts;
	//std::unordered_set<FPoint3, FPoint3> verts;
	int indexAmt{ 0 };
	const std::vector<Mesh::Vertex_Input>& vertexData = mesh->GetVertexData();
	for (size_t v = 0; v < vertexData.size(); v += 3)
	{
		IPoint3 index{};
		GetInstance()->AddVertexAndAssignIndex(verts, vertexData[v    ].Position, index.x);
		GetInstance()->AddVertexAndAssignIndex(verts, vertexData[v + 1].Position, index.y);
		GetInstance()->AddVertexAndAssignIndex(verts, vertexData[v + 2].Position, index.z);

		//auto x = verts.insert(vertexData[v].Position);

		// line example - "f 1 5 2"
		fStream << 'f' << ' '
			<< index.x << ' '
			<< index.y << ' '
			<< index.z << '\n';

		++indexAmt;
	}
	for (const auto& v : verts)
	{
		// line example - "v -1.06 2.56 5.1"
		vStream << 'v' << ' ' 
			<< v.x	<< ' ' 
			<< v.y	<< ' '
			<< v.z	<< '\n';
	}


	// write data to obj file
	std::ofstream out;
	out.open(location + fileName + ".obj");
	if (out.is_open())
	{
		out << "# " << verts.size() << " Vertices\n";
		out << vStream.str();
		out << "\n# " << indexAmt << " Faces\n";
		out << fStream.str();

		out.close();
	}
	std::cout << "Export Successful!\n";
	return true;
}

void IOFiles::ImportFile(ID3D11Device* pDevice, const std::string& file, const std::string& name, const FPoint3& pos)
{
	std::regex fileType{ ".+\\.(.+)$" };
	std::smatch sm{};

	if (std::regex_match(file, sm, fileType))
	{
		std::vector<Mesh::Vertex_Input> vertices;
		std::vector<uint32_t> indices;

		static const std::string importFolder{ "Resources/Import/" };
		if (sm[1] == "obj")
			GetInstance()->ImportOBJData(importFolder + file, vertices, indices);
		else if (sm[1] == "vtk")
			GetInstance()->ImportVTKData(importFolder + file, vertices, indices);
		else
			std::cout << "Unsupported file extension\n";

		SceneGraph::GetInstance()->AddMesh(
			new Mesh(pDevice, name, vertices, indices, pos)
		);
	}
}

void IOFiles::AddVertexAndAssignIndex(std::vector<FPoint3>& vector, const FPoint3& vertex, int& index)
{
	// check for existing value
	auto foundVal = std::find(vector.cbegin(), vector.cend(), vertex);
	if (foundVal != vector.cend())
		// if value already exists, uses this code
		index = int(foundVal - vector.cbegin());
	else
	{
		vector.emplace_back(vertex);
		index = int(vector.size() - 1);
	}
	// increase index by 1, obj files start counting from 1 and not from 0
	++index;

}

void IOFiles::ImportOBJData(const std::string& file, std::vector<Mesh::Vertex_Input>& vertices, std::vector<uint32_t>& indices)
{
	// open file
	std::ifstream myFile;

	std::vector<FPoint3> tempVerts;
	std::vector<FVector3> tempNormals;
	std::vector<IPoint3> tempIndices;

	myFile.open(file);
	if (myFile.is_open())
	{
		// regex and line inits
		std::string line;
		std::smatch sm;

		std::regex regexVert{ "^v\\s+(-?[[:digit:]\\.]+)\\s?(-?[[:digit:]\\.]+)\\s?(-?[[:digit:]\\.]+)$" };
		std::regex regexVertNormals{ "^vn\\s+(-?[[:digit:]\\.]+)\\s?(-?[[:digit:]\\.]+)\\s?(-?[[:digit:]\\.]+)$" };
		std::regex regexUV{ "^vt\\s+(-?[[:digit:]\\.]+)\\s?(-?[[:digit:]\\.]+)\\s?(-?[[:digit:]\\.]+)$" };
		// match {vert1, uv1, normal1, vert2, uv2, normal2, vert3, uv3, normal3}
		std::regex regexF{ "^f\\s+([0-9]+)/([0-9]+)/([0-9]+)\\s+([0-9]+)/([0-9]+)/([0-9]+)\\s+([0-9]+)/([0-9]+)/([0-9]+)\\s*$" };
		while (std::getline(myFile, line))
		{
			// ===== Vertices =====
			if (std::regex_match(line, sm, regexVert))
				tempVerts.push_back({ std::stof(sm[1]), std::stof(sm[2]), -std::stof(sm[3]) });
			// ===== Vertex normals =====
			else if (std::regex_match(line, sm, regexVertNormals))
				tempNormals.push_back({ std::stof(sm[1]), std::stof(sm[2]), -std::stof(sm[3]) });
			// ===== Indices =====
			else if (std::regex_match(line, sm, regexF))
			{
				// match {vert1, uv1, normal1
				tempIndices.push_back({ std::stoi(sm[1]) - 1 , std::stoi(sm[2]) - 1 , std::stoi(sm[3]) - 1 });
				//							 , vert2, uv2, normal2
				tempIndices.push_back({ std::stoi(sm[4]) - 1 , std::stoi(sm[5]) - 1 , std::stoi(sm[6]) - 1 });
				//												  , vert3, uv3, normal3}
				tempIndices.push_back({ std::stoi(sm[7]) - 1 , std::stoi(sm[8]) - 1 , std::stoi(sm[9]) - 1 });
			}
		}
	}
	myFile.close();

	for (uint32_t i = 0; i < tempIndices.size(); i++)
	{
		indices.push_back(i);
		const IPoint3& index = tempIndices[i];
		vertices.push_back(Mesh::Vertex_Input(tempVerts[index.x], tempNormals[index.z]));
	}
}

void IOFiles::ImportVTKData(const std::string& file, std::vector<Mesh::Vertex_Input>& vertices, std::vector<uint32_t>& indices)
{
	// open file
	std::ifstream myFile;
	std::string line;

	std::vector<FPoint3> tempVerts;
	std::vector<IPoint4> tempIndices;

	std::string newFile{ file.begin(), file.end() - 4 };

	// open vertex position file
	auto startT = high_resolution_clock::now();
	{
		myFile.open(newFile + ".pts");
		if(myFile.is_open())
		{
			std::getline(myFile, line);
			size_t vertCount = std::stoi(line);
			tempVerts.resize(vertCount);

			for (FPoint3& vert : tempVerts)
			{
				myFile >> vert.x >> vert.y >> vert.z;
				vert.x /= 1000;
				vert.y /= 1000;
				vert.z /= 1000;
			}
		}
		myFile.close();
	}

	{
		myFile.open(newFile + ".surf");
		if (myFile.is_open())
		{
			std::getline(myFile, line);
			size_t indCount = std::stoi(line);
			tempIndices.resize(indCount);
			indices.reserve(indCount);
			vertices.reserve(indCount);

			
			std::string c;

			for (auto& index : tempIndices)
			{
				myFile >> c >> index.x >> index.y >> index.z;
			}
		}
		myFile.close();


		for (uint32_t i = 0; i < tempIndices.size(); ++i)
		{
			indices.push_back(i * 3);
			indices.push_back(i * 3 + 1);
			indices.push_back(i * 3 + 2);

			const IPoint4& index = tempIndices[i];

			const FPoint3& p0 = tempVerts[index.x];
			const FPoint3& p1 = tempVerts[index.y];
			const FPoint3& p2 = tempVerts[index.z];

			// normal Calculation
			FVector3 normal{ Cross(p1 - p0, p2 - p0) };
			Normalize(normal);

			// triangle
			//FPoint3 color{ RandomFloat(), RandomFloat(), RandomFloat() };
			FPoint3 color{ 0.8f, 0.8f, 0.8f };
			vertices.push_back(Mesh::Vertex_Input(p0, normal, color));
			vertices.push_back(Mesh::Vertex_Input(p1, normal, color));
			vertices.push_back(Mesh::Vertex_Input(p2, normal, color));
		}
	}
	auto endT = high_resolution_clock::now();
	auto exT = duration_cast<seconds>(endT - startT).count();
	std::cout << "Voxel mesh loaded in: " + std::to_string(exT) + " seconds\n";
}