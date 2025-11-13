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

    const float GRAVITY = 18.0f;
    const float JUMP_HEIGHT = 12.0f;
    const size_t JUMP_MAX_COUNT = 1;

    // カメラの逆方向にする
    const float FACE_OFFSET_DEG = 180.0f;
    // 1秒当たりのターン率
    const float TURN_SPEED_DEG = 540.0f;
}

Player::Player(GameObject* parent)
	:GameObject(parent), walkModel_(-1), runModel_(-1), leftStrafeModel_(-1), rightStrafeModel_(-1)
	, backStrafeModel_(-1), idleModel_(-1), wasMoving_(false), velocityY_(0.0f), jumpCount_(0), onGround_(true)
    , nowModel_(-1)
{
	//先端までのベクトルとして（0,1,0)を代入しておく
	//初期位置は原点
}

void Player::Initialize()
{
	walkModel_ = Model::Load("Models/walk.fbx");
    runModel_ = Model::Load("Models/run.fbx");
    leftStrafeModel_ = Model::Load("Models/leftstrafe.fbx");
    rightStrafeModel_ = Model::Load("Models/rightstrafe.fbx");
    backStrafeModel_ = Model::Load("Models/backstrafe.fbx");
	idleModel_ = Model::Load("Models/idle.fbx");
	swordModel_ = Model::Load("Models/sword.fbx");
	assert(walkModel_ != -1);
    assert(runModel_ != -1);
    assert(leftStrafeModel_ != -1);
    assert(rightStrafeModel_ != -1);
    assert(backStrafeModel_ != -1);
    assert(idleModel_ != -1);
    assert(swordModel_ != -1);
	transform_.position_ = { 0.0, 0.0, 0.0 };
	transform_.rotate_ = { 0.0, 0.0, 0.0 };
    transform_.scale_ = { 0.1f, 0.1f, 0.1f };
	weaponTransform_.position_ = { 0.0f, 0.0f, 0.0f };
	weaponTransform_.rotate_ = { 0.0f, 0.0f, 0.0f };
	weaponTransform_.scale_ = { 3.0f, 3.0f, 3.0f };
	Camera::SetTarget(transform_.position_);
    wasMoving_ = false;
	// プレイヤーの後方上位位置にカメラを設定
	Camera::SetPosition(transform_.position_.x, transform_.position_.y + 10.0f, transform_.position_.z - 20.0f);

    // ジャンプの初速度
    JumpV0_ = sqrtf(2.0f * GRAVITY * JUMP_HEIGHT);
    velocityY_ = 0.0f;

    // ジャンプ初期化
    jumpCount_ = 0;
    onGround_ = true;

    nowModel_ = idleModel_;
	plvision_.Initialize(CAMERA_INIT_YAW_DEG, CAMERA_INIT_PITCH_DEG, CAMERA_INIT_DISTANCE);
    Model::SetAnimFrame(nowModel_, 1, 76, 0.5f);

    SetweaponToRightHand();
}

void Player::Update()
{
    float dt_ = GameTime::DeltaTime();

    // カメラ前方（XZ）を正規化
    XMFLOAT3 focus = plvision_.GetFocus();
    XMFLOAT3 camPos = plvision_.GetCameraPosition();
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

    // 入力によるモデル切り替え
    // 切り替えたタイミングでアニメーションを最初から再生
    int prevModel = nowModel_;
    int targetModel = nowModel_;

    if (fwd > 0) {
        if (str > 0) {
            targetModel = rightStrafeModel_;
        }
        else if (str < 0) {
            targetModel = leftStrafeModel_;
        }
        else {
            targetModel = walkModel_;
        }
    }
    else if (fwd < 0) {
        targetModel = backStrafeModel_;
    }
    else {
        if (str > 0) {
            targetModel = rightStrafeModel_;
        }
        else if (str < 0) {
            targetModel = leftStrafeModel_;
        }
        else {
            targetModel = idleModel_;
        }
    }

    if (prevModel != targetModel) {
        nowModel_ = targetModel;
        if (nowModel_ == rightStrafeModel_) {
            Model::SetAnimFrame(nowModel_, 1, 21, 0.5f);
        }
        else if (nowModel_ == leftStrafeModel_) {
            Model::SetAnimFrame(nowModel_, 1, 21, 0.5f);
        }
        else if (nowModel_ == walkModel_) {
            Model::SetAnimFrame(nowModel_, 1, 32, 0.5f);
        }
        else if (nowModel_ == backStrafeModel_) {
            Model::SetAnimFrame(nowModel_, 1, 17, 0.5f);
        }
        else if (nowModel_ == idleModel_) {
            Model::SetAnimFrame(nowModel_, 1, 76, 0.5f);
        }
    }

    // 入力が入った時・入り続けているときにだけカメラ正面へ向きを合わせる（スナップ）
    if ((isMovingNow && !wasMoving_) || (isMovingNow && wasMoving_))
    {
		float targetYawDeg = std::atan2f(fx, fz) * (180.0f / XM_PI) + FACE_OFFSET_DEG;
        float diff = targetYawDeg - transform_.rotate_.y;
        // -180~180 に折り返し
        while (diff > 180.0f) diff -= 360.0f;
        while (diff < -180.0f) diff += 360.0f;
        float step = TURN_SPEED_DEG * dt_;
        if (std::fabs(diff) <= step) {
            transform_.rotate_.y = targetYawDeg;
        }
        else {
            transform_.rotate_.y += (diff > 0 ? step : -step);
        }
    }

    // カメラ相対の移動ベクトルで移動
    float mvx = fwd * fx + str * rx;
    float mvz = fwd * fz + str * rz;
    float ml = std::sqrt(mvx * mvx + mvz * mvz);
    if (ml > 1e-5f) { mvx /= ml; mvz /= ml; }

    transform_.position_.x += mvx * PLAYER_SPEED * dt_;
    transform_.position_.z += mvz * PLAYER_SPEED * dt_;

    // 入力状態を保存（エッジ検出用）
    wasMoving_ = isMovingNow;

    // ジャンプや重力処理
    if (Input::IsKeyDown(DIK_SPACE) && (onGround_ || jumpCount_ < JUMP_MAX_COUNT))
    {
        velocityY_ = JumpV0_;   // 上向き初速
        onGround_ = false;
        ++jumpCount_;
    }

    float gravity_ = GRAVITY;
	// 落下中は速度を加算
    if (velocityY_ < 0.0f)
    {
        gravity_ *= 2.0f;
    }

    // 重力加速度による速度変化
    transform_.position_.y += velocityY_ * dt_ + (-gravity_) * 0.5f * dt_ * dt_;
    velocityY_ += (-gravity_) * dt_;

    // 地面との当たり判定（Y=0）
    if (transform_.position_.y <= 0.0f)
    {
        transform_.position_.y = 0.0f;
        // 接地時、下降中のみ速度をクリア
        if (velocityY_ < 0.0f) velocityY_ = 0.0f;
        onGround_ = true;
        jumpCount_ = 0;
    }
    else
    {
        onGround_ = false;
    }

    // モデルのワールド行列更新
    Model::SetTransform(nowModel_, transform_);

    SetweaponToRightHand();

    // カメラ更新
    plvision_.Update(transform_.position_);
}

void Player::Draw()
{
    // 現在のモデルを描画
	Model::Draw(nowModel_);
    Model::Draw(swordModel_);
}


void Player::Release()
{
}

void Player::SetweaponToRightHand()
{
    weaponTransform_.position_ = Model::GetAnimBonePosition(nowModel_, "mixamorig:RightHand");
    weaponTransform_.rotate_ = { transform_.rotate_.x + 45.0f, transform_.rotate_.y, transform_.rotate_.z + 90.0f };
	Model::SetTransform(swordModel_, weaponTransform_);
}