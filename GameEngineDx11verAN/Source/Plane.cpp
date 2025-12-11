#include "Plane.h"
#include "../Engine/Model.h"
#include <assert.h>
#include "../Engine/BoxCollider.h"

namespace
{
	const float PLANE_ROTATE_X = 0.0f;
	const XMFLOAT3 PLANE_SCALE = { 100.0f, 3.0f, 100.0f };
}

Plane::Plane(GameObject* parent) : GameObject(parent, "plane"), planeHandle_(-1)
{
}

void Plane::Initialize()
{
	planeHandle_ = Model::Load("Models/Stage_t.fbx");
	assert(planeHandle_ >= 0);
	transform_.position_ = { 0.0f, -10.0f, 0.0f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };
	transform_.scale_ = PLANE_SCALE;
	Model::SetTransform(planeHandle_, transform_);

	// 仮壁のサイズ・位置
	wallPos_ = { 0.0f, 0.0f, 0.0f };
	wallSize_ = { 10.0f, 5.0f, 10.0f };

	wallCollider_ = new BoxCollider(wallPos_, wallSize_);
	AddCollider(wallCollider_);
	wallCollider_->SetRole(Collider::Role::Static);
}

void Plane::Update()
{
}

void Plane::Draw()
{
	Model::Draw(planeHandle_);
	wallCollider_->Draw(wallPos_, wallSize_);
}

void Plane::Release()
{
}
