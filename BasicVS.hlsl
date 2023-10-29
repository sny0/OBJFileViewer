#include"BasicType.hlsli"
Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);

//定数バッファ0
//カメラ情報
cbuffer SceneData : register(b0) {
	matrix world;// ワールド変換行列
	matrix view;// ビュー行列
	matrix proj;// プロジェクション行列
	float3 eye; // カメラの位置
	float3 lightVector; // Direction Lightのベクトル
	float3 lightColor; // Direction Lightの色
};

//定数バッファ1
//モデルのSRT行列
cbuffer ModelData : register(b1) {
	matrix m_s;
	matrix m_r;
	matrix m_t;
};

//定数バッファ2
//サブモデルのSRT行列
cbuffer SubModelData : register(b2) {
	matrix sm_s;
	matrix sm_r;
	matrix sm_t;
};

BasicType BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD) {
	BasicType output;//ピクセルシェーダへ渡す値
	pos = mul(world, pos);
	pos = mul(sm_s, pos);
	pos = mul(sm_r, pos);
	pos = mul(sm_t, pos);
	pos = mul(m_s, pos);
	pos = mul(m_r, pos);
	pos = mul(m_t, pos);
	output.svpos = mul(mul(proj, view), pos);//シェーダでは列優先なので注意
	output.pos = mul(view, pos);
	normal.w = 0;//ここ重要(平行移動成分を無効にする)

	normal = mul(m_r, normal);
	output.normal = mul(world, normal);//法線にもワールド変換を行う
	output.vnormal = mul(view, output.normal);
	output.uv = uv;
	output.ray = normalize(pos.xyz - mul(view, eye));//視線ベクトル

	return output;
}
