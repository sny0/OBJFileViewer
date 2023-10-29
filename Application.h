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


// �����o�ϐ����F_�Z�Z
class Application
{
private: // �����o�ϐ�

	HINSTANCE _hInstance; // hInstance

	// MainWindow�̈ʒu, �傫��
	unsigned int _mainWindow_x = 300;
	unsigned int _mainWindow_y = 300;
	unsigned int _mainWindow_width = 1280;
	unsigned int _mainWindow_height = 720;

	// SourceWindow�̈ʒu, �傫��
	unsigned int _sourceWindow_x = 0;
	unsigned int _sourceWindow_y = 0;
	unsigned int _sourceWindow_widht = 1580;
	unsigned int _sourceWindow_height = 300;

	// ModelWindow�̈ʒu, �傫��
	unsigned int _modelWindow_x = 0;
	unsigned int _modelWindow_y = 300;
	unsigned int _modelWindow_width = 300;
	unsigned int _modelWindow_height = 720;

	// DetailWindow�̈ʒu, �傫��
	unsigned int _detailWindow_x = 1580;
	unsigned int _detailWindow_y = 0;
	unsigned int _detailWindow_width = 300;
	unsigned int _detailWindow_height = 1020;

	// �eWindow�ւ̃|�C���^
	MainWindow* _mainWindow = nullptr;
	SourceWindow* _sourceWindow = nullptr;
	ModelWindow* _modelWindow = nullptr;
	DetailWindow* _detailWindow = nullptr;


	// ModelManager
	ModelManager* _modelManager;

	// Camera
	Camera* _camera;

	map<string, bool> _objFiles; // �t�H���_����obj�t�@�C�������i�[���Ă���, �����F(OBJ FileName, Is MTL File exist)
	vector<string> _pngFiles; // �t�H���_����png�t�@�C�������i�[���Ă���


	// DXGI�֘A
	Microsoft::WRL::ComPtr<IDXGIFactory6> _dxgiFactory = nullptr; // DXGI�̊Ǘ�
	Microsoft::WRL::ComPtr<IDXGISwapChain4> _swapchain = nullptr; // �X���b�v�`�F�C��

	// DX12�֘A
	Microsoft::WRL::ComPtr<ID3D12Device> _dev = nullptr; // �f�o�C�X(=�g�p����GPU�̏����i�[)
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _cmdAllocator = nullptr; // �R�}���h�A���P�[�^(=�R�}���h�����߂Ă����ꏊ)
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _cmdList = nullptr; // �R�}���h���X�g(=�R�}���h�����s����Ƃ��ɎQ�Ƃ���C���^�[�t�F�[�X)
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr; // �R�}���h�L���[(=�R�}���h�̊Ǘ�)


	// DescriptorHeaps
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvHeap = nullptr; // �����_�[�e�N�X�`���r���[�̃f�B�X�N���v�^�[�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _dsvHeap = nullptr; // �[�x�X�e���V���r���[�̃f�B�X�N���v�^�[�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _basicHeap = nullptr; // ���W�ϊ��̃r���[�̃f�B�X�N���v�^�[�q�[�v

	// BackBuffer
	vector<ID3D12Resource*> _backBuffers;

	// Fence(=CPU��GPU�̓����ɗ��p)
	Microsoft::WRL::ComPtr<ID3D12Fence> _fence = nullptr;

	// Fence�l
	UINT64 _fenceVal = 0;

	// PipelineState
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineState = nullptr;

	// RootSignature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature = nullptr;

	Transform _cameraTrans;

	// ���W�ϊ��s��
	XMMATRIX _worldMat; // ���[���h�s��
	XMMATRIX _viewMat; // �r���[�s��
	XMMATRIX _projMat; // �v���W�F�N�V�����s��

	// Buffers
	Microsoft::WRL::ComPtr<ID3D12Resource> _depthBuffer = nullptr; // �[�x�����i�[����o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> _constBuffer = nullptr; // �ϊ��s����i�[����o�b�t�@

	SceneData* _sceneMap = nullptr; // �}�b�v���ꂽ_constBuffer�ւ̃A�h���X��ێ�

	// shader��model�t�@�C��������t�H���_�ւ�Path
	string _shaderPath = "";
	string _modelPath = "";

	XMFLOAT3 _backgroundColor;

private: // �����o�֐�

	// DXGI�֘A(Factory, D3D12Device)�̏�����
	HRESULT InitializeDXGIDevice();

	// �R�}���h�֘A(commandAllocator, commandList, commandQueue)�̏�����
	HRESULT InitializeCommand();

	// �X���b�v�`�F�C���̐���
	HRESULT CreateSwapChain();

	// ��ʂɕ\������RT�𐶐�
	HRESULT CreateFinalRenderTarget();

	// �[�x�X�e���V���r���[�̐���
	HRESULT CreateDepthStencilView();

	// ���W�ϊ��̃r���[�̐���
	HRESULT CreateSceneTransformView();

	// ���[�g�V�O�l�`���̐���
	HRESULT CreateRootSignature();

	// �O���t�B�N�X�p�C�v���C���̐���
	HRESULT CreateBasicGraphicsPipeline();

	// shader��model������t�H���_�ւ�Path���擾���A_shaderPath��_modelPath�Ɋi�[
	void CreatePath();

	// �t�H���_����obj�t�@�C����mtl�t�@�C�����擾
	void searchOBJFile();

	// �t�H���_����png�t�@�C�����擾
	void searchPNGFile();


public: // �����o�֐�

	// static��Application�C���X�^���X���쐬
	static Application& Instance(HINSTANCE hi);

	// ������
	bool Init();

	// Run
	void Run();

	// �I��
	void Terminate();

	// �f�X�g���N�^
	~Application();
};