#include "MagicSphere.h"
#include "../Engine/Model.h"
#include "../Engine/GameTime.h"

namespace
{
	const float MAGIC_SPEED = 10.0f;
	const float ATTACK_DURATION = 5.0f;
}

MagicSphere::MagicSphere(GameObject* parent) : GameObject(parent, "Magic"), magicModel_(-1), AttackTimer_(0.0f)
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

	transform_.scale_ = { 10.0f, 10.0f, 10.0f };
}

void MagicSphere::Update()
{
	float dt = GameTime::DeltaTime();
	AttackTimer_ += dt;
	transform_.position_.x += moveVec_.x * MAGIC_SPEED * dt;
	transform_.position_.z += moveVec_.z * MAGIC_SPEED * dt;

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
}

void MagicSphere::Release()
{
}
