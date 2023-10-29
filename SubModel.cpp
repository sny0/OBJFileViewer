#include "SubModel.h"

SubModel::SubModel() {

}

void SubModel::Update() {
	// 平行移動の変換行列を更新
	_srt->S = XMMatrixScaling(_transform.scale.x, _transform.scale.y, _transform.scale.z);

	// [deg]から[rad]に変換し、回転の変換行列を更新
	_srt->R = XMMatrixRotationRollPitchYaw(
		_transform.rotation.x * XM_PI / 180.0f,
		_transform.rotation.y * XM_PI / 180.0f,
		_transform.rotation.z * XM_PI / 180.0f
	);

	// 拡大・縮小の変換行列を更新
	_srt->T = XMMatrixTranslation(_transform.position.x, _transform.position.y, _transform.position.z);

	// マテリアルを更新
	*_pMaterial = _material;
	return;
}

SubModel::~SubModel() {

}