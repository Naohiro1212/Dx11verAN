#include "Plane.h"
#include "../Engine/Model.h"
#include <assert.h>

Plane::Plane(GameObject* parent) : GameObject(parent), planeHandle_(-1)
{
}

void Plane::Initialize()
{
	planeHandle_ = Model::Load("Models/Stage_t.fbx");
	assert(planeHandle_ >= 0);
	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 90.0f, 0.0f, 0.0f };
	transform_.scale_ = { 100.0f, 100.0f, 100.0f };
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
