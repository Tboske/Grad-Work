#include "pch.h"
#include "Effect.h"
#include "SceneGraph.h"
#include "Camera.h"


Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
	: m_pEffect{ LoadEffect(pDevice, assetFile) }
	, m_TechSize{ 1 }
{
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
		std::wcout << L"m_pMatWorldViewProjVariable not valid\n";

	m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatWorldVariable->IsValid())
		std::wcout << L"m_pMatWorldVariable not valid\n";

	m_pMatViewInverse = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	if (!m_pMatViewInverse->IsValid())
		std::wcout << L"m_pMatViewInverse not valid\n";
}

Effect::~Effect()
{
	m_pMatWorldViewProjVariable->Release();
	m_pMatWorldViewProjVariable = nullptr;
	m_pMatWorldVariable->Release();
	m_pMatWorldVariable = nullptr;
	m_pMatViewInverse->Release();
	m_pMatViewInverse = nullptr;

	m_pEffect->Release();
	m_pEffect = nullptr;
}

ID3DX11Effect* Effect::GetEffect() const
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechnique() const
{
	return m_pEffect->GetTechniqueByIndex(m_TechIndex);
}

void Effect::UpdateMatrix(const Elite::FMatrix4& transform)
{
	Camera* pCamera = SceneGraph::GetInstance()->GetCamera();

	Elite::FMatrix4 tempMatrix = transform;
	m_pMatWorldVariable->SetMatrix(&tempMatrix[0][0]);

	Elite::FMatrix4 wVPmatrix = pCamera->GetProjectionMatrix();
		wVPmatrix *= pCamera->GetONB();
		wVPmatrix *= transform;

	m_pMatViewInverse->SetMatrix(&pCamera->GetONB()[0][0]);
	m_pMatWorldViewProjVariable->SetMatrix(&wVPmatrix[0][0]);
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile) const
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			char* pErrors = (char*)pErrorBlob->GetBufferPointer();

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\n Path: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}
	return pEffect;
}


