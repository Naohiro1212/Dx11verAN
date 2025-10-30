#include "Player.h"
#include "../Engine/Model.h"
#include "../Engine/Debug.h"
#include "../Engine/Input.h"
#include "TestScene.h"
#include <assert.h>
#include "../Engine/Camera.h"
#include "../Engine/GameTime.h"
#include "PlayerCamera.h"
#include <cmath>
#include <algorithm>
using namespace DirectX;

namespace
{
	const float PLAYER_SPEED = 30.0f; // プレイヤーの移動速度
	const float PLAYER_ROTATE_SPEED = 30.0f; // プレイヤーの回転速度

	const float CAMERA_INIT_YAW_DEG = 180.0f;
	const float CAMERA_INIT_PITCH_DEG = 20.0f;
	const float CAMERA_INIT_DISTANCE = 20.0f;

    const float GRAVITY = 9.8f;
    const float JUMP_HEIGHT = 5.0f;
}

Player::Player(GameObject* parent)
	:GameObject(parent), hSilly(-1){
	//先端までのベクトルとして（0,1,0)を代入しておく
	//初期位置は原点
}

void Player::Initialize()
{
	hSilly = Model::Load("Box.fbx");
	assert(hSilly >= 0);
	transform_.position_ = { 0.0, 0.0, 0.0 };
	transform_.rotate_ = { 0.0, 180.0, 0.0 };
	Camera::SetTarget(transform_.position_);
    wasMoving_ = false;
	// プレイヤーの後方上位位置にカメラを設定
	Camera::SetPosition(transform_.position_.x, transform_.position_.y + 10.0f, transform_.position_.z - 20.0f);

    // ジャンプの初速度
    JumpV0_ = -sqrtf(2.0f * GRAVITY * JUMP_HEIGHT);

	plvision_.Initialize(CAMERA_INIT_YAW_DEG, CAMERA_INIT_PITCH_DEG, CAMERA_INIT_DISTANCE);
}

void Player::Update()
{
    // カメラ更新
    plvision_.Update(transform_.position_);

    // カメラ前方（XZ）を正規化
    DirectX::XMFLOAT3 focus = plvision_.GetFocus();
    DirectX::XMFLOAT3 camPos = plvision_.GetCameraPosition();
    float fx = focus.x - camPos.x;
    float fz = focus.z - camPos.z;
    float fl = std::sqrt(fx * fx + fz * fz);
    if (fl > 1e-5f) { fx /= fl; fz /= fl; }
    else { fx = 0.0f; fz = 1.0f; }

    // 右ベクトル（XZ）
    float rx = fz;
    float rz = -fx;

    // 入力を +1/0/-1 に畳む（カメラ相対移動: W/S=前後, A/D=ストレーフ）
    int fwd = 0;
    if (Input::IsKey(DIK_W)) {
        fwd += 1;
    }
    if (Input::IsKey(DIK_S)) {
        fwd -= 1;
    }

    int str = 0;
    if (Input::IsKey(DIK_D)) {
        str += 1;
    }
    if (Input::IsKey(DIK_A)) {
        str -= 1;
    }

    bool isMovingNow = false;
    if (fwd != 0 || str != 0) {
        isMovingNow = true;
    }

    // 入力が入った時・入り続けているときにだけカメラ正面へ向きを合わせる（スナップ）
    if ((isMovingNow && !wasMoving_) || (isMovingNow && wasMoving_))
    {
        float targetYawDeg = std::atan2f(fx, fz) * (180.0f / DirectX::XM_PI);
        transform_.rotate_.y = targetYawDeg; // 即一致（スムーズにしたいならここだけ補間に変更）
    }

    // カメラ相対の移動ベクトルで移動
    float mvx = fwd * fx + str * rx;
    float mvz = fwd * fz + str * rz;
    float ml = std::sqrt(mvx * mvx + mvz * mvz);
    if (ml > 1e-5f) { mvx /= ml; mvz /= ml; }

    transform_.position_.x += mvx * PLAYER_SPEED * GameTime::DeltaTime();
    transform_.position_.z += mvz * PLAYER_SPEED * GameTime::DeltaTime();

    // 入力状態を保存（エッジ検出用）
    wasMoving_ = isMovingNow;

    // ジャンプや重力処理
    if (Input::IsKeyDown(DIK_SPACE))
    {

    }
}

void Player::Draw()
{
	transform_.scale_ = { 1,1,1 };
	Model::SetTransform(hSilly, transform_);
	Model::Draw(hSilly);
}


void Player::Release()
{
}

void Player::UpdateOrbitCamera(const XMFLOAT3& _targetPos)
{

}
