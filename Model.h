#pragma once
#include"OBJFVLib.h"
#include"SubModel.h"
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

class Model
{
public:
	string _OBJFileName = "";
	string _MTLFileName = "";
	bool _isMTLExist;
	Transform _transform;
	SRTMatrices* _srt = nullptr;

	int _subModelNum = 0;

	vector<SubModel*> _subModels;

public:
	string _name;

	vector<Vertex> _vertices;
	Microsoft::WRL::ComPtr<ID3D12Resource> _vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};

	unsigned int _materialNum = 0;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _heap = nullptr; // _basicBufferのViewを保持する
	Microsoft::WRL::ComPtr<ID3D12Resource> _basicBuffer = nullptr; // モデルの座標、回転、拡大を保持

	string _mtlPath;
	Microsoft::WRL::ComPtr<ID3D12Resource> _materialBuffer = nullptr;
	D3D12_CONSTANT_BUFFER_VIEW_DESC _cbvDesc = {};



	Model(string fileName, bool exist);
	~Model();

	void Update();

	void AddSubModel(SubModel* sm);

};
