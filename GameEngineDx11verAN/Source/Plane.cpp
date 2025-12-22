#include "Plane.h"
#include "../Engine/Model.h"
#include <assert.h>
namespace
{
	const XMFLOAT3 PLANE_SCALE = { 700.0f, 3.0f, 700.0f };
	const XMFLOAT3 PLANE_POSITION = { 0.0f, -5.0f, 0.0f };
}

Plane::Plane(GameObject* parent) : GameObject(parent, "plane"), planeHandle_(-1)
{
}

void Plane::Initialize()
{
	planeHandle_ = Model::Load("floor.fbx");
	assert(planeHandle_ >= 0);
	transform_.position_ = PLANE_POSITION;
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };
	transform_.scale_ = PLANE_SCALE;
	Model::SetTransform(planeHandle_, transform_);
}

void Plane::Update()
{
}

void Plane::Draw()
{
	Model::Draw(planeHandle_);
}

void Plane::Release()
{
}
