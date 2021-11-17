/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render too + DirectX initialization.
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include "Objects.h"
#include "UI.h"

struct SDL_Window;
struct SDL_Surface;

namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer(); 

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		ID3D11Device* GetDevice() const;

		void Render();

	private:
		SDL_Window* m_pWindow;
		uint32_t m_Width;
		uint32_t m_Height;

		const RGBColor m_ClearBufferColor{ 0.1f, 0.1f, 0.1f };

		bool m_IsInitialized;

		// DirectX information
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pDeviceContext = nullptr;

		IDXGIFactory* m_pDXGIFactory = nullptr;
		IDXGISwapChain* m_pSwapChain = nullptr;
		ID3D11Texture2D* m_pDepthStencilBuffer = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
		ID3D11Resource* m_pRenderTargetBuffer = nullptr;
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

		// Private Functions
		HRESULT InitializeDirextX();
		void CleanUpDirectX();

		// UI Element
		UI* m_pUI = nullptr;
	};
}

#endif