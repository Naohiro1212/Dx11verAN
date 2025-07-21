#pragma once
#include "Collider.h"

// 球体の当たり判定
class SphereCollider : public Collider
{
	// Colliderクラスのprivateメンバにアクセス出来るようにする
	friend class Collider;

public:
	// コンストラクタ（当たり判定の作成）
	// 引数：basePos 当たり判定の中心位置（ゲームオブジェクトの原点から見た位置）
	// 引数：radius  当たり判定のサイズ（半径）
	SphereCollider(XMFLOAT3 center, float radius);

private:
	// 接触判定
	// 引数：target　相手の当たり判定
	// 戻り値：接触していればtrue
	bool IsHit(Collider* target) override;
};