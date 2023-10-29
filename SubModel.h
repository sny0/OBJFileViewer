#pragma once
#include"OBJFVLib.h"
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

class SubModel {
public:
	string _materialName = "";
	Transform _transform;
	SRTMatrices* _srt;
	Material _material;
	Material* _pMaterial = &_material;
	string _textureFileName = "";

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _heap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC _subModelDescHeapDesc = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> _basicBuffer = nullptr; // モデルの座標、回転、拡大を保持

	Microsoft::WRL::ComPtr<ID3D12Resource> _materialBuffer = nullptr;
	D3D12_CONSTANT_BUFFER_VIEW_DESC _cbvDesc = {};

	vector<unsigned int> _vertexIndices;
	Microsoft::WRL::ComPtr<ID3D12Resource> _indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView = {};

	string _textureName;
	TexMetadata _metadata = {};
	ScratchImage _scratchImage = {};
	Microsoft::WRL::ComPtr<ID3D12Resource> _textureBuffer = nullptr;

	string _materialID;

public:

	void Update();

	SubModel();

	~SubModel();
};