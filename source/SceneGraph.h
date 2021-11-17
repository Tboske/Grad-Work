#pragma once
#include <vector>
#include <unordered_map>
#include "Texture.h"
#include "Camera.h"
#include "BaseObject.h"
#include "Mesh.h"

class SceneGraph final
{
public:
	~SceneGraph();

	static SceneGraph* GetInstance();

	// Setters and Adders
	void AddObject(BaseObject* pMesh);
	void SetCamera(Camera* pCamera);
	void AddTexture(const std::string& name, Texture* pTexture);

	// Getters
	const std::vector<BaseObject*>& GetObjects() const;
	Camera* GetCamera() const;
	Texture* GetTexture(const std::string& name) const;

private:
	SceneGraph();

	std::vector<BaseObject*> m_pObjects;
	std::unordered_map<std::string, Texture*> m_pTextures;
	Camera* m_pCamera;
};
