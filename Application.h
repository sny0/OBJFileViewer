#pragma once
#include"OBJFVLib.h"
#include"SourceWindow.h"
#include"MainWindow.h"
#include"ModelWindow.h"
#include"DetailWindow.h"
#include"ModelManager.h"
#include"Model.h"
#include<Windows.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<d3dcompiler.h>
#include<DirectXTex.h>
#include<d3dx12.h>
#include<wrl.h>

#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace std;
using namespace DirectX;


// メンバ変数名：_〇〇
class Application
{
private: // メンバ変数

	HINSTANCE _hInstance; // hInstance

	// MainWindowの位置, 大きさ
	unsigned int _mainWindow_x = 300;
	unsigned int _mainWindow_y = 300;
	unsigned int _mainWindow_width = 1280;
	unsigned int _mainWindow_height = 720;

	// SourceWindowの位置, 大きさ
	unsigned int _sourceWindow_x = 0;
	unsigned int _sourceWindow_y = 0;
	unsigned int _sourceWindow_widht = 1580;
	unsigned int _sourceWindow_height = 300;

	// ModelWindowの位置, 大きさ
	unsigned int _modelWindow_x = 0;
	unsigned int _modelWindow_y = 300;
	unsigned int _modelWindow_width = 300;
	unsigned int _modelWindow_height = 720;

	// DetailWindowの位置, 大きさ
	unsigned int _detailWindow_x = 1580;
	unsigned int _detailWindow_y = 0;
	unsigned int _detailWindow_width = 300;
	unsigned int _detailWindow_height = 1020;

	// 各Windowへのポインタ
	MainWindow* _mainWindow = nullptr;
	SourceWindow* _sourceWindow = nullptr;
	ModelWindow* _modelWindow = nullptr;
	DetailWindow* _detailWindow = nullptr;


	// ModelManager
	ModelManager* _modelManager;

	// Camera
	Camera* _camera;

	map<string, bool> _objFiles; // フォルダ内のobjファイル名を格納しておく, 引数：(OBJ FileName, Is MTL File exist)
	vector<string> _pngFiles; // フォルダ内のpngファイル名を格納しておく


	// DXGI関連
	Microsoft::WRL::ComPtr<IDXGIFactory6> _dxgiFactory = nullptr; // DXGIの管理
	Microsoft::WRL::ComPtr<IDXGISwapChain4> _swapchain = nullptr; // スワップチェイン

	// DX12関連
	Microsoft::WRL::ComPtr<ID3D12Device> _dev = nullptr; // デバイス(=使用するGPUの情報を格納)
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _cmdAllocator = nullptr; // コマンドアロケータ(=コマンドをためておく場所)
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _cmdList = nullptr; // コマンドリスト(=コマンドを実行するときに参照するインターフェース)
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr; // コマンドキュー(=コマンドの管理)


	// DescriptorHeaps
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvHeap = nullptr; // レンダーテクスチャビューのディスクリプターヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _dsvHeap = nullptr; // 深度ステンシルビューのディスクリプターヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _basicHeap = nullptr; // 座標変換のビューのディスクリプターヒープ

	// BackBuffer
	vector<ID3D12Resource*> _backBuffers;

	// Fence(=CPUとGPUの同期に利用)
	Microsoft::WRL::ComPtr<ID3D12Fence> _fence = nullptr;

	// Fence値
	UINT64 _fenceVal = 0;

	// PipelineState
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineState = nullptr;

	// RootSignature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature = nullptr;

	Transform _cameraTrans;

	// 座標変換行列
	XMMATRIX _worldMat; // ワールド行列
	XMMATRIX _viewMat; // ビュー行列
	XMMATRIX _projMat; // プロジェクション行列

	// Buffers
	Microsoft::WRL::ComPtr<ID3D12Resource> _depthBuffer = nullptr; // 深度情報を格納するバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> _constBuffer = nullptr; // 変換行列を格納するバッファ

	SceneData* _sceneMap = nullptr; // マップされた_constBufferへのアドレスを保持

	// shaderやmodelファイルがあるフォルダへのPath
	string _shaderPath = "";
	string _modelPath = "";

	XMFLOAT3 _backgroundColor;

private: // メンバ関数

	// DXGI関連(Factory, D3D12Device)の初期化
	HRESULT InitializeDXGIDevice();

	// コマンド関連(commandAllocator, commandList, commandQueue)の初期化
	HRESULT InitializeCommand();

	// スワップチェインの生成
	HRESULT CreateSwapChain();

	// 画面に表示するRTを生成
	HRESULT CreateFinalRenderTarget();

	// 深度ステンシルビューの生成
	HRESULT CreateDepthStencilView();

	// 座標変換のビューの生成
	HRESULT CreateSceneTransformView();

	// ルートシグネチャの生成
	HRESULT CreateRootSignature();

	// グラフィクスパイプラインの生成
	HRESULT CreateBasicGraphicsPipeline();

	// shaderやmodelがあるフォルダへのPathを取得し、_shaderPathや_modelPathに格納
	void CreatePath();

	// フォルダ内のobjファイルやmtlファイルを取得
	void searchOBJFile();

	// フォルダ内のpngファイルを取得
	void searchPNGFile();


public: // メンバ関数

	// staticなApplicationインスタンスを作成
	static Application& Instance(HINSTANCE hi);

	// 初期化
	bool Init();

	// Run
	void Run();

	// 終了
	void Terminate();

	// デストラクタ
	~Application();
};