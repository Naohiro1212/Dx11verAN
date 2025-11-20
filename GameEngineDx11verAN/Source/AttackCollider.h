#pragma once
#include "../Engine/GameObject.h"

class BoxCollider;

class AttackCollider : public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（Player）
	AttackCollider(GameObject* parent);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//開放
	void Release() override;

private:
	BoxCollider* pCollider_;
	int debugModel_;

};