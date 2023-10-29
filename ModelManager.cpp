#include "ModelManager.h"
#include<fstream>
#include<iostream>

ModelManager::ModelManager() {}

// 初期化
void ModelManager::Init(Microsoft::WRL::ComPtr<ID3D12Device> dev, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature, Camera* camera) {
	_models.clear();
	_dev = dev;
	_cmdList = cmdList;
	_camera = camera;
	_rootSignature = rootSignature;
}

ModelManager::~ModelManager() {

}

// objファイルがあるフォルダへのPathをセット
void ModelManager::SetModelPath(string path) {
	_modelPath = path;
}

// Modelを追加
//@param Model* m : 追加したいModelポインタ
void ModelManager::AddModel(Model* m) {
	_models.push_back(m);
}

// Modelを削除
//@param int n： 削除したいModelインデックス
void ModelManager::DeleteModel(int n) {
	if (n >= 0 && n < _models.size()) {
		for (auto sm : _models[n]->_subModels) {
			delete sm;
		}
		delete _models[n];

		_models.erase(_models.begin() + n);
	}
}

// Modelを描画
void ModelManager::Draw() {
	for (auto m : _models) {
		m->Update();
		for (auto sm : m->_subModels) {
			_cmdList->IASetVertexBuffers(0, 1, &m->_vertexBufferView);
			_cmdList->IASetIndexBuffer(&sm->_indexBufferView);
			_cmdList->SetGraphicsRootSignature(_rootSignature.Get());

			ID3D12DescriptorHeap* bdh[] = { _camera->_basicHeap.Get() };
			_cmdList->SetDescriptorHeaps(1, bdh);
			_cmdList->SetGraphicsRootDescriptorTable(0, _camera->_basicHeap->GetGPUDescriptorHandleForHeapStart());

			ID3D12DescriptorHeap* mdh[] = { m->_heap.Get() };
			_cmdList->SetDescriptorHeaps(1, mdh);
			_cmdList->SetGraphicsRootDescriptorTable(1, m->_heap->GetGPUDescriptorHandleForHeapStart());

			ID3D12DescriptorHeap* smdh[] = { sm->_heap.Get() };
			_cmdList->SetDescriptorHeaps(1, smdh);
			_cmdList->SetGraphicsRootDescriptorTable(2, sm->_heap->GetGPUDescriptorHandleForHeapStart());

			_cmdList->DrawIndexedInstanced(sm->_vertexIndices.size(), 1, 0, 0, 0);
		}
	}
}

// objファイルやmtlファイルを読み取り、Modelを作成。追加する
//@param string fileName : objファイルの名前
//@param bool isMTLExist : mtlファイルが存在しているか
void ModelManager::CreateModel(string fileName, bool isMTLExist) {
	Model* model = new Model(fileName, isMTLExist);
	model->_name = fileName;

	LoadOBJFile(fileName.c_str(), model);

	if (isMTLExist) LoadMTLFile(fileName.c_str(), model);

	CreateTextureBuffers(model, DEFAULT_TEX);

	CreateVertexBuffer(model);

	CreateIndexBuffer(model);

	CreateModelTransformBuffer(model);

	CreateMaterialBuffer(model);

	CreateModelTransformView(model);

	CreateSubModelTransformView(model);

	CreateMaterialView(model);

	CreateTextureViews(model);

	AddModel(model);
}

// OBJファイルのロード、パース
//@param string name : objファイル名
//@param Model* model : 情報を格納するModelのポインタ
HRESULT ModelManager::LoadOBJFile(string name, Model* model) {

	std::cout << "start loadOBJFileMethod" << std::endl;
	string modelFilePath = _modelPath + "\\" + name + ".obj";
	ifstream ifs(modelFilePath);

	if (!ifs) {
		std::cout << "Don't Open File.";
		assert(0);
		return E_ABORT;
	}

	std::cout << "Open file." << std::endl;

	string objFileName;
	string mtlFileName;
	vector<string> memo; // コメント（objファイルで # で書かれているもの）
	vector<XMFLOAT3> v; // 頂点座標
	vector<XMFLOAT2> vt; // uv座標
	vector<XMFLOAT3> vn; // 法線ベクトル
	vector<vector<vector<XMINT3>>> fs; // インデックス情報
	vector<string> mtlNames; // マテリアル名

	// objファイルの読み取り
	string line;
	std::cout << "start read file" << std::endl;
	int mtlNum = -1;
	vector<vector<XMINT3>> tmpf;
	while (getline(ifs, line)) { // 1行ずつ読み取る

		if (line == "") {

		}
		else if (line[0] == '#') {
			memo.push_back(line.substr(1, string::npos));
		}
		else {
			vector<string> strs;
			strs.clear();
			SeparateString(line, strs, " ");

			if (strs[0] == "v") {
				XMFLOAT3 tmpv = { stof(strs[1]), stof(strs[2]), stof(strs[3]) };
				v.push_back(tmpv);
			}
			else if (strs[0] == "vt") {
				XMFLOAT2 tmpvt = { stof(strs[1]), stof(strs[2]) };
				vt.push_back(tmpvt);
			}
			else if (strs[0] == "vn") {
				XMFLOAT3 tmpvn = { stof(strs[1]), stof(strs[2]), stof(strs[3]) };
				vn.push_back(tmpvn);
			}
			else if (strs[0] == "f") {
				vector<string> strs1;
				vector<XMINT3> tmpff;
				for (int i = 0; i < strs.size() - 1; i++) {
					strs1.clear();
					SeparateString(strs[i + 1], strs1, "/");

					for (int j = 0; j < 3; j++) {
						if (strs1[j] == "") {
							strs1[j] = "0";
						}
					}

					XMINT3 tmpvd = { stoi(strs1[0]), stoi(strs1[1]), stoi(strs1[2]) };
					tmpff.push_back(tmpvd);
				}
				tmpf.push_back(tmpff);

			}
			else if (strs[0] == "mtllib") {
				mtlFileName = strs[1];
			}
			else if (strs[0] == "usemtl") {
				string tmpMtlName = strs[1];
				mtlNum++;
				mtlNames.push_back(tmpMtlName);

				if (mtlNum > 0) {
					fs.push_back(tmpf);
					tmpf.clear();
				}

			}
			else {

			}
		}
	}
	if (!tmpf.empty()) {
		fs.push_back(tmpf);
		tmpf.clear();
	}

	std::cout << "finish read file" << endl;

	ifs.close(); // ファイルを閉じる


	std::cout << "close file" << endl;

	std::cout << "make VertexStrct" << endl;

	int offset = 0;

	// 読み取った数値の確認
	for (int i = 0; i < fs.size(); i++) {
		std::cout << "fs:" << i << std::endl;
		for (int j = 0; j < fs[i].size(); j++) {
			std::cout << "fs[" << i << "]:" << j << std::endl;
			for (int k = 0; k < fs[i][j].size(); k++) {
				std::cout << fs[i][j][k].x << ", " << fs[i][j][k].y << ", " << fs[i][j][k].z << std::endl;
			}
		}
	}

	// 読み取った情報を基に、DXで扱える形に変換
	for (int i = 0; i < fs.size(); i++) {
		// 頂点
		for (int j = 0; j < fs[i].size(); j++) {
			for (int l = 0; l < fs[i][j].size(); l++) {
				Vertex tmpVert = {};

				tmpVert.position = v[fs[i][j][l].x - 1];
				tmpVert.uv = { vt[fs[i][j][l].y - 1].x, 1 - vt[fs[i][j][l].y - 1].y };
				tmpVert.normal = vn[fs[i][j][l].z - 1];
				model->_vertices.push_back(tmpVert);
				std::cout << "i:" << i << "  j:" << j << "  vertex:" << tmpVert.position.x << " " << tmpVert.position.y << " " << tmpVert.position.z << endl;
			}
		}

		// 設定されているマテリアルの数だけSubModelを生成
		SubModel* subModel = new SubModel();
		if (mtlNames.size() == 0) subModel->_materialName = "Default Material"; // マテリアルが設定されていなかった場合
		else subModel->_materialName = mtlNames[i];
		subModel->_materialID = i;

		// インデックス
		for (int j = 0; j < fs[i].size(); j++) {
			for (int l = 0; fs[i][j].size() - l >= 3; l++) {
				subModel->_vertexIndices.push_back(offset);
				subModel->_vertexIndices.push_back(offset + 1 + l);
				subModel->_vertexIndices.push_back(offset + 2 + l);
				std::cout << offset << " " << offset + 1 + j << " " << offset + 2 + j << endl;
			}
			std::cout << j << std::endl;
			offset += fs[i][j].size();
			std::cout << j << std::endl;
		}

		model->AddSubModel(subModel); // SubModelの追加
	}
	std::cout << "finish making vertexstrct" << endl;

	return S_OK;
}


// MTLファイルのロード、パース
//@param string name : MTLファイル名
//@param Model* model : 情報を格納するModelのポインタ
HRESULT ModelManager::LoadMTLFile(string name, Model* model) {
	std::cout << "start loadOBJFileMethod" << endl;

	string filePath = _modelPath + "\\" + name + ".mtl";
	ifstream ifs(filePath);


	if (!ifs) {
		std::cout << "Don't Open File." << endl;
		assert(0);
		return E_ABORT;
	}

	std::cout << "Open file." << endl;

	std::cout << "start read file" << endl;

	vector<string> memo;
	string line;

	string mtlName;
	Material mtl;
	SubModel* tmpSubModel = model->_subModels[0];
	int offset = 0;
	while (getline(ifs, line)) { // 1行ずつ読み取る
		if (line == "") {

		}
		else if (line[0] == '#') {
			memo.push_back(line.substr(1, string::npos));
		}
		else {
			vector<string> strs;
			strs.clear();
			SeparateString(line, strs, " ");
			if (strs[0] == "newmtl") {
				if (offset > 0) {
					for (auto sm : model->_subModels) {
						if (sm->_materialName == mtlName) {
							sm->_material = mtl;
						}
					}
				}
				mtlName = strs[1];
				offset++;
				Material mtpMtl;
				mtl = mtpMtl;
			}
			else if (strs[0] == "Ka") {
				if (strs.size() == 4) {
					XMFLOAT3 tmpa = { stof(strs[1]), stof(strs[2]), stof(strs[3]) };
					mtl.ambient = tmpa;
				}
			}
			else if (strs[0] == "Kd") {
				if (strs.size() == 4) {
					XMFLOAT3 tmpd = { stof(strs[1]), stof(strs[2]), stof(strs[3]) };
					mtl.diffuse = tmpd;
				}
			}
			else if (strs[0] == "Ks") {
				if (strs.size() == 4) {
					XMFLOAT3 tmps = { stof(strs[1]), stof(strs[2]), stof(strs[3]) };
					mtl.specular = tmps;
				}
			}
			else if (strs[0] == "Ns") {
				if (strs.size() == 2) {
					mtl.specularity = stof(strs[1]);
				}
			}
			else if (strs[0] == "d") {
				if (strs.size() == 2) {
					mtl.dissolve = stof(strs[1]);
				}
			}
			else {

			}


		}
	}

	for (auto sm : model->_subModels) {
		if (sm->_materialName == mtlName) {
			sm->_material = mtl;
		}
	}

	std::cout << "finish read file";

	ifs.close(); // ファイルを閉じる

	std::cout << "Print loaded Material Data" << endl;
	for (auto sm : model->_subModels) {
		std::cout << "Material Name: " << sm->_materialName << endl;
		std::cout << "abient: " << sm->_material.ambient.x << ", " << sm->_material.ambient.y << ", " << sm->_material.ambient.z << endl;
		std::cout << "diffuse: " << sm->_material.diffuse.x << ", " << sm->_material.diffuse.y << ", " << sm->_material.diffuse.z << endl;
		std::cout << "specular: " << sm->_material.specular.x << ", " << sm->_material.specular.y << ", " << sm->_material.specular.z << endl;
		std::cout << "dissolve: " << sm->_material.dissolve << endl;
		std::cout << "specularity: " << sm->_material.specularity << endl;
		std::cout << endl;
	}

	return S_OK;
}

// テスクチャファイルのロード、バッファーの生成を行う関数
HRESULT ModelManager::CreateTextureBuffers(Model* model, string fileName) {
	HRESULT result = S_OK;
	for (auto sm : model->_subModels) {
		result = CreateTextureBuffer(sm, fileName);
	}
	return result;
}

// テクスチャバッファーを生成する関数
HRESULT ModelManager::CreateTextureBuffer(SubModel* sm, string fileName) {
	HRESULT result = S_OK;
	std::cout << "Load Texture File" << endl;
	string filePath = _modelPath + "\\" + fileName;
	wstring wfilePath(filePath.begin(), filePath.end());

	std::cout << filePath << endl;

	result = LoadFromWICFile(wfilePath.c_str(),
		WIC_FLAGS_NONE,
		&sm->_metadata,
		sm->_scratchImage
	);
	std::cout << result << endl;

	if (FALSE(result)) {
		assert(0);
		return result;
	}

	auto img = sm->_scratchImage.GetImage(0, 0, 0);

	D3D12_HEAP_PROPERTIES texHeapProp = {};
	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	texHeapProp.CreationNodeMask = 0;
	texHeapProp.VisibleNodeMask = 0;

	CD3DX12_RESOURCE_DESC resDesc = {};
	resDesc.Format = sm->_metadata.format;
	resDesc.Width = static_cast<UINT>(sm->_metadata.width);
	resDesc.Height = static_cast<UINT>(sm->_metadata.height);
	resDesc.DepthOrArraySize = static_cast<UINT16>(sm->_metadata.arraySize);
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.MipLevels = static_cast<UINT16>(sm->_metadata.mipLevels);
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(sm->_metadata.dimension);
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	result = _dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(sm->_textureBuffer.ReleaseAndGetAddressOf())
	);

	if (FALSE(result)) {
		assert(0);
		return result;
	}

	sm->_textureName = fileName;

	result = sm->_textureBuffer->WriteToSubresource(
		0,
		nullptr,
		img->pixels,
		static_cast<UINT>(img->rowPitch),
		static_cast<UINT>(img->slicePitch)
	);

	if (FALSE(result)) {
		assert(0);
		return result;
	}


	return result;
}

// 頂点バッファーを生成する関数
HRESULT ModelManager::CreateVertexBuffer(Model* model) {
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(model->_vertices.size() * sizeof(Vertex));

	HRESULT result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model->_vertexBuffer.ReleaseAndGetAddressOf())
	);

	Vertex* vertMap = nullptr;
	result = model->_vertexBuffer->Map(0, nullptr, (void**)&vertMap);
	copy(begin(model->_vertices), end(model->_vertices), vertMap);
	//std::cout << "vertices num:" << model._vertices.size() << endl;
	//std::cout << "check vertMap" << endl;
	//for (int i = 0; i < model._vertices.size(); i++) {
	//	std::cout << i+1 <<  " pos: " << vertMap[i].position.x << " " << vertMap[i].position.y << " " << vertMap[i].position.z << endl;
	//	std::cout << i+1 <<  " nor: " << vertMap[i].normal.x << " " << vertMap[i].normal.y << " " << vertMap[i].normal.z << endl;
	//	std::cout << i+1 <<  " uv : " << vertMap[i].uv.x << " " << vertMap[i].uv.y << endl;
	//}
	model->_vertexBuffer->Unmap(0, nullptr);

	model->_vertexBufferView.BufferLocation = model->_vertexBuffer->GetGPUVirtualAddress();
	model->_vertexBufferView.SizeInBytes = model->_vertexBuffer->GetDesc().Width;
	//std::cout << "size of vbv: " << model._vertexBufferView.SizeInBytes << endl;
	model->_vertexBufferView.StrideInBytes = sizeof(Vertex);
	//std::cout << " one size of vbv: " << model._vertexBufferView.StrideInBytes << endl;



	return result;
}

// インデックスバッファを生成する関数
HRESULT ModelManager::CreateIndexBuffer(Model* model) {
	HRESULT result = S_OK;
	for (auto sm : model->_subModels) {
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(sm->_vertexIndices.size() * sizeof(unsigned int));

		result = _dev->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,

			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(sm->_indexBuffer.ReleaseAndGetAddressOf())
		);
		if (FALSE(result)) {
			assert(0);
			return result;
		}

		unsigned int* indeMap = nullptr;
		result = sm->_indexBuffer->Map(0, nullptr, (void**)&indeMap);
		copy(begin(sm->_vertexIndices), end(sm->_vertexIndices), indeMap);
		std::cout << "check indexMap" << endl;
		for (int i = 0; i < sm->_vertexIndices.size() / 3; i++) {
			std::cout << indeMap[3 * i] << " " << indeMap[3 * i + 1] << " " << indeMap[3 * i + 2] << endl;
		}
		std::cout << endl;
		sm->_indexBuffer->Unmap(0, nullptr);

		sm->_indexBufferView.BufferLocation = sm->_indexBuffer->GetGPUVirtualAddress();
		sm->_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		sm->_indexBufferView.SizeInBytes = sm->_indexBuffer->GetDesc().Width;
	}
	return result;
}

// マテリアルのバッファを生成する関数
HRESULT ModelManager::CreateMaterialBuffer(Model* model) {
	HRESULT result = S_OK;
	std::cout << "Print loaded Material Data" << endl;
	for (auto sm : model->_subModels) {
		std::cout << "Material Name: " << sm->_materialName << endl;
		std::cout << "abient: " << sm->_material.ambient.x << ", " << sm->_material.ambient.y << ", " << sm->_material.ambient.z << endl;
		std::cout << "diffuse: " << sm->_material.diffuse.x << ", " << sm->_material.diffuse.y << ", " << sm->_material.diffuse.z << endl;
		std::cout << "specular: " << sm->_material.specular.x << ", " << sm->_material.specular.y << ", " << sm->_material.specular.z << endl;
		std::cout << "dissolve: " << sm->_material.dissolve << endl;
		std::cout << "specularity: " << sm->_material.specularity << endl;
		std::cout << endl;

		// マテリアルバッファーを作成
		auto materialBufferSize = sizeof(Material);
		materialBufferSize = (materialBufferSize + 0xff) & ~0xff;
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(materialBufferSize);
		HRESULT result = _dev->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(sm->_materialBuffer.ReleaseAndGetAddressOf())
		);

		if (FAILED(result)) {
			assert(0);
			return result;
		}

		result = sm->_materialBuffer->Map(0, nullptr, (void**)&sm->_pMaterial);
		if (FAILED(result)) {
			assert(0);
			return result;
		}

		*sm->_pMaterial = sm->_material;

		// マップされた中身を表示する
		/*
		for (auto& m : model._materials) {

			*((Material*)tmaterialMap) = m.second;
			//tmaterialMap += materialBufferSize;
		}
		*/

		/*
		Material* materialMap = (Material*)tmaterialMap;

		std::cout << "Print Mapped Material Data" << endl;
		//for (int i = 0; i < sm->_materials.size(); i++) {
			std::cout << "Mapped Material Index : " << i << endl;
			std::cout << "ambient: " << materialMap[i].ambient.x << ", " << materialMap[i].ambient.y << ", " << materialMap[i].ambient.z << endl;
			std::cout << "diffuse: " << materialMap[i].diffuse.x << ", " << materialMap[i].diffuse.y << ", " << materialMap[i].diffuse.z << endl;
			std::cout << "specular: " << materialMap[i].specular.x << ", " << materialMap[i].specular.y << ", " << materialMap[i].specular.z << endl;
			std::cout << "dissolve: " << materialMap[i].dissolve << endl;
			std::cout << "specularity: " << materialMap[i].specularity << endl;
		//}
		*/

		//sm->_materialBuffer->Unmap(0, nullptr);

		sm->_cbvDesc.BufferLocation = sm->_materialBuffer->GetGPUVirtualAddress();
		sm->_cbvDesc.SizeInBytes = static_cast<UINT>(materialBufferSize);

	}
	return S_OK;
}

// Modelの変換行列のバッファーを生成する関数
HRESULT ModelManager::CreateModelTransformBuffer(Model* model) {
	auto heapPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SRTMatrices) + 0xff) & ~0xff);
	HRESULT result = _dev->CreateCommittedResource(
		&heapPro,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(model->_basicBuffer.ReleaseAndGetAddressOf())
	);

	if (FAILED(result)) {
		assert(0);
		return result;
	}


	result = model->_basicBuffer->Map(0, nullptr, (void**)&model->_srt);
	model->Update();

	if (FAILED(result)) {
		assert(0);
		return result;
	}


	for (auto sm : model->_subModels) {
		result = _dev->CreateCommittedResource(
			&heapPro,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(sm->_basicBuffer.ReleaseAndGetAddressOf())
		);

		if (FAILED(result)) {
			assert(0);
			return result;
		}

		result = sm->_basicBuffer->Map(0, nullptr, (void**)&sm->_srt);
		sm->Update();
		if (FAILED(result)) {
			assert(0);
			return result;
		}
	}

	return S_OK;
}

// Modelの変換行列のビューを生成する関数
HRESULT ModelManager::CreateModelTransformView(Model* model) {
	// Modelのディスクリプターヒープ

	D3D12_DESCRIPTOR_HEAP_DESC modelDescHeapDesc = {};
	modelDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	modelDescHeapDesc.NodeMask = 0;
	modelDescHeapDesc.NumDescriptors = 1;
	modelDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	HRESULT result = _dev->CreateDescriptorHeap(&modelDescHeapDesc, IID_PPV_ARGS(model->_heap.ReleaseAndGetAddressOf()));

	if (FAILED(result)) {
		HRESULT tmp = _dev->GetDeviceRemovedReason();
		assert(0);
		return result;
	}

	auto basicHeapHandle = model->_heap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CONSTANT_BUFFER_VIEW_DESC modelCbvDesc = {};
	modelCbvDesc.BufferLocation = model->_basicBuffer->GetGPUVirtualAddress();
	modelCbvDesc.SizeInBytes = static_cast<UINT>(model->_basicBuffer->GetDesc().Width);

	_dev->CreateConstantBufferView(&modelCbvDesc, basicHeapHandle);

	return S_OK;
}

// SubModelの変換行列のビューを生成する関数
HRESULT ModelManager::CreateSubModelTransformView(Model* model) {
	for (auto sm : model->_subModels) {
		sm->_subModelDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		sm->_subModelDescHeapDesc.NodeMask = 0;
		sm->_subModelDescHeapDesc.NumDescriptors = 3;
		sm->_subModelDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;



		HRESULT result = _dev->CreateDescriptorHeap(&sm->_subModelDescHeapDesc, IID_PPV_ARGS(sm->_heap.ReleaseAndGetAddressOf()));

		if (FAILED(result)) {
			std::cout << result << std::endl;
			assert(0);
			return result;
		}

		auto basicHeapHandle = sm->_heap->GetCPUDescriptorHandleForHeapStart();

		D3D12_CONSTANT_BUFFER_VIEW_DESC subModelCbvDesc = {};
		subModelCbvDesc.BufferLocation = sm->_basicBuffer->GetGPUVirtualAddress();
		subModelCbvDesc.SizeInBytes = static_cast<UINT>(sm->_basicBuffer->GetDesc().Width);

		_dev->CreateConstantBufferView(&subModelCbvDesc, basicHeapHandle);
	}

	return S_OK;
}

// テクスチャのビューを生成する関数
HRESULT ModelManager::CreateTextureViews(Model* model) {
	for (auto sm : model->_subModels) {
		CreateTextureView(sm);
	}
	return S_OK;
}

// テクスチャのビューを生成する関数
HRESULT ModelManager::CreateTextureView(SubModel* sm) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sm->_metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	auto basicHeapH = sm->_heap->GetCPUDescriptorHandleForHeapStart();

	basicHeapH.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	basicHeapH.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	_dev->CreateShaderResourceView(
		sm->_textureBuffer.Get(),
		&srvDesc,
		basicHeapH
	);

	return S_OK;
}

// マテリアルビューを生成する関数
HRESULT ModelManager::CreateMaterialView(Model* model) {
	for (auto sm : model->_subModels) {
		auto basicHeapH = sm->_heap->GetCPUDescriptorHandleForHeapStart();

		basicHeapH.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = sm->_materialBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = static_cast<UINT>(sm->_materialBuffer->GetDesc().Width);

		_dev->CreateConstantBufferView(&cbvDesc, basicHeapH);
	}

	return S_OK;
}


// 指定した文字列で文字列を分解する関数
// @param string str: 分解される文字列
// @param vector<string>& vec: 分解した文字列を保管する文字列
// @param const string cs: 分解する文字列
void ModelManager::SeparateString(string str, vector<string>& vec, const string cs) {
	int offset = 0;
	int tmpPos = str.find(cs, offset);

	while (tmpPos != -1) {
		if (tmpPos != offset) vec.push_back(str.substr(offset, tmpPos - offset));
		offset = tmpPos + 1;
		tmpPos = str.find(cs, offset);
	}
	if (offset < str.size()) {
		vec.push_back(str.substr(offset, str.size() - offset));
	}

	return;
}

// SubModelのテクスチャを変更する関数
bool ModelManager::ChangeTexture(int mId, int smId, string fileName) {
	HRESULT result = CreateTextureBuffer(_models[mId]->_subModels[smId], fileName);
	result = CreateTextureView(_models[mId]->_subModels[smId]);
	if (result != S_OK) return false;
	else return true;
}

// 指定したModelインデックスを複製する関数
void ModelManager::DuplicateModel(int n) {
	CreateModel(_models[n]->_OBJFileName, _models[n]->_isMTLExist);
	int index = _models.size() - 1;
	_models[index]->_transform = _models[n]->_transform;
	_models[index]->Update();

	for (int i = 0; i < _models[n]->_subModels.size(); i++) {
		_models[index]->_subModels[i]->_transform = _models[n]->_subModels[i]->_transform;
		_models[index]->_subModels[i]->_material = _models[n]->_subModels[i]->_material;
		_models[index]->_subModels[i]->Update();
		
		CreateTextureBuffer(_models[index]->_subModels[i], _models[n]->_subModels[i]->_textureName);
		CreateTextureView(_models[index]->_subModels[i]);
	}
}