#include "Portal.h"
#include "../Engine/Model.h"

Portal::Portal(GameObject* parent) : GameObject(parent, "Portal"), portalSphereModel_(-1), portalStandModel_(-1)
{
}

void Portal::Initialize()
{
	portalSphereModel_ = Model::Load("PortalSphere.fbx");
	portalStandModel_ = Model::Load("PortalStand.fbx");
	assert(portalSphereModel_ != -1);
	assert(portalStandModel_ != -1);
	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 270.0f, 0.0f, 0.0f };
	transform_.scale_ = { 0.07f, 0.07f, 0.07f };

	standTransform_.position_ = transform_.position_;
	standTransform_.rotate_ = { 270.0f, 0.0f, 0.0f };
	standTransform_.scale_ = { 0.15f, 0.15f, 0.15f };

	isActive_ = false;
}

void Portal::Update()
{
	if (isActive_)
	{
		ActivatePortal();
	}
}

void Portal::Draw()
{
	transform_.position_.y = 14.0f; // 地面から少し浮かせる
	Model::SetTransform(portalSphereModel_, transform_);
	Model::Draw(portalSphereModel_);

	// 台座の座標設定
	standTransform_.position_.x = transform_.position_.x;
	standTransform_.position_.y = 6.0f; // 台座は地面に設置
	standTransform_.position_.z = transform_.position_.z;
	Model::SetTransform(portalStandModel_, standTransform_);
	Model::Draw(portalStandModel_);
}

void Portal::Release()
{
}

void Portal::ActivatePortal()
{
	transform_.rotate_.y += 1.0f;
	if (transform_.rotate_.y >= 360.0f)
	{
		transform_.rotate_.y = 0.0f;
	}
}
