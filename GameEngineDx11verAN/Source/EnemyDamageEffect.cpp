#include "EnemyDamageEffect.h"
#include "../Engine/GameTime.h"

namespace 
{
    // エフェクトの持続時間
    const float DURATION = 2.0f;

	// 頭上に表示するための高さオフセット
	const float HEIGHT_OFFSET = 9.0f;
}

EnemyDamageEffect::EnemyDamageEffect(GameObject* parent, XMFLOAT3 pos) : GameObject(parent, "EnemyDamageEffect"),
timer_(0.0f), data_(), hEmit_{ -1, -1 }, slashData_(), slashTimer_(0.0f)
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

    hEmit_[BLOOD_EFFECT] = VFX::Start(data_);

    slashData_;
    slashData_.textureFileName = "Effects/slash_B.png"; // 横斬撃用テクスチャ
    slashData_.isBillBoard = true;

    // 斬撃の中心位置（例: エネミーの前方 or プレイヤーの前方）
	slashData_.position = transform_.position_;
	slashData_.position.y += HEIGHT_OFFSET;

    slashData_.number = 1; // 1枚だけ
    slashData_.lifeTime = 15;

    // 横長のサイズ
    slashData_.size = XMFLOAT2(3.5f, 1.0f); // 横幅3.5, 縦1.0
    slashData_.sizeRnd = XMFLOAT2(0.0f, 0.0f);
    slashData_.scale = XMFLOAT2(1.2f, 1.2f);

    // 回転
    slashData_.rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
    slashData_.rotateRnd = XMFLOAT3(0.0f, 0.0f, 0.0f);

    // 移動関連
    slashData_.direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
    slashData_.directionRnd = XMFLOAT3(0.0f, 0.0f, 0.0f);
    slashData_.speed = 0.0f;
    slashData_.accel = 1.0f;
    slashData_.gravity = 0.0f;

    // スピン
    slashData_.spin = XMFLOAT3(0.0f, 0.0f, 0.0f);

    hEmit_[SLASH_EFFECT] = VFX::Start(slashData_);
}

void EnemyDamageEffect::Update()
{
    float dt_ = GameTime::DeltaTime();
    timer_ += dt_;
    if (timer_ >= DURATION) // エフェクトの持続時間
    {
        if (hEmit_[BLOOD_EFFECT] != -1) 
        {
            VFX::End(hEmit_[BLOOD_EFFECT]);
            hEmit_[BLOOD_EFFECT] = -1;
        }
        KillMe();
	}

    slashTimer_ += dt_;
    if (slashTimer_ >= 0.1f)
    {
		if (hEmit_[SLASH_EFFECT] != -1)
		{
			VFX::End(hEmit_[SLASH_EFFECT]);
			hEmit_[SLASH_EFFECT] = -1;
		}
    }

	data_.position.y = transform_.position_.y + HEIGHT_OFFSET; // 常に敵の体の位置
	slashData_.position.y = transform_.position_.y + HEIGHT_OFFSET; // 常に敵の体の位置
    VFX::SetEmitterPosition(hEmit_[BLOOD_EFFECT], data_.position);
	VFX::SetEmitterPosition(hEmit_[SLASH_EFFECT], slashData_.position);
}

void EnemyDamageEffect::Draw()
{
}

void EnemyDamageEffect::Release()
{
}
