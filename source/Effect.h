#pragma once
#include <sstream>


class Effect final
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique() const;

	void UpdateMatrix(const Elite::FMatrix4& transform);

private:
	ID3DX11Effect* m_pEffect = nullptr;
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable	= nullptr;
	ID3DX11EffectMatrixVariable* m_pMatWorldVariable			= nullptr;
	ID3DX11EffectMatrixVariable* m_pMatViewInverse				= nullptr;

	uint32_t m_TechSize = 0;
	uint32_t m_TechIndex = 0;
	ID3DX11EffectGroup* m_pTechniqueGroup = nullptr;

	ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile) const;
};