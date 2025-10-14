#include "Wall.h"
#include "../Engine/Model.h"
#include "../Engine/Camera.h"

Wall::Wall(GameObject* _parent, XMFLOAT3 _pos)
	: GameObject(_parent), hModel_(-1)
{
	transform_.position_ = { _pos };
	transform_.rotate_ = { 0,0,0 };
}

Wall::~Wall()
{
}

void Wall::Initialize()
{
	hModel_ = Model::Load("Box.fbx");
}

void Wall::Update()
{
}

void Wall::Draw()
{
	transform_.scale_ = { 1,1,1 };
	Model::SetTransform(hModel_, transform_);
	Model::Draw(hModel_);
}

void Wall::Release()
{
}
