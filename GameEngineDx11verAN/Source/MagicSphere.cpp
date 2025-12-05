#include "MagicSphere.h"
#include "../Engine/Model.h"
#include "../Engine/GameTime.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/BoxCollider.h"

namespace
{
	const float MAGIC_SPEED = 30.0f;
	const float ATTACK_DURATION = 5.0f;
}

MagicSphere::MagicSphere(GameObject* parent) : GameObject(parent, "MagicSphere"), magicModel_(-1), AttackTimer_(0.0f)
{
}

MagicSphere::~MagicSphere()
{
}

void MagicSphere::Initialize()
{
	//‰¼‚É” ƒ‚ƒfƒ‹‚ðŽg‚¤
	magicModel_ = Model::Load("Models/Box.fbx");
	assert(magicModel_ != -1);

	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };
	transform_.scale_ = { 1.0f, 1.0f, 1.0f };

	pCollider_ = new SphereCollider(XMFLOAT3(0.0f, 0.0f, 0.0f), 2.0f);
	pCollider_->SetRole(Collider::Role::Attack);
	AddCollider(pCollider_);
}

void MagicSphere::Update()
{
	float dt_ = GameTime::DeltaTime();
	AttackTimer_ += dt_;

	float yawRad = DirectX::XMConvertToRadians(transform_.rotate_.y);
	float vx = -sinf(yawRad);
	float vz = -cosf(yawRad);

	transform_.position_.x += vx * MAGIC_SPEED * dt_;
	transform_.position_.z += vz * MAGIC_SPEED * dt_;

	//transform_.rotate_.z += 120.0f * dt_; //‰ñ“]

	//ˆê’èŽžŠÔŒo‰ß‚ÅÁ‚¦‚é
	if (AttackTimer_ >= ATTACK_DURATION)
	{
		KillMe();
	}
}

void MagicSphere::Draw()
{
	Model::SetTransform(magicModel_, transform_);
	Model::Draw(magicModel_);

	pCollider_->Draw(transform_.position_, transform_.rotate_);
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
	const bool isEnemy = (pTarget->GetObjectName() == "testEnemy");

	if (isAttack && isEnemy)
	{
		KillMe();
	}
}
