#include"BasicType.hlsli"
Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);

//�萔�o�b�t�@0
//�J�������
cbuffer SceneData : register(b0) {
	matrix world;// ���[���h�ϊ��s��
	matrix view;// �r���[�s��
	matrix proj;// �v���W�F�N�V�����s��
	float3 eye; // �J�����̈ʒu
	float3 lightVector; // Direction Light�̃x�N�g��
	float3 lightColor; // Direction Light�̐F
};

//�萔�o�b�t�@1
//���f����SRT�s��
cbuffer ModelData : register(b1) {
	matrix m_s;
	matrix m_r;
	matrix m_t;
};

//�萔�o�b�t�@2
//�T�u���f����SRT�s��
cbuffer SubModelData : register(b2) {
	matrix sm_s;
	matrix sm_r;
	matrix sm_t;
};

BasicType BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD) {
	BasicType output;//�s�N�Z���V�F�[�_�֓n���l
	pos = mul(world, pos);
	pos = mul(sm_s, pos);
	pos = mul(sm_r, pos);
	pos = mul(sm_t, pos);
	pos = mul(m_s, pos);
	pos = mul(m_r, pos);
	pos = mul(m_t, pos);
	output.svpos = mul(mul(proj, view), pos);//�V�F�[�_�ł͗�D��Ȃ̂Œ���
	output.pos = mul(view, pos);
	normal.w = 0;//�����d�v(���s�ړ������𖳌��ɂ���)

	normal = mul(m_r, normal);
	output.normal = mul(world, normal);//�@���ɂ����[���h�ϊ����s��
	output.vnormal = mul(view, output.normal);
	output.uv = uv;
	output.ray = normalize(pos.xyz - mul(view, eye));//�����x�N�g��

	return output;
}
