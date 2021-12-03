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
		Vertex_Input(const FPoint3& pos, const FVector3& normal)
		{
			Position = pos;
			Normal = normal;
		}
		//void GenerateNormals();

		FPoint3 Position;
		FVector3 Normal;
	};

	Mesh(const std::string& meshName, const std::vector<Vertex_Input>& vertices, const FPoint3& pos = { 0,0,0 }, const RGBColor color = { 0.75f, 0.4f, 0.4f });
	Mesh(const std::string& meshName, const std::vector<Vertex_Input>& vertices, const FMatrix4& transform);
	virtual ~Mesh();

	const std::vector<Vertex_Input>& GetVertexData() const { return m_Vertices; }

	virtual void Render(ID3D11DeviceContext* pDeviceContext) const override;
	virtual void RenderUI() override;

private:
	ID3D11InputLayout* m_pVertexLayout = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	std::vector<Vertex_Input> m_Vertices;
	ID3DX11EffectVectorVariable* m_pColorEffectVariable = nullptr;
	RGBColor m_Color;

	HRESULT Initialize();
};

