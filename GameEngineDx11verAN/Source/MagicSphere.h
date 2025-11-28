#pragma once
#include "../Engine/GameObject.h"

class SphereCollider;

class MagicSphere : public GameObject
{
public:
	//コンストラクタ
	MagicSphere(GameObject* parent);
	//デストラクタ
	~MagicSphere();
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//開放
	void Release() override;

	void OnCollision(GameObject* pTarget) override;

private:
	int magicModel_;
	float AttackTimer_;

	SphereCollider* pCollider_;
};