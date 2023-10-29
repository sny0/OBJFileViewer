
#include"OBJFVLib.h"
#include"Application.h"
#include"Model.h"
#include<Windows.h>
#include<fstream>
#include<iostream>

using namespace std;

// デバッグレイヤーを有効にする
void EnableDebugLayer() {
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
	debugLayer->EnableDebugLayer();
	debugLayer->Release();
}

// DXGIの初期化
HRESULT Application::InitializeDXGIDevice() {
	// DXGIFactoryの初期化
	UINT flagsDXGI = 0;
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG; // DXGIのエラーを表示できるようにする
	HRESULT result = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
	// 失敗したら終了
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// 使用するアダプター(=GPU)を決める
	vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;
	// PCに搭載されているアダプター(=GPU)を取得し、記録する
	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
		adapters.push_back(tmpAdapter);
	}

	// NVIDIAのアダプターを見つける
	for (auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);
		wstring strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != string::npos) {
			tmpAdapter = adpt;
			break;
		}
	}

	result = S_FALSE;
	D3D_FEATURE_LEVEL featureLevel;

	// フィーチャーレベルの列挙
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	// アダプターに適応するフィーチャーレベルを探し、デバイスを初期化
	for (auto l : levels) {
		if (SUCCEEDED(D3D12CreateDevice(tmpAdapter, l, IID_PPV_ARGS(_dev.ReleaseAndGetAddressOf())))) {
			featureLevel = l;
			result = S_OK;
			break;
		}
	}
	return result;
}

// Comand系統の初期化
HRESULT Application::InitializeCommand() {
	// コマンドアロケーターの生成
	HRESULT result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// コマンドリストの生成
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// コマンドキューの生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
	}
	return 0;
}

// スワップチェーンの生成
HRESULT Application::CreateSwapChain() {
	// スワップチェインの生成
	DXGI_SWAP_CHAIN_DESC1  swapchainDesc = {};
	swapchainDesc.Width = _mainWindow_width;
	swapchainDesc.Height = _mainWindow_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	HRESULT result = _dxgiFactory->CreateSwapChainForHwnd(_cmdQueue.Get(),
		_mainWindow->_hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)_swapchain.ReleaseAndGetAddressOf());

	return result;
}

// 最終的なレンダーターゲットの生成
HRESULT Application::CreateFinalRenderTarget() {
	// RTV用のディスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2; // ダブルバッファ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HRESULT result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_rtvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// スワップチェーンの情報を取得
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
		return result;
	}
	_backBuffers.resize(swcDesc.BufferCount); // スワップチェーンのバッファー数に合わせてバックバッファの数を変更

	// ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

	// RTVを設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	for (size_t i = 0; i < swcDesc.BufferCount; i++) {
		result = _swapchain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&_backBuffers[i])); // スワップチェーンのバックバッファのアドレスを記録
		assert(SUCCEEDED(result));
		rtvDesc.Format = _backBuffers[i]->GetDesc().Format;
		_dev->CreateRenderTargetView(_backBuffers[i], &rtvDesc, handle); // RTVを生成
		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); // RTVの大きさ分ハンドルをずらす
	}

	// ビューポートを設定
	_mainWindow->_viewPort = CD3DX12_VIEWPORT(_backBuffers[0]);

	// シザー矩形を設定
	_mainWindow->_scissorRect = CD3DX12_RECT(0, 0, _mainWindow_width, _mainWindow_height);

	return result;
}

// 初期化
bool Application::Init() {
	_modelManager = new ModelManager(); // ModelManagerインスタンスを生成
	_camera = new Camera(); // Cameraインスタンスを生成
	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);

	// 各Windowを生成
	_mainWindow = new MainWindow(_hInstance, "Main", _modelManager, _camera, _mainWindow_x, _mainWindow_y, _mainWindow_width, _mainWindow_height);
	_mainWindow->CreateNewWindow();
	_sourceWindow = new SourceWindow(_hInstance, "Source", _modelManager, _camera, _sourceWindow_x, _sourceWindow_y, _sourceWindow_widht, _sourceWindow_height);
	_sourceWindow->CreateNewWindow();
	_modelWindow = new ModelWindow(_hInstance, "Model", _modelManager, _camera, _modelWindow_x, _modelWindow_y, _modelWindow_width, _modelWindow_height);
	_modelWindow->CreateNewWindow();
	_detailWindow = new DetailWindow(_hInstance, "Detail", _modelManager, _camera, _detailWindow_x, _detailWindow_y, _detailWindow_width, _detailWindow_height);
	_detailWindow->CreateNewWindow();

	// 各Windowの初期化
	_sourceWindow->Init(_modelWindow);
	_modelWindow->Init(_detailWindow);

#ifdef _DEBUG
	// デバッグレイヤーを有効にする
	EnableDebugLayer();
#endif

	// shaderやmodelがあるフォルダへのPathを取得
	CreatePath();

	// DXGI関連の初期化
	if (FAILED(InitializeDXGIDevice())) {
		assert(0);
		return false;
	}

	// コマンド関連の初期化
	if (FAILED(InitializeCommand())) {
		assert(0);
		return false;
	}

	// スワップチェーンを生成
	if (FAILED(CreateSwapChain())) {
		assert(0);
		return false;
	}

	// 最終的に表示するRTVを設定
	if (FAILED(CreateFinalRenderTarget())) {
		assert(0);
		return false;
	}

	// フェンスの初期化
	if (FAILED(_dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf())))) {
		assert(0);
		return false;
	}

	// 深度ステンシルビューを生成
	if (FAILED(CreateDepthStencilView())) {
		assert(0);
		return false;
	}

	// ルートシグネチャを生成
	if (FAILED(CreateRootSignature())) {
		assert(0);
		return false;
	}

	// グラフィクスパイプラインを生成
	if (FAILED(CreateBasicGraphicsPipeline())) {
		assert(0);
		return false;
	}

	/*
	// 座標変換行列のビューを生成
	if (FAILED(CreateSceneTransformView())) {
		assert(0);
		return false;
	}
	*/

	// ModelManagerを初期化
	_modelManager->Init(_dev, _cmdList, _rootSignature, _camera);

	// Cameraを初期化
	_camera->Init(_mainWindow_width, _mainWindow_height, _dev, _cmdList);

	return true;
}

// Run
void Application::Run() {
	// 各Windowを表示
	ShowWindow(_mainWindow->_hwnd, SW_SHOW);
	ShowWindow(_sourceWindow->_hwnd, SW_SHOW);
	ShowWindow(_modelWindow->_hwnd, SW_SHOW);
	ShowWindow(_detailWindow->_hwnd, SW_SHOW);

	// objファイルを取得
	searchOBJFile();

	// SourceWindowにModel生成ボタンを生成
	_sourceWindow->CreateSourceButtons(_objFiles);

	// pngファイルを取得
	searchPNGFile();

	_detailWindow->SetComboBox();

	// ModelWindowにTreeViewを生成
	_modelWindow->CreateModelTree();


	auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart(); // 深度ステンシルビューのディスクリプタヒープハンドルを取得

	MSG msg = {}; // ウィンドウメッセージ

	// ループ
	while (true) {

		// ウィンドウメッセージの処理
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		// アプリケーションの終了メッセージが入力されたら終了
		if (msg.message == WM_QUIT) {
			break;
		}


		UINT bbIdx = _swapchain->GetCurrentBackBufferIndex(); // 現在のバックバッファのインデックスを取得

		// 【コマンドプッシュ】バックバッファーにバリアを設定
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		_cmdList->ResourceBarrier(1, &barrier);

		// 【コマンドプッシュ】パイプラインステイトをセット
		_cmdList->SetPipelineState(_pipelineState.Get());

		// 【コマンドプッシュ】レンダーターゲットを指定
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH = _rtvHeap->GetCPUDescriptorHandleForHeapStart(); // RTVのディスクリプターヒープのハンドルを取得
		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); // 現在のバックバッファのRTVハンドルを取得
		_cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

		// 【コマンドプッシュ】深度ステンシルビューをクリア
		_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		//【コマンドをプッシュ】画面をクリア
		float clearColor[] = { _camera->_background.x, _camera->_background.y, _camera->_background.z, 1.0f }; // RBGA
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		// 【コマンドプッシュ】ビューポートをセット
		_cmdList->RSSetViewports(1, &_mainWindow->_viewPort);

		// 【コマンドプッシュ】シザー矩形をセット
		_cmdList->RSSetScissorRects(1, &_mainWindow->_scissorRect);

		// 【コマンドプッシュ】ルートシグネチャをセット
		_cmdList->SetGraphicsRootSignature(_rootSignature.Get());

		// 【コマンドプッシュ】三角ポリゴンを使用
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// モデルの描画
		_modelManager->Draw();

		//【コマンドプッシュ】バックバッファーのバリアをPresent状態に戻す
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(_backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		_cmdList->ResourceBarrier(1, &barrier);

		// 【コマンドプッシュ】コマンド入力の終了
		_cmdList->Close();

		// コマンドの実行
		ID3D12CommandList* cmdLists[] = { _cmdList.Get() };
		_cmdQueue->ExecuteCommandLists(1, cmdLists);

		// フェンスを使用し、GPUの処理を待つ
		_cmdQueue->Signal(_fence.Get(), ++_fenceVal);
		if (_fence->GetCompletedValue() != _fenceVal) { // CPUのフェンス値とGPUのフェンス値が異なる時
			auto event = CreateEvent(nullptr, false, false, nullptr);
			_fence->SetEventOnCompletion(_fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		// コマンドリストをクリア
		_cmdAllocator->Reset();
		_cmdList->Reset(_cmdAllocator.Get(), nullptr);

		// フリップ(画面の切り替え)
		_swapchain->Present(1, 0); // 1フレームで切り替える

	}

}

// アプリの終了
void Application::Terminate() {
	//UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);
	UnregisterClass(_mainWindow->_windowClass.lpszClassName, _mainWindow->_windowClass.hInstance);
}

// Applicationインスタンスの生成
Application& Application::Instance(HINSTANCE hi) {
	static Application instance;
	instance._hInstance = hi;
	return instance;
}

Application::~Application() {

}

// 深度ステンシルビューの生成
HRESULT Application::CreateDepthStencilView() {
	auto depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		_mainWindow_width,
		_mainWindow_height,
		1,
		1,
		1,
		0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	auto depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);


	CD3DX12_CLEAR_VALUE depthClearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	CD3DX12_CLEAR_VALUE rtClearValue(DXGI_FORMAT_R8G8B8A8_UINT, clearColor);

	HRESULT result = _dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf())
	);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	result = _dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.ReleaseAndGetAddressOf()));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	_dev->CreateDepthStencilView(_depthBuffer.Get(), &dsvDesc, _dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}



// 深度ステンシルビューの生成
HRESULT Application::CreateSceneTransformView() {
	_worldMat = XMMatrixIdentity();

	//XMFLOAT3 eye(0, 1, -3);
	//XMFLOAT3 target(0, 1, 0);
	//XMFLOAT3 up(0, 1, 0);

	_cameraTrans.position = XMFLOAT3(0.0f, 0.0f, -5.0f);
	_cameraTrans.rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMMATRIX viewRotMat = XMMatrixRotationRollPitchYaw(_cameraTrans.rotation.x, _cameraTrans.rotation.y, _cameraTrans.rotation.z);
	XMMATRIX viewTraMat = XMMatrixTranslation(_cameraTrans.position.x, _cameraTrans.position.y, _cameraTrans.position.z);

	_viewMat = XMMatrixInverse(nullptr, viewRotMat * viewTraMat);
	//_viewMat = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	_projMat = XMMatrixPerspectiveFovLH(XM_PIDIV2,
		static_cast<float>(_mainWindow_width) / static_cast<float>(_mainWindow_height),
		1.0f,
		1000.0f
	);

	auto heapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + 0xff) & ~0xff);
	HRESULT result = _dev->CreateCommittedResource(
		&heapPro,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_constBuffer.ReleaseAndGetAddressOf())
	);

	if (FAILED(result)) {
		assert(0);
		return result;
	}


	result = _constBuffer->Map(0, nullptr, (void**)&_sceneMap);

	if (FAILED(result)) {
		assert(0);
		return result;
	}

	_sceneMap->world = _worldMat;
	_sceneMap->view = _viewMat;
	_sceneMap->proj = _projMat;
	_sceneMap->eye = _cameraTrans.position;


	// ディスクリプターヒープ
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NodeMask = 0;
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(_basicHeap.ReleaseAndGetAddressOf()));

	auto basicHeapHandle = _basicHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(_constBuffer->GetDesc().Width);

	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return result;
}

// ルートシグネチャの生成
HRESULT Application::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descTblRanges[5] = {};
	// カメラ(変換行列)の情報
	descTblRanges[0].NumDescriptors = 1;
	descTblRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblRanges[0].BaseShaderRegister = 0;
	descTblRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ModelのSRT情報
	descTblRanges[1].NumDescriptors = 1;
	descTblRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblRanges[1].BaseShaderRegister = 1;
	descTblRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// SubModelのSRT情報
	descTblRanges[2].NumDescriptors = 1;
	descTblRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblRanges[2].BaseShaderRegister = 2;
	descTblRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// SubModelのMaterial情報
	descTblRanges[3].NumDescriptors = 1;
	descTblRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descTblRanges[3].BaseShaderRegister = 3;
	descTblRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// SubModelのTexture情報
	descTblRanges[4].NumDescriptors = 1;
	descTblRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descTblRanges[4].BaseShaderRegister = 0;
	descTblRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootparam[3] = {};
	rootparam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam[0].DescriptorTable.pDescriptorRanges = &descTblRanges[0];
	rootparam[0].DescriptorTable.NumDescriptorRanges = 1;
	rootparam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootparam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam[1].DescriptorTable.pDescriptorRanges = &descTblRanges[1];
	rootparam[1].DescriptorTable.NumDescriptorRanges = 1;
	rootparam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootparam[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam[2].DescriptorTable.pDescriptorRanges = &descTblRanges[2];
	rootparam[2].DescriptorTable.NumDescriptorRanges = 3;
	rootparam[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootparam;
	rootSignatureDesc.NumParameters = 3;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	Microsoft::WRL::ComPtr<ID3DBlob> rootSigBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob
	);

	if (FAILED(result)) {
		assert(0);
		return result;
	}

	result = _dev->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.ReleaseAndGetAddressOf())
	);

	if (FAILED(result)) {
		assert(0);
		return result;
	}
	return result;

}

// グラフィクスパイプラインの生成
HRESULT Application::CreateBasicGraphicsPipeline() {
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

	string vsShaderPath = _shaderPath + "\\BasicVS.hlsl";
	std::cout << vsShaderPath << endl;
	wstring wvsShaderPath(vsShaderPath.begin(), vsShaderPath.end());
	HRESULT result = D3DCompileFromFile(wvsShaderPath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob,
		&errorBlob
	);

	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("頂点シェーダファイルが見つかりません。");
		}
		else {
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
			errstr += "\n";
			OutputDebugStringA(errstr.c_str());
		}
		return result;
	}

	string psShaderPath = _shaderPath + "\\BasicPS.hlsl";
	std::cout << psShaderPath << endl;
	wstring wpsShaderPath(psShaderPath.begin(), psShaderPath.end());

	result = D3DCompileFromFile(wpsShaderPath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		&errorBlob
	);

	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("ピクセルシェーダファイルが見つかりません。");
		}
		else {
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
			errstr += "\n";
			OutputDebugStringA(errstr.c_str());
		}
		return result;
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.pRootSignature = _rootSignature.Get();
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;
	D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
		false, false,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL
	};

	for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++) {
		gpipeline.BlendState.RenderTarget[i] = defaultRenderTargetBlendDesc;
	}

	gpipeline.BlendState.RenderTarget[0].BlendEnable = true;
	gpipeline.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	gpipeline.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングなし

	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipeline.DepthStencilState.StencilEnable = false;

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	gpipeline.SampleDesc.Count = 1;
	gpipeline.SampleDesc.Quality = 0;

	result = _dev->CreateGraphicsPipelineState(&gpipeline,
		IID_PPV_ARGS(_pipelineState.ReleaseAndGetAddressOf())
	);

	if (FAILED(result)) {
		assert(0);
	}
	return result;
}

// shaderがあるフォルダへのPathを取得 -> _shaderPath, _shaderPathD
// modelがあるフォルダへのPathを取得 -> _modelPath, _modelPathD
void Application::CreatePath() {
	char currentDirectoryPath[10000];

	GetCurrentDirectory(10000, currentDirectoryPath);

	_shaderPath = currentDirectoryPath;
	_modelPath = string(currentDirectoryPath) + "\\model";

	string currentDirectoryPathS = string(currentDirectoryPath);
	size_t found = currentDirectoryPathS.rfind("x64");
	if (found != string::npos) {
		auto folderPath = currentDirectoryPathS.substr(0, found - 1);
		_shaderPath = folderPath;
		_modelPath = folderPath + "\\model";

	}

	return;
}

// フォルダ内にあるobjファイルやmtlファイルを取得 -> _objFiles
void Application::searchOBJFile() {
	CreatePath();
	_modelManager->SetModelPath(_modelPath);
	_objFiles.clear();

	WIN32_FIND_DATA findFileData;
	string searchFile = _modelPath + "\\*.obj";
	std::cout << "OBJファイルを検索します。パス：" << _modelPath << endl;
	HANDLE hFind = FindFirstFile(searchFile.c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		std::cout << "OBJファイルが見つかりませんでした。" << endl;
		return;
	}

	do {
		string fileName = findFileData.cFileName;
		size_t dotPos = fileName.find_last_of(".");

		if (dotPos != string::npos) {
			string nameWithoutExt = fileName.substr(0, dotPos);
			_objFiles.insert(make_pair(nameWithoutExt, false));
			std::cout << "OBJファイルを見つけました：" << fileName << endl;
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	std::cout << "mtl" << endl;
	searchFile = _modelPath + "\\*.mtl";
	hFind = FindFirstFile(searchFile.c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		std::cout << "MTLファイルが見つかりませんでした。" << endl;
		return;
	}
	do {
		string fileName = findFileData.cFileName;
		size_t dotPos = fileName.find_last_of(".");

		if (dotPos != string::npos) {
			string nameWithoutExt = fileName.substr(0, dotPos);
			auto it = _objFiles.find(nameWithoutExt);
			if (it != _objFiles.end()) {
				std::cout << it->first << "のMTLファイルを見つけました。" << endl;
				it->second = true;
			}
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	return;
}

// フォルダ内にあるpngファイルを取得 -> _pngFiles
void Application::searchPNGFile() {
	CreatePath();
	WIN32_FIND_DATA findFileData;
	string searchFile = _modelPath + "\\*.png";
	std::cout << "PNGファイルを検索します。パス：" << _modelPath << endl;
	HANDLE hFind = FindFirstFile(searchFile.c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		std::cout << "PNGファイルが見つかりませんでした。" << endl;
		return;
	}

	do {
		string fileName = findFileData.cFileName;
		_pngFiles.push_back(fileName);
		std::cout << fileName << "を見つけました。" << endl;

	} while (FindNextFile(hFind, &findFileData) != 0);

	_detailWindow->_pngFiles = _pngFiles;
}
