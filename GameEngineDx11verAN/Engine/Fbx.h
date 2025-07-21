#pragma once
#include <d3d11.h>
#include <fbxsdk.h>
#include <vector>
#include <string>
#include "Transform.h"

class FbxParts;

// レイキャスト用構造体
struct RayCastData {
	XMFLOAT3 start;
	XMFLOAT3 dir;
	float dist;
	BOOL hit;
	XMFLOAT3 normal;

	RayCastData()
		: start(0.f, 0.f, 0.f)
		, dir(0.f, 0.f, 0.f)
		, dist(99999.0f)
		, hit(FALSE)
		, normal(0.f, 0.f, 0.f)
	{}
};

// FBXファイルを扱うクラス
// ほとんどの処理は各パーツ

class Fbx
{
	// FbxPartクラスをフレンドクラスにする
	// FbxPartのprivateな関数にもアクセス可能
	friend class FbxParts;

	// モデルの各パーツ（複数あるかも）
	std::vector<FbxParts*> parts_;

	// FBXファイルを扱う機能の本体
	FbxManager* pFbxManager_;

	// FBXファイルのシーン（Mayaで作ったすべての物体）を扱う
	FbxScene* pFbxScene_;

	// アニメーションのフレームレート
	FbxTime::EMode _frameRate;

	// アニメーション速度
	float _animSpeed;

	// アニメーションの最初と最後のフレーム
	int _startFrame, _endFrame;

	// ノードの中身を調べる
	// 引数：pNode 調べるノード
	// 引数：pPartsList パーツのリスト
	void CheckNode(FbxNode* pNode, std::vector<FbxParts*>* pPartsList);

public:
	Fbx();
	~Fbx();

	FbxManager* GetFbxManager()
	{
		return pFbxManager_;
	}
	FbxScene* GetFbxScene()
	{
		return pFbxScene_;
	}

	//ロード
	//引数：fileName	ファイル名
	//戻値：成功したかどうか
	virtual HRESULT Load(std::string fileName);

	//描画
	//引数：World	ワールド行列
	void    Draw(Transform& transform, int frame);

	//解放
	void    Release();

	//任意のボーンの位置を取得
	//引数：boneName	取得したいボーンの位置
	//戻値：ボーンの位置
	XMFLOAT3 GetBonePosition(std::string boneName);

	//スキンメッシュアニメ中の現在の任意のボーンの位置を取得
	//引数：boneName	取得したいボーンの位置
	//戻値：ボーンの位置
	XMFLOAT3 GetAnimBonePosition(std::string boneName);

	//レイキャスト（レイを飛ばして当たり判定）
	//引数：data	必要なものをまとめたデータ
	void RayCast(RayCastData* data);
};