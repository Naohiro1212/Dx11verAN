#include "EnemyDeathEffect.h"
#include "../Engine/GameTime.h"

namespace
{
	const float DURATION = 2.0f;
}

EnemyDeathEffect::EnemyDeathEffect(GameObject* parent, XMFLOAT3 pos) : GameObject(parent, "EnemyDeathEffect"), 
timer_(0.0f), data_(), hEmit_(-1)
{
	transform_.position_ = pos;
}

void EnemyDeathEffect::Initialize()
{
	data_.textureFileName = "Effects/circle_B.png";
	data_.isBillBoard = true;

	data_.position = { transform_.position_.x, transform_.position_.y + 5.0f, transform_.position_.z };
	data_.positionRnd = XMFLOAT3(0.6f, 0.2f, 0.6f);   // 少し広げて発生

	data_.delay = 10;        
	data_.number = 15;       // 密度
	data_.lifeTime = 30;      // 長めに残す（フレーム）

	data_.color = XMFLOAT4(0.9f, 0.2f, 1.0f, 0.85f); // 濃いめ紫＋高α
	data_.deltaColor = XMFLOAT4(-0.001f, -0.001f, -0.001f, -0.012f); // 徐々に暗く＆フェードアウト

	data_.size = XMFLOAT2(2.5f, 2.5f);   // 円形に近い
	data_.sizeRnd = XMFLOAT2(0.6f, 0.6f);   // サイズにばらつき
	data_.scale = XMFLOAT2(1.015f, 1.018f); // 毎フレーム少し拡大（拡散演出）

	// 上向きの弱い流れ＋微小な水平広がり
	data_.direction = XMFLOAT3(0.0f, 1.0f, 0.0f);
	data_.directionRnd = XMFLOAT3(18.0f, 8.0f, 18.0f); // 角度のランダム（小さめ）
	data_.speed = 0.045f;    // 初速は弱め
	data_.accel = 0.992f;    // ゆるく減速（拡散感）
	data_.gravity = -0.055f;   // 上昇を徐々に止める（落下はしない程度）

	// ほんの少し回転を付ける（煙のゆらぎ）
	data_.spin = XMFLOAT3(0.0f, 0.0f, 0.06f);
	data_.rotateRnd = XMFLOAT3(0.0f, 0.0f, 180.0f);

	hEmit_ = VFX::Start(data_);
}

void EnemyDeathEffect::Update()
{
	float dt_ = GameTime::DeltaTime();
	timer_ += dt_;
	if (timer_ >= DURATION)
	{
		VFX::End(hEmit_);
		hEmit_ = -1;
		KillMe();
	}
}

void EnemyDeathEffect::Draw()
{
}

void EnemyDeathEffect::Release()
{
}
