#pragma once
#include"OBJFVLib.h"
#include"Model.h"
#include"Camera.h"
#include<Windows.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<ostream>
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

#define DEFAULT_TEX "white.png"

// Modelの管理、生成、描画を行う
class ModelManager {
public: // メンバ変数

	// Models
	vector<Model*> _models;
	string _modelPath = "";

private: // メンバ変数

	// DXGI関連
	Microsoft::WRL::ComPtr<ID3D12Device> _dev;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _cmdList;

	// DX12関連
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;

	Camera* _camera;

public:
	// Modelの追加
	void AddModel(Model* m);

	// Modelの生成
	void CreateModel(string fileName, bool isMTLExist);

	// Modelの削除
	void DeleteModel(int n);

	// 描画
	void Draw();

	// objファイルがあるフォルダへのPathをセット
	void SetModelPath(string path);

	ModelManager();

	// Init
	void Init(Microsoft::WRL::ComPtr<ID3D12Device> dev, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature, Camera* camera);

	~ModelManager();

	// テクスチャの変更
	bool ChangeTexture(int modelId, int subModelId, string fileName);

	// Modelの複製
	void DuplicateModel(int n);

private:

	// OBJファイルのロード
	HRESULT LoadOBJFile(string name, Model* model);

	// MTLファイルのロード
	HRESULT LoadMTLFile(string name, Model* model);

	// テクスチャファイルのロード、バッファの生成
	HRESULT CreateTextureBuffers(Model* model, string fileName);

	// テクスチャファイルのロード、バッファの生成
	HRESULT CreateTextureBuffer(SubModel* sm, string fileName);

	// 頂点バッファーの生成
	HRESULT CreateVertexBuffer(Model* model);

	// インデックスバッファの生成
	HRESULT CreateIndexBuffer(Model* model);

	// マテリアルの（定数）バッファの生成
	HRESULT CreateMaterialBuffer(Model* model);

	// Modelの変換行列のバッファーの生成
	HRESULT CreateModelTransformBuffer(Model* model);

	// SubModelの変換行列のバッファーの生成
	HRESULT CreateSubModelTransformView(Model* model);

	// MpdelとSubModelの変換行列のビューを生成
	HRESULT CreateModelTransformView(Model* model);

	// テクスチャのビューの生成
	HRESULT CreateTextureViews(Model* model);

	// テクスチャのビューの生成
	HRESULT CreateTextureView(SubModel* subModel);

	// マテリアルのビューの生成
	HRESULT CreateMaterialView(Model* model);

	// 文字列の分裂 string csに入っている文字列で分裂させる
	void SeparateString(string str, vector<string>& vec, const string cs);

};