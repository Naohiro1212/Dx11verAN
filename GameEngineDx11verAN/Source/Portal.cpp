#include "Portal.h"
#include "../Engine/Model.h"

Portal::Portal(GameObject* parent) : GameObject(parent, "Portal"), portalModel_(-1)
{
}

void Portal::Initialize()
{
	portalModel_ = Model::Load("portal.fbx");
	assert(portalModel_ != -1);
	transform_.position_ = { 0.0f, 100.0f, 0.0f };
	transform_.rotate_ = { 0.0, 0.0, 0.0 };
	transform_.scale_ = { 10.0f, 10.0f, 10.0f };
}

void Portal::Update()
{
}

void Portal::Draw()
{
	Model::SetTransform(portalModel_, transform_);
	Model::Draw(portalModel_);
}

void Portal::Release()
{
}
