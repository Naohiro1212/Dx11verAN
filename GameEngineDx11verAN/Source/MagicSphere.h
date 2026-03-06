#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/VFX.h"
#include <vector>

class SphereCollider;
class BoxCollider;

class MagicSphere : public GameObject
{
public:
	//コンストラクタ
	MagicSphere(GameObject* parent);
	MagicSphere(GameObject* parent, const std::vector<BoxCollider*>& _wallColliders);
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
	float attackTimer_;

	EmitterData effectData_;
	int hEmit_;
	SphereCollider* pCollider_;
	std::vector<BoxCollider*> wallColliders_;
};