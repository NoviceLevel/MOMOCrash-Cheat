#pragma once
#include "pch.h"

#include <D3D11.h>
#include <DXGI.h>
#include "../../ImGUI/Styles.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace D3D11Hook
{
	typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

	inline Present oPresent = nullptr;
	inline WNDPROC oWndProc = nullptr;
	inline HWND window = NULL;
	inline ID3D11Device* pDevice = NULL;
	inline ID3D11DeviceContext* pContext = NULL;
	inline ID3D11RenderTargetView* mainRenderTargetView = nullptr;
	inline bool hkInit = false;

	void InitImGui()
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(pDevice, pContext);

		SetupStyle();
		ImportFonts();
	}

	LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		if (GUI::bMenuOpen)
		{
			switch (uMsg)
			{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_MOUSEMOVE:
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
				return true;
			}
		}

		return CallWindowProcA(oWndProc, hWnd, uMsg, wParam, lParam);
	}

	HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		if (!hkInit)
		{
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
			{
				pDevice->GetImmediateContext(&pContext);

				DXGI_SWAP_CHAIN_DESC sd;
				pSwapChain->GetDesc(&sd);
				window = sd.OutputWindow;
				
				ID3D11Texture2D* pBackBuffer;
				pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
				if (pBackBuffer != NULL)
				{
					pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
					pBackBuffer->Release();
				}

				oWndProc = (WNDPROC)SetWindowLongPtrA(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
				InitImGui();

				hkInit = true;
				return oPresent(pSwapChain, SyncInterval, Flags);
			}
			else
				return oPresent(pSwapChain, SyncInterval, Flags);
		}

		GUI::BeginRender();
		ImGui::PushFont(tahomaFont);
		GUI::Render();
		ImGui::PopFont();
		GUI::EndRender();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (!Cheat::bShouldRun)
		{
			for (size_t i = 0; i < Features.size(); i++)
			{
				Features[i].get()->Destroy();
			}
			
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			
			if (oWndProc)
				SetWindowLongPtrA(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
			
			if (mainRenderTargetView)
			{
				mainRenderTargetView->Release();
				mainRenderTargetView = nullptr;
			}
			
			if (pContext)
			{
				pContext->Release();
				pContext = nullptr;
			}
			if (pDevice)
			{
				pDevice->Release();
				pDevice = nullptr;
			}
			
			kiero::shutdown();
			hkInit = false;
			return oPresent(pSwapChain, SyncInterval, Flags);
		}

		return oPresent(pSwapChain, SyncInterval, Flags);
	}
} // namespace D3D11Hook
