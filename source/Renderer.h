#pragma once
#include "UI.h"

struct SDL_Window;

class Renderer final
{
public:
	Renderer(Renderer&&) = delete;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	static void Cleanup() { delete GetInstance(); };
	static void Initialize(SDL_Window* pWindow) { GetInstance()->InitializeImpl(pWindow); };
	static void Render() { GetInstance()->RenderImpl(); };
	static ID3D11Device* GetDevice() { return GetInstance()->m_pDevice; }

private:
	Renderer() = default;
	~Renderer();
	static Renderer* GetInstance()
	{
		static Renderer* pInstance{ new Renderer() };
		return pInstance;
	}

	SDL_Window* m_pWindow = nullptr;
	uint32_t m_Width = 0;
	uint32_t m_Height = 0;

	bool m_IsInitialized = false;

	// DirectX information
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

	IDXGIFactory* m_pDXGIFactory = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11Texture2D* m_pDepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
	ID3D11Resource* m_pRenderTargetBuffer = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

	// UI Element
	UI* m_pUI = nullptr;

	HRESULT InitializeDirextX();
	void CleanUpDirectX();
	void InitializeImpl(SDL_Window* pWindow);
	void RenderImpl() const;
};

