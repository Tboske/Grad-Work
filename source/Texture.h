#pragma once
#include <SDL_image.h>
#include <string>
#include <vector>
#include "EMath.h"
#include "ERGBColor.h"

class Texture final
{
public:
	Texture(ID3D11Device* pDevice, const std::string& filename);
	~Texture();

	ID3D11ShaderResourceView* GetSRV() const;

private:
	size_t m_Width;
	size_t m_Height;

	// DirectX 
	ID3D11Texture2D* m_pTexture = nullptr;
	ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
};