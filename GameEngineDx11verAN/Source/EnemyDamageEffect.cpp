#include "EnemyDamageEffect.h"
#include "../Engine/GameTime.h"

namespace 
{
    // エフェクトの持続時間
    const float DURATION = 2.0f;

	// 頭上に表示するための高さオフセット
	const float HEIGHT_OFFSET = 2.5f;
}

EnemyDamageEffect::EnemyDamageEffect(GameObject* parent, XMFLOAT3 pos) : GameObject(parent, "EnemyDamageEffect"),
timer_(0.0f), data_(), hEmit_(-1)
{
	transform_.position_ = pos;
}

void EnemyDamageEffect::Initialize()
{
    // 流血表現：その場で血しぶきが広がる
    data_.textureFileName = "Effects/blood.png";
    data_.isBillBoard = true;

    data_.position = transform_.position_;
    data_.position.y += HEIGHT_OFFSET;
    data_.positionRnd = XMFLOAT3(0.5f, 0.1f, 0.5f); // 少しだけ広がる

    data_.delay = 0;
    data_.number = 12;
    data_.lifeTime = 120;

    data_.color = XMFLOAT4(0.8f, 0.05f, 0.05f, 0.85f); // 濃い赤
    data_.deltaColor = XMFLOAT4(-0.002f, -0.001f, -0.001f, -0.018f); // 徐々に暗く＆フェードアウト

    data_.size = XMFLOAT2(10.0f, 10.0f);
    data_.sizeRnd = XMFLOAT2(0.5f, 0.5f);
    data_.scale = XMFLOAT2(1.01f, 1.01f); // わずかに拡大

    data_.direction = XMFLOAT3(0.0f, 0.2f, 0.0f); // わずかに上方向
    data_.directionRnd = XMFLOAT3(30.0f, 10.0f, 30.0f); // 角度ランダム
    data_.speed = 0.09f;    // 少し速め
    data_.accel = 0.96f;    // ゆるく減速
    data_.gravity = -0.012f;   // 上昇を止める

    data_.spin = XMFLOAT3(0.0f, 0.0f, 0.12f);
    data_.rotateRnd = XMFLOAT3(0.0f, 0.0f, 180.0f);

    hEmit_ = VFX::Start(data_);
}

void EnemyDamageEffect::Update()
{
    float dt_ = GameTime::DeltaTime();
    timer_ += dt_;
    if (timer_ >= DURATION) // エフェクトの持続時間
    {
        if (hEmit_ != -1) 
        {
            VFX::End(hEmit_);
            hEmit_ = -1;
        }
        KillMe();
	}

	data_.position.y = transform_.position_.y + HEIGHT_OFFSET; // 常にプレイヤーの頭上に位置
	VFX::SetEmitterPosition(hEmit_, data_.position);
}

void EnemyDamageEffect::Draw()
{
}

void EnemyDamageEffect::Release()
{
}
