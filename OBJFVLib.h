#pragma once
#include<string>
#include<Windows.h>
#include<DirectXMath.h>

using namespace DirectX;

// Vertex���
struct Vertex {
	XMFLOAT3 position; // �ʒu
	XMFLOAT3 normal; // �@��
	XMFLOAT2 uv; // UV���W
};

// Material���
struct Material {
	XMFLOAT3 ambient = { 1.0f, 1.0f, 1.0f }; // �A���r�G���g���̐F 0.0f�`1.0f
	float pad0 = 0.0f;
	XMFLOAT3 diffuse = { 1.0f, 1.0f, 1.0f }; // �g�U���ˌ��̐F 0.0f�`1.0f
	float pad1 = 0.0f;
	XMFLOAT3 specular = { 1.0f, 1.0, 1.0f }; // ���ʔ��ˌ��̐F 0.0f�`1.0f
	float dissolve = 1.0f; // �����x 0.0�`1.0f
	float specularity = 10.0f; // specular�̋��� 0f�`1000f
};

// �J�����i�ϊ��s��j���
struct SceneData {
	XMMATRIX world; // ���[���h�s��
	XMMATRIX view; // �r���[�s��
	XMMATRIX proj; // �v���W�F�N�V�����s��
	XMFLOAT3 eye; // ���_�ʒu
	float pad0 = 0.0f;
	XMFLOAT3 ligVec; // �f�B���N�V�������C�g�̃x�N�g��
	float pad1 = 0.0f;
	XMFLOAT3 ligCol; // �f�B���N�V�������C�g�̐F
};

// SRT(Transform)���
struct Transform {
	XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	float pad0 = 0.0f;
	XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
	float pad1 = 0.0f;
	XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
	float pad2 = 0.0f;
};

// SRT�̕ϊ��s��
struct SRTMatrices {
	XMMATRIX S; // Scale
	XMMATRIX R; // Rotation
	XMMATRIX T; // Transform

};
