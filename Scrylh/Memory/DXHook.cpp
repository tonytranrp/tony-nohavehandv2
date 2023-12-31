#include "DXHook.h"
#include "../Utils/D2DUtils.h"

#include "../NoHaveHand/Command/CommandMgr.h"
#include "../NoHaveHand/Config/ConfigManager.h"
#include "../Utils/DrawUtils.h"
#include "../Utils/ImmediateGui.h"
#include "../NoHaveHand/Menu/ConfigManagerMenu.h"
#include "../NoHaveHand/Menu/HudEditor.h"
#include "../NoHaveHand/Menu/ClickGui.h"
#include "../NoHaveHand/Menu/TabGui.h"
#include "../NoHaveHand/Module/ModuleManager.h"
#include "pch.h"
typedef HRESULT(__thiscall* present_t)(IDXGISwapChain3*, UINT, UINT);
present_t original_present;

typedef HRESULT(__thiscall* resize_buffers_t)(IDXGISwapChain3*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
resize_buffers_t original_resize_buffers;

// store the game's new D3D11 device2 here
ID3D11Device* device;

HRESULT present_callback(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags) {
	static bool once = false;
	if (!once) {
		// the game will fall back to D3D11 if the D3D12 device2 is dropped
		// useful for D2D but is kind of unstable
		ID3D12Device* bad_device;
		if (SUCCEEDED(swap_chain->GetDevice(IID_PPV_ARGS(&bad_device)))) {
			dynamic_cast<ID3D12Device5*>(bad_device)->RemoveDevice();
			return original_present(swap_chain, sync_interval, flags);
		}

		once = true;
	}

	// wait until we can get a D3D11 device2
	if (FAILED(swap_chain->GetDevice(IID_PPV_ARGS(&device))))
		return original_present(swap_chain, sync_interval, flags);

	// the game is now using D3D11
	// initialize renderer (this function only runs once)
	D2DUI::initRendering(swap_chain);

	// draw whatever you want now!
	D2DUI::beginRender();

	D2DUI::drawRectFilled(vec2(100, 100), vec2(100, 100), D2D1::ColorF(D2D1::ColorF::Red));
	D2DUI::drawRect(vec2(100, 100), vec2(100, 100), D2D1::ColorF(D2D1::ColorF::Black), 5.0f);

	D2DUI::drawLine(vec2(100, 100), vec2(200, 200), D2D1::ColorF(D2D1::ColorF::Black), 5.0f);

	D2DUI::setFont(L"Arial");
	D2DUI::drawText(L"Greetings planet - Floppy", vec2(100, 100), D2D1::ColorF(D2D1::ColorF::White), false, 20.0f);

	D2DUI::endRender();

	return original_present(swap_chain, sync_interval, flags);
}

HRESULT resize_buffers_callback(IDXGISwapChain3* swap_chain, UINT buffer_count, UINT width,
	UINT height, DXGI_FORMAT new_format, UINT swap_chain_flags) {
	// reinitialize renderer
	D2DUI::deinitRender();
	// init_render() will be called again in present_callback()

	return original_resize_buffers(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
}

// ImGui Shit
bool initContext = false;
enum SwapChain_DeviceType {
	INVALID_DEVICE_TYPE,
	D3D11,
	D3D12
};

typedef HRESULT(__thiscall* PresentD3D12)(IDXGISwapChain3*, UINT, UINT);
PresentD3D12 oPresentD3D12;

struct FrameContext {
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12Resource* main_render_target_resource = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE main_render_target_descriptor;
};

UINT buffersCounts = -1;
FrameContext* frameContext = nullptr;

ID3D12CommandQueue* d3d12CommandQueue = nullptr;

ID3D12DescriptorHeap* d3d12DescriptorHeapImGuiRender = nullptr;
ID3D12DescriptorHeap* d3d12DescriptorHeapBackBuffers = nullptr;
ID3D12GraphicsCommandList* d3d12CommandList = nullptr;
ID3D12CommandAllocator* allocator = nullptr;

// Devices
SwapChain_DeviceType deviceType = SwapChain_DeviceType::INVALID_DEVICE_TYPE;
ID3D11Device* d3d11Device = nullptr;
ID3D12Device* d3d12Device = nullptr;

void tryTickImGui(ModuleManager* mgr) {
	__try {
		mgr->onImGuiRender();
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {  // use a try catch to midicate any crashes and errors
	}
};

auto hookPresentD3D12(IDXGISwapChain3* ppSwapChain, UINT syncInterval, UINT flags) -> HRESULT {
	auto window = (HWND)FindWindowA(nullptr, (LPCSTR)"Minecraft");

	if (SUCCEEDED(ppSwapChain->GetDevice(IID_PPV_ARGS(&d3d11Device)))) {
		deviceType = SwapChain_DeviceType::D3D11;
	}
	else if (SUCCEEDED(ppSwapChain->GetDevice(IID_PPV_ARGS(&d3d12Device)))) {
		deviceType = SwapChain_DeviceType::D3D12;
	};

	if (deviceType == SwapChain_DeviceType::INVALID_DEVICE_TYPE)
		goto out;

	if (deviceType == SwapChain_DeviceType::D3D11) {
		if (!initContext) {
			ImGui::CreateContext();
			auto& io = ImGui::GetIO();
			const char* appData = getenv("AppData");
			std::string FontPath = appData ? std::string(appData) + "\\..\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState\\NG\\Fonts\\NotoSans.ttf" : "";
			io.Fonts->AddFontFromFileTTF(FontPath.c_str(), 16);
			io.Fonts->Build();
			initContext = true;
		};

		ID3D11DeviceContext* ppContext = nullptr;
		d3d11Device->GetImmediateContext(&ppContext);

		ID3D11Texture2D* pBackBuffer;
		ppSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		ID3D11RenderTargetView* mainRenderTargetView;
		d3d11Device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);

		pBackBuffer->Release();

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(d3d11Device, ppContext);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		tryTickImGui(moduleMgr);

		ImGui::EndFrame();
		ImGui::Render();

		ppContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		mainRenderTargetView->Release();
		d3d11Device->Release();

	}
	else if (deviceType == SwapChain_DeviceType::D3D12) {
		if (!initContext) {
			ImGui::CreateContext();
			auto& io = ImGui::GetIO();
			const char* appData = getenv("AppData");
			std::string FontPath = appData ? std::string(appData) + "\\..\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState\\NG\\Fonts\\NotoSans.ttf" : "";
			io.Fonts->AddFontFromFileTTF(FontPath.c_str(), 16);
			io.Fonts->Build();
		};

		DXGI_SWAP_CHAIN_DESC sdesc;
		ppSwapChain->GetDesc(&sdesc);
		sdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sdesc.OutputWindow = window;
		sdesc.Windowed = ((GetWindowLongPtr(window, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

		buffersCounts = sdesc.BufferCount;
		frameContext = new FrameContext[buffersCounts];

		D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
		descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorImGuiRender.NumDescriptors = buffersCounts;
		descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		if (d3d12DescriptorHeapImGuiRender == nullptr)
			if (FAILED(d3d12Device->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&d3d12DescriptorHeapImGuiRender))))
				goto out;

		if (d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)) != S_OK)
			return false;

		for (size_t i = 0; i < buffersCounts; i++) {
			frameContext[i].commandAllocator = allocator;
		};

		if (d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL, IID_PPV_ARGS(&d3d12CommandList)) != S_OK ||
			d3d12CommandList->Close() != S_OK)
			return false;

		D3D12_DESCRIPTOR_HEAP_DESC descriptorBackBuffers;
		descriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		descriptorBackBuffers.NumDescriptors = buffersCounts;
		descriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorBackBuffers.NodeMask = 1;

		if (d3d12Device->CreateDescriptorHeap(&descriptorBackBuffers, IID_PPV_ARGS(&d3d12DescriptorHeapBackBuffers)) != S_OK)
			return false;

		const auto rtvDescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12DescriptorHeapBackBuffers->GetCPUDescriptorHandleForHeapStart();

		for (size_t i = 0; i < buffersCounts; i++) {
			ID3D12Resource* pBackBuffer = nullptr;

			frameContext[i].main_render_target_descriptor = rtvHandle;
			ppSwapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&pBackBuffer));
			d3d12Device->CreateRenderTargetView(pBackBuffer, nullptr, rtvHandle);
			frameContext[i].main_render_target_resource = pBackBuffer;
			rtvHandle.ptr += rtvDescriptorSize;

			pBackBuffer->Release();
		};

		if (!initContext) {
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX12_Init(d3d12Device, buffersCounts,
				DXGI_FORMAT_R8G8B8A8_UNORM, d3d12DescriptorHeapImGuiRender,
				d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart(),
				d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart());
			initContext = true;
		};

		if (d3d12CommandQueue == nullptr)
			goto out;

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		tryTickImGui(moduleMgr);

		FrameContext& currentFrameContext = frameContext[ppSwapChain->GetCurrentBackBufferIndex()];
		currentFrameContext.commandAllocator->Reset();

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = currentFrameContext.main_render_target_resource;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		d3d12CommandList->Reset(currentFrameContext.commandAllocator, nullptr);
		d3d12CommandList->ResourceBarrier(1, &barrier);
		d3d12CommandList->OMSetRenderTargets(1, &currentFrameContext.main_render_target_descriptor, FALSE, nullptr);
		d3d12CommandList->SetDescriptorHeaps(1, &d3d12DescriptorHeapImGuiRender);

		ImGui::EndFrame();
		ImGui::Render();

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12CommandList);

		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		d3d12CommandList->ResourceBarrier(1, &barrier);
		d3d12CommandList->Close();

		d3d12CommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&d3d12CommandList));

		d3d12DescriptorHeapBackBuffers->Release();
		d3d12CommandList->Release();
		allocator->Release();

		currentFrameContext.main_render_target_resource->Release();
		currentFrameContext.commandAllocator->Release();

		d3d12Device->Release();

		delete frameContext;
	};

	goto out;

out:
	return oPresentD3D12(ppSwapChain, syncInterval, flags);
};

typedef void(__thiscall* ExecuteCommandListsD3D12)(ID3D12CommandQueue*, UINT, ID3D12CommandList*);
ExecuteCommandListsD3D12 oExecuteCommandListsD3D12;

auto hookExecuteCommandListsD3D12(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists) -> void {
	if (!d3d12CommandQueue)
		d3d12CommandQueue = queue;

	oExecuteCommandListsD3D12(queue, NumCommandLists, ppCommandLists);
};

void DXHook::InitImGui() {
	if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success) {
		kiero::bind(54, (void**)&oExecuteCommandListsD3D12, hookExecuteCommandListsD3D12);
		kiero::bind(140, (void**)&oPresentD3D12, hookPresentD3D12);
		logF("Created hook for SwapChain::Present (DX12)!");
	}
	else if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
		kiero::bind(8, (void**)&oPresentD3D12, hookPresentD3D12);
		logF("Created hook for SwapChain::Present (DX11)!");
	}
	else {
		logF("Failed to initialize hook for DX11 and DX12");
	}
}
