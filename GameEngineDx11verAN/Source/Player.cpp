#include "Player.h"
#include "../Engine/Model.h"
#include "../Engine/Debug.h"
#include "../Engine/Input.h"
#include "TestScene.h"
#include <assert.h>
#include "../Engine/Camera.h"

namespace
{
	const float PLAYER_SPEED = 0.01f; // プレイヤーの移動速度
}

Player::Player(GameObject* parent)
	:GameObject(parent), hSilly(-1){
	//先端までのベクトルとして（0,1,0)を代入しておく
	//初期位置は原点
}

void Player::Initialize()
{
	hSilly = Model::Load("characterMedium.fbx");
	assert(hSilly >= 0);
	Model::SetAnimFrame(hSilly, 0, 1600, 0.2);
	animState_ = ANIM_STATE::Idle;
	transform_.position_ = { 0.0, 0.0, 0.0 };
	Camera::SetTarget(transform_.position_);
}

void Player::Update()
{
	if(Input::IsKey(DIK_W))
	{
		transform_.position_.z += PLAYER_SPEED;
		animState_ = ANIM_STATE::Run;
	}
	if(Input::IsKey(DIK_S))
	{
		transform_.position_.z -= PLAYER_SPEED;
		animState_ = ANIM_STATE::Run;
	}
	if(Input::IsKey(DIK_A))
	{
		transform_.position_.x -= PLAYER_SPEED;
		animState_ = ANIM_STATE::Run;
	}
	if(Input::IsKey(DIK_D))
	{
		transform_.position_.x += PLAYER_SPEED;
		animState_ = ANIM_STATE::Run;
	}
}

void Player::Draw()
{
	transform_.scale_ = { 0.10,0.10,0.10 };
	Model::SetTransform(hSilly, transform_);
	Model::Draw(hSilly);
}


void Player::Release()
{
}
