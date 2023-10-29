#include "SubModel.h"

SubModel::SubModel() {

}

void SubModel::Update() {
	// ���s�ړ��̕ϊ��s����X�V
	_srt->S = XMMatrixScaling(_transform.scale.x, _transform.scale.y, _transform.scale.z);

	// [deg]����[rad]�ɕϊ����A��]�̕ϊ��s����X�V
	_srt->R = XMMatrixRotationRollPitchYaw(
		_transform.rotation.x * XM_PI / 180.0f,
		_transform.rotation.y * XM_PI / 180.0f,
		_transform.rotation.z * XM_PI / 180.0f
	);

	// �g��E�k���̕ϊ��s����X�V
	_srt->T = XMMatrixTranslation(_transform.position.x, _transform.position.y, _transform.position.z);

	// �}�e���A�����X�V
	*_pMaterial = _material;
	return;
}

SubModel::~SubModel() {

}