#pragma once
#include <vector>
#include <unordered_map>
#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"

class SceneGraph final
{
public:
	~SceneGraph();

	static SceneGraph* GetInstance();

	// Setters and Adders
	void AddObject(Mesh* pMesh);
	void SetCamera(Camera* pCamera);
	void AddTexture(const std::string& name, Texture* pTexture);

	// Getters
	const std::vector<Mesh*>& GetMeshes() const;
	Camera* GetCamera() const;
	Texture* GetTexture(const std::string& name) const;

private:
	SceneGraph();

	std::vector<Mesh*> m_pMeshes;
	std::unordered_map<std::string, Texture*> m_pTextures;
	Camera* m_pCamera;
};
