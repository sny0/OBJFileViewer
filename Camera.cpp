#include "Camera.h"

Camera::Camera() {}

Camera::~Camera() {}

// Init
void Camera::Init(int w, int h, Microsoft::WRL::ComPtr<ID3D12Device> dev, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList) {
	_mainWindow_width = w;
	_mainWindow_height = h;
	_dev = dev;
	_cmdList = cmdList;

	CreateCameraBuffer();
	CreateCameraView();
}

// Cameraの情報を格納するバッファーを生成する関数
HRESULT Camera::CreateCameraBuffer() {
	_transform.position = { 0.0f, 0.0f, -5.0f };

	_sceneData.ligVec = { 1.0f, -1.0f, 1.0f };
	_sceneData.ligCol = { 1.0f, 1.0f, 1.0f };

	XMMATRIX viewRotMat = XMMatrixRotationRollPitchYaw(
		_transform.rotation.x * XM_PI / 180.0f,
		_transform.rotation.y * XM_PI / 180.0f,
		_transform.rotation.z * XM_PI / 180.0f);
	XMMATRIX viewTraMat = XMMatrixTranslation(_transform.position.x, _transform.position.y, _transform.position.z);

	_sceneData.world = XMMatrixIdentity();
	_sceneData.view = XMMatrixInverse(nullptr, viewRotMat * viewTraMat);
	//_sceneData.view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	_sceneData.proj = XMMatrixPerspectiveFovLH(_fov * XM_PI / 180.0f,
		static_cast<float>(_mainWindow_width) / static_cast<float>(_mainWindow_height),
		_clippingPlanes.x,
		_clippingPlanes.y
	);
	_sceneData.eye = _transform.position;
	//_sceneData.eye = eye;

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

	*_sceneMap = _sceneData;
}

// Cameraの情報を格納するバッファーのビューを生成する関数
HRESULT Camera::CreateCameraView() {
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NodeMask = 0;
	descHeapDesc.NumDescriptors = 1;
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	HRESULT result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(_basicHeap.ReleaseAndGetAddressOf()));

	if (FAILED(result)) {
		assert(0);
		return result;
	}

	auto basicHeapHandle = _basicHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _constBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(_constBuffer->GetDesc().Width);

	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	return result;
}

// Update
void Camera::Update() {
	XMMATRIX viewRotMat = XMMatrixRotationRollPitchYaw(
		_transform.rotation.x * XM_PI / 180.0f,
		_transform.rotation.y * XM_PI / 180.0f,
		_transform.rotation.z * XM_PI / 180.0f);
	XMMATRIX viewTraMat = XMMatrixTranslation(_transform.position.x, _transform.position.y, _transform.position.z);
	_sceneData.view = XMMatrixInverse(nullptr, viewRotMat * viewTraMat);
	//_viewMat = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	_sceneData.proj = XMMatrixPerspectiveFovLH(_fov * XM_PI / 180.0f,
		static_cast<float>(_mainWindow_width) / static_cast<float>(_mainWindow_height),
		_clippingPlanes.x,
		_clippingPlanes.y
	);
	_sceneData.eye = _transform.position;
	*_sceneMap = _sceneData;
}
