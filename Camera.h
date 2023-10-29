#pragma once
#include"OBJFVLib.h"
#include<Windows.h>
#include<commctrl.h>
#include<tchar.h>
#include<d3d12.h>
#include<iostream>
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

class Camera {
public:
	Camera();

	void Init(int w, int h, Microsoft::WRL::ComPtr<ID3D12Device> dev, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);

	void Update();

	~Camera();

	HRESULT CreateCameraBuffer();

	HRESULT CreateCameraView();

	Transform _transform;

	SceneData _sceneData;

	XMFLOAT3 _background = { 0.0f, 0.5f, 0.5f };

	bool _isHorizontal;

	float _fov = 90.0f;

	XMFLOAT2 _clippingPlanes = { 0.1f, 1000.0f };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _basicHeap = nullptr;

private:
	int _mainWindow_width;
	int _mainWindow_height;

	Microsoft::WRL::ComPtr<ID3D12Resource> _constBuffer = nullptr; // 変換行列を格納するバッファ

	Microsoft::WRL::ComPtr<ID3D12Device> _dev = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _cmdList = nullptr;

	SceneData* _sceneMap = nullptr; // マップされた_constBufferへのアドレスを保持
};