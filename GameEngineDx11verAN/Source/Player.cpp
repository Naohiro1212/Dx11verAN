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
	objectName_ = "Player";
}

void Player::Initialize()
{
	hSilly = Model::Load("Cube.fbx");
	assert(hSilly >= 0);
	transform_.position_ = { 0.0, 0.0, 0.0 };
	transform_.rotate_ = { 0.0, 180.0, 0.0 };
	transform_.scale_ = { 10.0f,10.0f,10.0f };
	Camera::SetTarget(transform_.position_);
}

void Player::Update()
{
}

void Player::Draw()
{
	Model::SetTransform(hSilly, transform_);
	Model::Draw(hSilly);
}


void Player::Release()
{
}
