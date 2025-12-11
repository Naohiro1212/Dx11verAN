#pragma once
#include "../Engine/GameObject.h"

class BoxCollider;

class Plane :
	public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	Plane(GameObject* parent);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;

	int GetPlaneHandle() { return planeHandle_; }
	BoxCollider* GetWallCollider() { return wallCollider_; }

private:
	int planeHandle_;
	BoxCollider* wallCollider_;

	XMFLOAT3 wallPos_;
	XMFLOAT3 wallSize_;
};

