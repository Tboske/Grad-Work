#pragma once
#include "BaseObject.h"
#include "Texture.h"
#include "Effect.h"

using namespace Elite;

class Mesh final : public BaseObject
{
public:
	struct Vertex_Input
	{
		Vertex_Input(const FPoint3& pos, const FVector3& normal, const Elite::FPoint3& color = {1,1,1})
		{
			Position = pos;
			Normal = normal;
			Color = color;
		}

		FPoint3 Position;
		FVector3 Normal;
		FPoint3 Color;
	};

	Mesh(ID3D11Device* pDevice, const std::string& meshName, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices, const FPoint3& pos = { 0,0,0 });
	~Mesh();

	const std::vector<Vertex_Input>& GetVertexData() const { return m_Vertices; }

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const override;
	virtual void Update() override;

private:
	ID3D11InputLayout* m_pVertexLayout = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;
	std::vector<Vertex_Input> m_Vertices;
	std::vector<uint32_t> m_Indices;

	HRESULT Initialize(ID3D11Device* pDevice, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices);
};

