#include "MagicSphere.h"
#include "../Engine/Model.h"
#include "../Engine/GameTime.h"
#include "../Engine/SphereCollider.h"

namespace
{
	const float MAGIC_SPEED = 100.0f;
	const float ATTACK_DURATION = 5.0f;
	const float MAGIC_SCALE = 1.2f;

	const float COLLILDER_RADIUS = 2.5f;
}

MagicSphere::MagicSphere(GameObject* parent) : GameObject(parent, "MagicSphere"), magicModel_(-1), attackTimer_(0.0f)
{
}

MagicSphere::~MagicSphere()
{
}

void MagicSphere::Initialize()
{
	//仮に箱モデルを使う
	magicModel_ = Model::Load("Models/magicsphere.fbx");
	assert(magicModel_ != -1);

	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };
	transform_.scale_ = { MAGIC_SCALE, MAGIC_SCALE, MAGIC_SCALE };

	pCollider_ = new SphereCollider(XMFLOAT3(0.0f, 0.0f, 0.0f),COLLILDER_RADIUS);
	AddCollider(pCollider_);
	pCollider_->SetRole(Collider::Role::Attack);

	// 魔法のスフィアにエフェクトを追加
	effectData_.textureFileName = "Effects/flashC_B.png";
	effectData_.position = transform_.position_;
	effectData_.positionRnd = XMFLOAT3(0.12f, 0.12f, 0.12f);
	effectData_.direction = XMFLOAT3(0, 0, 0);      // その場で揺らぐ
	effectData_.directionRnd = XMFLOAT3(0, 0, 0);
	effectData_.speed = 0.01f;
	effectData_.speedRnd = 0.1f;
	effectData_.accel = 1.0f;
	effectData_.gravity = 0.0f;
	effectData_.color = XMFLOAT4(0.85f, 0.95f, 1.0f, 0.35f);   // 少し青白
	effectData_.deltaColor = XMFLOAT4(-0.001f, -0.001f, -0.001f, -0.003f);
	effectData_.rotate = XMFLOAT3(0, 0, 0);
	effectData_.rotateRnd = XMFLOAT3(0.0f, 15.0f, 0.0f);
	effectData_.spin = XMFLOAT3(0.0f, 1.0f, 0.0f);
	effectData_.size = XMFLOAT2(1.2f, 1.2f);
	effectData_.sizeRnd = XMFLOAT2(0.2f, 0.2f);
	effectData_.scale = XMFLOAT2(1.005f, 1.005f);   // ゆっくり膨らむ
	effectData_.lifeTime = 40;
	effectData_.delay = 3;                          // 継続的に発生
	effectData_.number = 2;
	effectData_.isBillBoard = true;

	hEmit_ = VFX::Start(effectData_);
}

void MagicSphere::Update()
{
	float dt_ = GameTime::DeltaTime();
	attackTimer_ += dt_;

	float yawRad = DirectX::XMConvertToRadians(transform_.rotate_.y);
	float vx = -sinf(yawRad);
	float vz = -cosf(yawRad);

	transform_.position_.x += vx * MAGIC_SPEED * dt_;
	transform_.position_.z += vz * MAGIC_SPEED * dt_;

	//transform_.rotate_.z += 120.0f * dt_; //回転
	effectData_.position = transform_.position_;

	if (hEmit_ >= 0)
	{
		VFX::SetEmitterPosition(hEmit_, effectData_.position);
	}

	//一定時間経過で消える
	if (attackTimer_ >= ATTACK_DURATION)
	{
		VFX::End(hEmit_);
		KillMe();
	}
}

void MagicSphere::Draw()
{
	Model::SetTransform(magicModel_, transform_);
	Model::Draw(magicModel_);

//	pCollider_->Draw(transform_.position_, transform_.rotate_);
}

void MagicSphere::Release()
{
}

void MagicSphere::OnCollision(GameObject* pTarget)
{
	if (!pTarget) return;

	Collider* myCol = GetLastHitCollider();
	Collider* targetCol = pTarget->GetLastHitCollider();
	if (!myCol || !targetCol) return;

	const auto myRole = myCol->GetRole();
	const auto targetRole = targetCol->GetRole();

	const bool isAttack = (myRole == Collider::Role::Attack && targetRole == Collider::Role::Body);
	const bool isEnemy = (pTarget->GetObjectName() == "Enemy");
	const bool isWall = (targetRole == Collider::Role::Static);

	if ((isAttack && isEnemy) || isWall)
	{
		VFX::End(hEmit_);
		KillMe();
	}
}
