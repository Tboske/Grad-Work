#include "pch.h"
#include "SceneGraph.h"

SceneGraph::~SceneGraph()
{
	// cleanup meshes
	for (auto& i : m_pMeshes)
	{
		delete i;
		i = nullptr;
	}

	for (auto& pTexture : m_pTextures)
	{
		delete pTexture.second;
		pTexture.second = nullptr;
	}

	delete m_pCamera;
	m_pCamera = nullptr;
}

SceneGraph* SceneGraph::GetInstance()
{
	static SceneGraph* pInstance{ new SceneGraph() };
	return pInstance;
}

void SceneGraph::AddMesh(Mesh* pMesh)
{
	m_pMeshes.push_back(pMesh);
}

void SceneGraph::SetCamera(Camera* pCamera)
{
	m_pCamera = pCamera;
}

void SceneGraph::AddTexture(const std::string& name, Texture* pTexture)
{
	// key is always lowercase
	m_pTextures[name] = pTexture;
}

const std::vector<Mesh*>& SceneGraph::GetMeshes() const
{
	return m_pMeshes;
}

Camera* SceneGraph::GetCamera() const
{
	return m_pCamera;
}

Texture* SceneGraph::GetTexture(const std::string& name) const
{
	// key is always lowercase
	auto found = m_pTextures.find(name);
	if (found != m_pTextures.cend())
		return found->second;

	return m_pTextures.at("NotFound");
}

SceneGraph::SceneGraph()
	: m_pMeshes{ }
	, m_pCamera{ nullptr }
{
}
