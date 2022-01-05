 #include "pch.h"
#include "IOFiles.h"
#include <fstream>
#include <regex>
#include <algorithm>
#include <numeric>
#include "SceneGraph.h"
#include "Progress.h"
#include <ios>
#include <thread>

IOFiles::~IOFiles()
{
}

bool IOFiles::ExportMesh(Mesh* mesh, std::string fileName)
{
	// check if filename is valid
	static const std::regex fileCheck{ "^[\\w\\-. ]+$" };
	std::smatch m{};
	if (!std::regex_match(fileName, m, fileCheck))
	{
		if (fileName.empty())
		{
			fileName = mesh->GetMeshName();
		}
		else
		{
			std::cout << "Invalid export filename!\n";
			return false;
		}
	}

	// convert data from buffer to stringstream
	std::stringstream vStream, fStream;
	std::vector<FPoint3> verts;
	//std::unordered_set<FPoint3, FPoint3> verts;
	int indexAmt{ 0 };
	const std::vector<Mesh::Vertex_Input>& vertexData = mesh->GetVertexData();

	Progress::Start("Exporting Mesh", "Constructing vertices", vertexData.size());
	for (size_t f = 0; f < vertexData.size(); f += 3)
	{
		IPoint3 index{};
		AddVertexAndAssignIndex(verts, vertexData[f    ].Position, index.x);
		AddVertexAndAssignIndex(verts, vertexData[f + 1].Position, index.y);
		AddVertexAndAssignIndex(verts, vertexData[f + 2].Position, index.z);

		// line example - "f 1 5 2"
		fStream << 'f' << ' '
			<< index.x << ' '
			<< index.y << ' '
			<< index.z << '\n';

		++indexAmt;
		Progress::SetValue(float(f));
	}
	Progress::Reset("Constructing indices", float(verts.size()));
	for (size_t v = 0; v < verts.size(); ++v)
	{
		// line example - "v -1.06 2.56 5.1"
		vStream << 'v' << ' ' 
			<< verts[v].x	<< ' ' 
			<< verts[v].y	<< ' '
			<< verts[v].z	<< '\n';
		Progress::SetValue(float(v));
	}


	// write data to obj file
	std::ofstream out;
	out.open("Resources/Export/" + fileName + ".obj");
	if (out.is_open())
	{
		out << "# " << verts.size() << " Vertices\n";
		out << vStream.str();
		out << "\n# " << indexAmt << " Faces\n";
		out << fStream.str();

		out.close();
	}
	
	Progress::End();
	return true;
}

void IOFiles::ImportFile(const std::string& file, std::string name, const FPoint3& pos)
{
	auto inst = GetInstance();
	Progress::Start("Start loading in Mesh");

	std::regex fileType{ ".+\\/(.+)\\.(.+)$" };
	std::smatch sm{};

	const std::string importLocation{ "Resources/Import/" + file};

	if (std::regex_match(importLocation, sm, fileType))
	{
		if (name.empty())
			name = sm[1];

		if (sm[2] == "obj")
			inst->ImportOBJData(importLocation, name, pos);
		else if (sm[2] == "vtk")
			inst->ImportVTKData(importLocation, name, pos);
	/*	else if (sm[2] == "vvtk")
			inst->ImportVoxelData(importLocation, vertices, indices);*/
		else if (sm[2] == "var")
			inst->ImportIthildinFile(importLocation, name, pos);
		else
			std::cout << "Unsupported file extension\n";
	}
	Progress::End();
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

void IOFiles::ImportOBJData(const std::string& file, const std::string& fileName, const FPoint3& pos)
{
	std::vector<Mesh::Vertex_Input> vertices;
	std::vector<uint32_t> indices;

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

	SceneGraph::GetInstance()->AddObject(
		new Mesh(fileName, vertices, pos)
	);
}

void IOFiles::ImportVTKData(const std::string& file, const std::string& fileName, const FPoint3& pos)
{
	std::vector<Mesh::Vertex_Input> vertices;
	std::vector<uint32_t> indices;

	// open file
	std::ifstream myFile;
	std::string line;

	std::vector<FPoint3> tempVerts;
	std::vector<IPoint3> tempIndices;

	// remove the .vtk extension, so it can open the other files with their specific file extensions
	std::string newFile{ file.begin(), file.end() - 4 };

	// open vertex position file
	myFile.open(newFile + ".pts");
	if(myFile.is_open())
	{
		std::getline(myFile, line);
		size_t vertCount = std::stoi(line);
		tempVerts.resize(vertCount);

		Progress::Reset("Importing Vertices", float(tempVerts.size()));
		for (size_t i = 0; i < tempVerts.size(); ++i)
		{
			FPoint3& vert = tempVerts[i];

			myFile >> vert.x >> vert.y >> vert.z;
			vert.x /= 1000;
			vert.y /= 1000;
			vert.z /= 1000;

			Progress::SetValue(float(i));
		}
		myFile.close();
	}

	myFile.open(newFile + ".surf");
	if (myFile.is_open())
	{
		while (std::getline(myFile, line))
		{
			if (line.empty()) // check to see if it failed to insert the next line
			{
				std::getline(myFile, line);
				if (line.empty()) // if still no line found, break out of the loop
					break;
			}

			size_t indCount = std::stoi(line);
			tempIndices.reserve(tempIndices.size() + indCount);

			std::string c;
			Progress::Reset("Importing Indices", float(tempIndices.size()));

			for (size_t i = 0; i < indCount; ++i)
			{
				IPoint3 index{};
				myFile >> c >> index.x >> index.y >> index.z;
				tempIndices.push_back(index);

				Progress::SetValue(float(i));
			}
		}
		myFile.close();
	}


	Progress::Reset("Constructing Mesh", float(tempIndices.size()));
	for (uint32_t i = 0; i < tempIndices.size(); ++i)
	{
		constexpr int vertsPerLoop = 3;
		indices.reserve(tempIndices.size() * vertsPerLoop);
		vertices.reserve(tempIndices.size() * vertsPerLoop);

		for (size_t j = 0; j < vertsPerLoop; j++)
			indices.emplace_back(uint32_t(uint64_t(i) * vertsPerLoop + j));

		const IPoint4& index = tempIndices[i];

		const FPoint3& p0 = tempVerts[index.x];
		const FPoint3& p1 = tempVerts[index.y];
		const FPoint3& p2 = tempVerts[index.z];

		// normal Calculation
		FVector3 normal{ Cross(p1 - p0, p2 - p0) };
		Normalize(normal);

		// triangle
		vertices.emplace_back(p0, -normal);
		vertices.emplace_back(p1, -normal);
		vertices.emplace_back(p2, -normal);

		Progress::SetValue(float(i));
	}
	


	SceneGraph::GetInstance()->AddObject(
		new Mesh(fileName, vertices, pos)
	);
}

void IOFiles::ImportVoxelData(const std::string& file, const std::string& fileName, const FPoint3& pos)
{
	std::vector<Mesh::Vertex_Input> vertices;
	std::vector<uint32_t> indices;

	// open file
	std::ifstream myFile;
	std::string line;

	std::vector<FPoint3> tempVerts;
	std::vector<std::pair<int, IPoint4>> tempIndices;

	// remove the .vtk extension, so it can open the other files with their specific file extensions
	std::string newFile{ file.begin(), file.end() - 5 };

	// open vertex position file
	myFile.open(newFile + ".pts");
	if (myFile.is_open())
	{
		std::getline(myFile, line);
		size_t vertCount = std::stoi(line);
		tempVerts.resize(vertCount);

		Progress::Reset("Importing Vertices", float(tempVerts.size()));
		for (size_t i = 0; i < tempVerts.size(); ++i)
		{
			FPoint3& vert = tempVerts[i];

			myFile >> vert.x >> vert.y >> vert.z;
			vert.x /= 1000;
			vert.y /= 1000;
			vert.z /= 1000;

			Progress::SetValue(float(i));
		}
		myFile.close();
	}

	myFile.open(newFile + ".elem");
	if (myFile.is_open())
	{
		while (std::getline(myFile, line))
		{
			size_t indCount = std::stoi(line);
			tempIndices.resize(indCount);

			std::string c;
			Progress::Reset("Importing Indices", float(tempIndices.size()));

			for (size_t i = 0; i < tempIndices.size(); ++i)
			{
				IPoint4& index = tempIndices[i].second;
				myFile >> c >> index.x >> index.y >> index.z >> index.w >> tempIndices[i].first;

				Progress::SetValue(float(i));
			}
		}
		myFile.close();
	}

	Progress::Reset("Constructing Mesh", float(tempIndices.size()));
	for (uint32_t i = 0; i < tempIndices.size(); ++i)
	{
		constexpr int vertsPerLoop = 6;
		indices.reserve(tempIndices.size() * vertsPerLoop);
		vertices.reserve(tempIndices.size() * vertsPerLoop);

		for (size_t j = 0; j < vertsPerLoop; j++)
			indices.emplace_back(uint64_t(i) * vertsPerLoop + j);

		const IPoint4& index = tempIndices[i].second;

		const FPoint3& p0 = tempVerts[index.x];
		const FPoint3& p1 = tempVerts[index.y];
		const FPoint3& p2 = tempVerts[index.z];
		const FPoint3& p3 = tempVerts[index.w];

		// normal Calculation
		FVector3 normal{ Cross(p1 - p0, p2 - p0) };
		Normalize(normal);

		// triangle
		vertices.emplace_back(p0, -normal);
		vertices.emplace_back(p1, -normal);
		vertices.emplace_back(p2, -normal);
		// triangle2
		vertices.emplace_back(p1, -normal);
		vertices.emplace_back(p3, -normal);
		vertices.emplace_back(p2, -normal);

		Progress::SetValue(float(i));
	}

	SceneGraph::GetInstance()->AddObject(
		new Mesh(fileName, vertices, pos)
	);
}

void IOFiles::ImportIthildinFile(const std::string& file, const std::string& fileName, const FPoint3& pos)
{
	std::vector<float> data;
	std::vector<uint32_t> shape;

	std::ifstream f;
	
	f.open(file, std::fstream::binary);
	if (f.is_open())
	{
		ReadVarHeader(f, shape);
		
		// read all information in data
		const int product = std::accumulate(shape.cbegin(), shape.cend(), 1, std::multiplies<int>());

		data.resize(product);
		// read all the binary data into data
		f.read((char*)data.data(), product * sizeof(float));

		// it is possible that data is too short
		const uint32_t nfr = uint32_t(std::floorf(float(data.size()) / std::accumulate(shape.cbegin() + 1, shape.cend(), 1, std::multiplies<int>())));
		if (nfr < shape[0])
			std::cout << "Not all frames recorded in varfile.\n";
		else
			assert(nfr == shape[0]);
		shape[0] = nfr;

		// check if last frame is corrupt
		if (data.size() > product)
			std::cout << "Part of the last frame is missing! Frame skipped.";

		// create mesh
		SceneGraph::GetInstance()->AddObject(
			new PointCloud(fileName, data, shape, pos)
		);

		f.close();
	}

}

void IOFiles::ReadVarHeader(std::ifstream& f, std::vector<uint32_t>& shape) const
{
	float frameDur;
	uint32_t nDim;
	uint32_t nfr, bytesPerData;

	f.read((char*)&nDim, sizeof(uint32_t));
	shape.resize(nDim);
	for (uint32_t i = 0; i < nDim; i++)
		f.read((char*)&shape[i], sizeof(uint32_t));
	f.read((char*)&nfr, sizeof(uint32_t));
	f.read((char*)&bytesPerData, sizeof(uint32_t));
	if (bytesPerData != 4)
	{
		std::cout << "BytesPerData is not 4\n";
		return;
	}
	f.read((char*)&frameDur, sizeof(float));

	// this will make sure the shape size is always at least 3
	if (shape.size() < 3)
		shape.resize(3, 1); // default value is 1


	// add the nfr to this vector
	std::reverse(shape.begin(), shape.end());	// reverse current shape (from kji to ijk)
	shape.push_back(nfr);						// add the nfr to shape (ijkt) 
	std::reverse(shape.begin(), shape.end());	// reverse this again (from ijkt to tkji)

	for (uint32_t i : shape)
	{
		if (i <= 0)
		{
			std::cout << "Dimensions must be greater than 0";
			return;
		}
	}
}
