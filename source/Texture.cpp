#include "pch.h"
#include "Texture.h"

Texture::Texture(ID3D11Device* pDevice, const std::string& filename)
{
	SDL_Surface* pSurface = IMG_Load(filename.c_str());
	m_Height = pSurface->h;
	m_Width = pSurface->w;

	// create texture description
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = (UINT)m_Width;
	desc.Height = (UINT)m_Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pSurface->pixels;
	initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(pSurface->pitch * pSurface->h);

	HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC sRVDesc{};
	sRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sRVDesc.Texture2D.MipLevels = 1;

	if (SUCCEEDED(hr))
	{
		hr = pDevice->CreateShaderResourceView(m_pTexture, &sRVDesc, &m_pShaderResourceView);
	}

	// release textrue info 
	SDL_FreeSurface(pSurface);
}

Texture::~Texture()
{
	m_pShaderResourceView->Release();
	m_pShaderResourceView = nullptr;
	m_pTexture->Release();
	m_pTexture = nullptr;
}

ID3D11ShaderResourceView* Texture::GetSRV() const
{
	return m_pShaderResourceView;
}
