#include "Jewel.h"
#include "../Engine/Model.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/GameTime.h"

Jewel::Jewel(GameObject* parent) :GameObject(parent, "Jewel"), modelHandle_(-1), pCollider_(nullptr)
{
}

Jewel::~Jewel()
{
}

void Jewel::Initialize()
{
	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.scale_ = { 0.5f, 0.5f, 0.5f };

	// 仮に箱モデルを使う
	modelHandle_ = Model::Load("Models/Box.fbx");
	assert(modelHandle_ != -1);
	
	pCollider_ = new SphereCollider(transform_.position_, 3.0f);
	AddCollider(pCollider_);
	pCollider_->SetRole(Collider::Role::Static);
}

void Jewel::Update()
{
	float dt_ = GameTime::DeltaTime();
	transform_.rotate_.y += 30.0f * dt_;

	// Playerオブジェクト取得
	GameObject* pPlayer_ = FindObject("Player");
	if (!pPlayer_) return;

	// Playerの位置取得
	XMFLOAT3 playerPos_ = pPlayer_->GetPosition();

	// Jewelの現在位置
	XMFLOAT3 jewelPos_ = transform_.position_;

	// Playerへの方向ベクトル
	XMFLOAT3 dir_ =
	{
		playerPos_.x - jewelPos_.x,
		playerPos_.y - jewelPos_.y,
		playerPos_.z - jewelPos_.z
	};

	// 距離計算
	float len_ = sqrtf(dir_.x * dir_.x + dir_.y * dir_.y + dir_.z * dir_.z);
	if (1e-3f < len_ && len_ < 30.0f)
	{
		// 正規化
		dir_.x /= len_;
		dir_.y /= len_;
		dir_.z /= len_;

		// 速度をかけて計算
		float baseSpeed_ = 50.0f; // 速度
		float speed_ = baseSpeed_ * (5.0f / len_); // 距離が近いほど速く
		transform_.position_.x += dir_.x * speed_ * dt_;
		transform_.position_.y += dir_.y * speed_ * dt_;
		transform_.position_.z += dir_.z * speed_ * dt_;
	}
}

void Jewel::Draw()
{
	Model::SetTransform(modelHandle_, transform_);
	Model::Draw(modelHandle_);
//	pCollider_->Draw(transform_.position_, transform_.rotate_);
}

void Jewel::Release()
{
}

void Jewel::OnCollision(GameObject* pTarget)
{
	if (!pTarget) return;

	if (pTarget->GetObjectName() == "Player")
	{
		// 宝石を取得したときの処理をここに書く
		KillMe();
	}
}
