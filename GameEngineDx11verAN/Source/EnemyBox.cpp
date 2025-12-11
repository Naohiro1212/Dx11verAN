#include "EnemyBox.h"
#include "../Engine/Model.h"

EnemyBox::EnemyBox(GameObject* parent) : hSilly(-1)
{
	objectName_ = "EnemyBox";
}

void EnemyBox::Initialize()
{
	hSilly = Model::Load("Box.fbx");
	transform_.position_ = { 0.0, 0.0, 0.0 };
	transform_.rotate_ = { 0.0, 180.0, 0.0 };
	transform_.scale_ = { 10.0f,10.0f,10.0f };
}

void EnemyBox::Update()
{
}

void EnemyBox::Draw()
{
	Model::SetTransform(hSilly, transform_);
	Model::Draw(hSilly);
}

void EnemyBox::Release()
{
}
