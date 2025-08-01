#pragma once
#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 position; // 頂点の位置
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 tex;
};

struct ConstantBuffer
{
	DirectX::XMMATRIX worldViewProj;
	float time;
	float padding[3]; // パディングで16バイト境界に合わせる
};