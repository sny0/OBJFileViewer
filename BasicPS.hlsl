#include"BasicType.hlsli"
Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);

//�萔�o�b�t�@0
cbuffer SceneData : register(b0) {
	matrix world;// ���[���h�ϊ��s��
	matrix view;// �r���[�s��
	matrix proj;// �v���W�F�N�V�����s��
	float3 eye; // �J�����̈ʒu
	float3 sd_lightVector; // Direction Light�̃x�N�g��
	float3 sd_lightColor; // Direction Light�̐F
};

//�萔�o�b�t�@3
//Material
cbuffer Material : register(b3) {
	float3 ambientColor; // ����
	float3 diffuseColor; // �g�U���ˌ�
	float3 specularColor; // ���ʔ��ˌ�
	float dissolve; // �����x
	float specularity; // ���ʔ��˗�
};


float4 BasicPS(BasicType input) : SV_TARGET{

	float4 lightColor = {sd_lightColor.xyz, 1.0f}; // ���C�g�̌�
	float3 lightVec = normalize(sd_lightVector); // ���C�g�i�f�B���N�V�������C�g�j�̕����x�N�g��

	float diffuseBrightness = saturate(dot(-lightVec, input.normal)); // �g�U���˂̋���

	float3 reflectLightVec = normalize(reflect(lightVec, input.normal.xyz)); // ���C�g���e�N�Z���̖@���Ŕ��˂����Ƃ��̃x�N�g��
	float specularBrightness = pow(saturate(dot(reflectLightVec, -input.ray)), 3) * specularity / 1; // ���ʔ��˂̋���

	float4 textureColor = tex.Sample(smp, input.uv); // �e�N�X�`���̐F

	float3 finalColorxyz = saturate(lightColor * textureColor * diffuseBrightness * diffuseColor); // �g�U���˂̓K��
	finalColorxyz += saturate(lightColor * textureColor * specularBrightness * specularColor.xyz); // ���ʔ��˂̓K��
	finalColorxyz += 0.5 * saturate(lightColor * textureColor * ambientColor.xyz); // �����̓K��
	float4 finalColor = float4(finalColorxyz, dissolve); // �����x�̓K��
	finalColor = saturate(finalColor); // �ۂ߂�

	return finalColor;
}