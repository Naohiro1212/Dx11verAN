#include "Sword.h"
#include "../Engine/Model.h"

Sword::Sword(GameObject* parent) : GameObject(parent), swordModel_(-1)
{
}

void Sword::Initialize()
{
	swordModel_ = Model::Load("Models/sword.fbx");
	assert(swordModel_ != -1);
	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };
	transform_.scale_ = { 40.0f, 40.0f, 40.0f };
}

void Sword::Update()
{
	Model::SetTransform(swordModel_, transform_);
}

void Sword::Draw()
{
	Model::Draw(swordModel_);
}

void Sword::Release()
{
}
