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
	const float PLAYER_ROTATE_SPEED = 0.2f; // プレイヤーの回転速度
}

Player::Player(GameObject* parent)
	:GameObject(parent), hSilly(-1){
	//先端までのベクトルとして（0,1,0)を代入しておく
	//初期位置は原点
}

void Player::Initialize()
{
	hSilly = Model::Load("ship-medium.fbx");
	assert(hSilly >= 0);
	Model::SetAnimFrame(hSilly, 0, 1600, 0.2);
	transform_.position_ = { 0.0, 0.0, 0.0 };
	transform_.rotate_ = { 0.0, 180.0, 0.0 };
	Camera::SetTarget(transform_.position_);
}

void Player::Update()
{
	if(Input::IsKey(DIK_W))
	{
		float rad = XMConvertToDegrees(transform_.rotate_.y) * XM_PI / 180.0f;
		transform_.position_.x += PLAYER_SPEED * cos(rad);
		transform_.position_.z += PLAYER_SPEED * sin(rad);
	}
	if(Input::IsKey(DIK_S))
	{
		float rad = XMConvertToDegrees(transform_.rotate_.y) * XM_PI / 180.0f;
		transform_.position_.x -= PLAYER_SPEED * cos(rad);
		transform_.position_.z -= PLAYER_SPEED * sin(rad);
	}
	if(Input::IsKey(DIK_A))
	{
		transform_.rotate_.y -= PLAYER_ROTATE_SPEED;
	}
	if(Input::IsKey(DIK_D))
	{
		transform_.rotate_.y += PLAYER_ROTATE_SPEED;
	}

	// カメラの位置を更新
	Camera::SetPosition(transform_.position_.x, transform_.position_.y + 50.0f, transform_.position_.z - 50.0f);
}

void Player::Draw()
{
	transform_.scale_ = { 0.01,0.01,0.01 };
	Model::SetTransform(hSilly, transform_);
	Model::Draw(hSilly);
}


void Player::Release()
{
}
