#include "Player.h"
#include "../Engine/Model.h"
#include "../Engine/Debug.h"
#include "TestScene.h"
#include <assert.h>

Player::Player(GameObject* parent) : GameObject(parent), hSilly(-1)
{
}

Player::~Player()
{
}

void Player::Initialize()
{
	hSilly = Model::Load("GS_MotionSet.fbx");
	Model::SetAnimFrame(hSilly, 0, 1200, 1.0);
}

void Player::Update()
{
	transform_.rotate_.y += 1;
}

void Player::Draw()
{
	transform_.scale_ = { 0.05,0.05,0.05 };
	transform_.position_ = { 0, -5.0, 0 };
	Model::SetTransform(hSilly, transform_);
	Model::Draw(hSilly);
}


void Player::Release()
{
}