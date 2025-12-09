#pragma once
#include "../Engine/GameObject.h"

class SphereCollider;

// 敵のドロップアイテムを管理するクラス
class Jewel : public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	Jewel(GameObject* parent);
	//デストラクタ
	~Jewel();
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//開放
	void Release() override;
	//当たり判定
	void OnCollision(GameObject* pTarget) override;

private:
	int modelHandle_;
	SphereCollider* pCollider_;
};