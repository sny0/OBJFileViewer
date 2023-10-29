#pragma once
#include<string>
#include<Windows.h>
#include<DirectXMath.h>

using namespace DirectX;

// Vertex情報
struct Vertex {
	XMFLOAT3 position; // 位置
	XMFLOAT3 normal; // 法線
	XMFLOAT2 uv; // UV座標
};

// Material情報
struct Material {
	XMFLOAT3 ambient = { 1.0f, 1.0f, 1.0f }; // アンビエント光の色 0.0f～1.0f
	float pad0 = 0.0f;
	XMFLOAT3 diffuse = { 1.0f, 1.0f, 1.0f }; // 拡散反射光の色 0.0f～1.0f
	float pad1 = 0.0f;
	XMFLOAT3 specular = { 1.0f, 1.0, 1.0f }; // 鏡面反射光の色 0.0f～1.0f
	float dissolve = 1.0f; // 透明度 0.0～1.0f
	float specularity = 10.0f; // specularの強さ 0f～1000f
};

// カメラ（変換行列）情報
struct SceneData {
	XMMATRIX world; // ワールド行列
	XMMATRIX view; // ビュー行列
	XMMATRIX proj; // プロジェクション行列
	XMFLOAT3 eye; // 視点位置
	float pad0 = 0.0f;
	XMFLOAT3 ligVec; // ディレクションライトのベクトル
	float pad1 = 0.0f;
	XMFLOAT3 ligCol; // ディレクションライトの色
};

// SRT(Transform)情報
struct Transform {
	XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
	float pad0 = 0.0f;
	XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
	float pad1 = 0.0f;
	XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
	float pad2 = 0.0f;
};

// SRTの変換行列
struct SRTMatrices {
	XMMATRIX S; // Scale
	XMMATRIX R; // Rotation
	XMMATRIX T; // Transform

};
