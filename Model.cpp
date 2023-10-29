#include"Model.h"

Model::Model(string fileName, bool exist) : _OBJFileName(fileName), _isMTLExist(exist) {

}

void Model::Update() {
	_srt->S = XMMatrixScaling(_transform.scale.x, _transform.scale.y, _transform.scale.z);
	_srt->R = XMMatrixRotationRollPitchYaw(
		_transform.rotation.x * XM_PI / 180.0f,
		_transform.rotation.y * XM_PI / 180.0f,
		_transform.rotation.z * XM_PI / 180.0f
	);
	_srt->T = XMMatrixTranslation(_transform.position.x, _transform.position.y, _transform.position.z);

	return;
}

Model::~Model() {

}


void Model::AddSubModel(SubModel* sm) {
	_subModels.push_back(sm);
}