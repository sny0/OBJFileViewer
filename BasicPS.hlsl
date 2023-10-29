#include"BasicType.hlsli"
Texture2D<float4> tex:register(t0);
SamplerState smp:register(s0);

//定数バッファ0
cbuffer SceneData : register(b0) {
	matrix world;// ワールド変換行列
	matrix view;// ビュー行列
	matrix proj;// プロジェクション行列
	float3 eye; // カメラの位置
	float3 sd_lightVector; // Direction Lightのベクトル
	float3 sd_lightColor; // Direction Lightの色
};

//定数バッファ3
//Material
cbuffer Material : register(b3) {
	float3 ambientColor; // 環境光
	float3 diffuseColor; // 拡散反射光
	float3 specularColor; // 鏡面反射光
	float dissolve; // 透明度
	float specularity; // 鏡面反射率
};


float4 BasicPS(BasicType input) : SV_TARGET{

	float4 lightColor = {sd_lightColor.xyz, 1.0f}; // ライトの光
	float3 lightVec = normalize(sd_lightVector); // ライト（ディレクションライト）の方向ベクトル

	float diffuseBrightness = saturate(dot(-lightVec, input.normal)); // 拡散反射の強さ

	float3 reflectLightVec = normalize(reflect(lightVec, input.normal.xyz)); // ライトがテクセルの法線で反射したときのベクトル
	float specularBrightness = pow(saturate(dot(reflectLightVec, -input.ray)), 3) * specularity / 1; // 鏡面反射の強さ

	float4 textureColor = tex.Sample(smp, input.uv); // テクスチャの色

	float3 finalColorxyz = saturate(lightColor * textureColor * diffuseBrightness * diffuseColor); // 拡散反射の適応
	finalColorxyz += saturate(lightColor * textureColor * specularBrightness * specularColor.xyz); // 鏡面反射の適応
	finalColorxyz += 0.5 * saturate(lightColor * textureColor * ambientColor.xyz); // 環境光の適応
	float4 finalColor = float4(finalColorxyz, dissolve); // 透明度の適応
	finalColor = saturate(finalColor); // 丸める

	return finalColor;
}