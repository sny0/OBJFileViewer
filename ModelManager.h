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

// Model�̊Ǘ��A�����A�`����s��
class ModelManager {
public: // �����o�ϐ�

	// Models
	vector<Model*> _models;
	string _modelPath = "";

private: // �����o�ϐ�

	// DXGI�֘A
	Microsoft::WRL::ComPtr<ID3D12Device> _dev;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _cmdList;

	// DX12�֘A
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;

	Camera* _camera;

public:
	// Model�̒ǉ�
	void AddModel(Model* m);

	// Model�̐���
	void CreateModel(string fileName, bool isMTLExist);

	// Model�̍폜
	void DeleteModel(int n);

	// �`��
	void Draw();

	// obj�t�@�C��������t�H���_�ւ�Path���Z�b�g
	void SetModelPath(string path);

	ModelManager();

	// Init
	void Init(Microsoft::WRL::ComPtr<ID3D12Device> dev, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature, Camera* camera);

	~ModelManager();

	// �e�N�X�`���̕ύX
	bool ChangeTexture(int modelId, int subModelId, string fileName);

	// Model�̕���
	void DuplicateModel(int n);

private:

	// OBJ�t�@�C���̃��[�h
	HRESULT LoadOBJFile(string name, Model* model);

	// MTL�t�@�C���̃��[�h
	HRESULT LoadMTLFile(string name, Model* model);

	// �e�N�X�`���t�@�C���̃��[�h�A�o�b�t�@�̐���
	HRESULT CreateTextureBuffers(Model* model, string fileName);

	// �e�N�X�`���t�@�C���̃��[�h�A�o�b�t�@�̐���
	HRESULT CreateTextureBuffer(SubModel* sm, string fileName);

	// ���_�o�b�t�@�[�̐���
	HRESULT CreateVertexBuffer(Model* model);

	// �C���f�b�N�X�o�b�t�@�̐���
	HRESULT CreateIndexBuffer(Model* model);

	// �}�e���A���́i�萔�j�o�b�t�@�̐���
	HRESULT CreateMaterialBuffer(Model* model);

	// Model�̕ϊ��s��̃o�b�t�@�[�̐���
	HRESULT CreateModelTransformBuffer(Model* model);

	// SubModel�̕ϊ��s��̃o�b�t�@�[�̐���
	HRESULT CreateSubModelTransformView(Model* model);

	// Mpdel��SubModel�̕ϊ��s��̃r���[�𐶐�
	HRESULT CreateModelTransformView(Model* model);

	// �e�N�X�`���̃r���[�̐���
	HRESULT CreateTextureViews(Model* model);

	// �e�N�X�`���̃r���[�̐���
	HRESULT CreateTextureView(SubModel* subModel);

	// �}�e���A���̃r���[�̐���
	HRESULT CreateMaterialView(Model* model);

	// ������̕��� string cs�ɓ����Ă��镶����ŕ��􂳂���
	void SeparateString(string str, vector<string>& vec, const string cs);

};