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
#include "../Engine/BoxCollider.h"
#include "../Source/MagicSphere.h"

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

	// スラッシュアニメーション関連
    const int   SLASH_ANIM_START = 1;
    const int   SLASH_ANIM_END = 45;
    const float BASE_ANIM_FPS = 30.0f;     // モーションが想定する基準FPS（仮定）
    const float SLASH_PLAY_SPEED = 0.65f;      // 既存指定の再生スピード
    // 実時間 = (フレーム数 / FPS) / 再生スピード
    const float SLASH_DURATION_SEC = (SLASH_ANIM_END - SLASH_ANIM_START + 1) / BASE_ANIM_FPS / SLASH_PLAY_SPEED;

}

Player::Player(GameObject* parent)
    :GameObject(parent, "Player"), walkModel_(-1), runModel_(-1), leftStrafeModel_(-1), rightStrafeModel_(-1)
    , backStrafeModel_(-1), idleModel_(-1), wasMoving_(false), velocityY_(0.0f), jumpCount_(0), onGround_(true)
	, nowModel_(-1), attackTimer_(0.0f), isAttacking_(false), prevMouseLeftDown_(false),pCollider_(nullptr)
	, magicDir_(0.0f, 0.0f, 0.0f)
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
    slashModel_ = Model::Load("Models/slash.fbx");

	assert(walkModel_ != -1);
    assert(runModel_ != -1);
    assert(leftStrafeModel_ != -1);
    assert(rightStrafeModel_ != -1);
    assert(backStrafeModel_ != -1);
    assert(idleModel_ != -1);
	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0, 0.0, 0.0 };
    transform_.scale_ = { 0.1f, 0.1f, 0.1f };
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

    pCollider_ = new BoxCollider(XMFLOAT3(0.0f,10.0f,0.0f), XMFLOAT3(transform_.scale_.x * 40.0f, transform_.scale_.y * 170.0f, transform_.scale_.z * 40.0f));
    AddCollider(pCollider_);
    pCollider_->SetRole(Collider::Role::Body);
}

void Player::Update()
{
    float dt_ = GameTime::DeltaTime();

    // 攻撃モーション中は他の動作を行えない
    if (isAttacking_)
    {
        // 1周目の途中でループ（startに戻る）したら終了
        int cur = Model::GetAnimFrame(slashModel_);
        if (cur < lastSlashFrame_) // startへ巻き戻った＝ループ発生
        {
            isAttacking_ = false;

            // 攻撃用コライダー破棄
            if(attackCollider_)
            {
                RemoveCollider(attackCollider_);
                attackCollider_ = nullptr;
            }
            nowModel_ = idleModel_;
            Model::SetAnimFrame(nowModel_, 1, 76, 0.5f);
        }
        else
        {
            lastSlashFrame_ = cur;
        }


        Model::SetTransform(nowModel_, transform_);
        plvision_.Update(transform_.position_);
        return;
    }

    // 攻撃開始（開始時だけセット）
    if (Input::IsMouseButtonDown(0) && onGround_)
    {
        attackCollider_ = new BoxCollider(
            XMFLOAT3(0.0f, transform_.scale_.y * 100.0f, transform_.scale_.z * 50.0f),
            XMFLOAT3(transform_.scale_.x * 60.0f,
                     transform_.scale_.y * 100.0f, 
                     transform_.scale_.z * 100.0f)
        );

        AddCollider(attackCollider_);
        attackCollider_->SetRole(Collider::Role::Attack);
        attackCollider_->SetCenter(rotateCenter_);

        isAttacking_ = true;
        nowModel_ = slashModel_;
        Model::SetAnimFrame(nowModel_, SLASH_ANIM_START, SLASH_ANIM_END, SLASH_PLAY_SPEED);
        lastSlashFrame_ = SLASH_ANIM_START; // 巻き戻り検知の基準
        Model::SetTransform(nowModel_, transform_);
        plvision_.Update(transform_.position_);
        return;
    }

    // カメラ前方（XZ）を正規化
    XMFLOAT3 focus = plvision_.GetFocus();
    XMFLOAT3 camPos = plvision_.GetCameraPosition();
    XMFLOAT3 forward = {
        focus.x - camPos.x,
        0.0f,
        focus.z - camPos.z
    };
    XMVECTOR vForward = XMLoadFloat3(&forward);
    vForward = XMVector3Normalize(vForward);
    XMStoreFloat3(&forward, vForward);

    // 右ベクトル（XZ）
    XMFLOAT3 right = {
        forward.z,
        0.0f,
        -forward.x
    };
    XMVECTOR vRight = XMLoadFloat3(&right);
    vRight = XMVector3Normalize(vRight);
    XMStoreFloat3(&right, vRight);

    // 入力を +1/0/-1 に畳む（カメラ相対移動: W/S=前後, A/D=ストレーフ）
    int fwd = 0;
    int str = 0;
    if (!isAttacking_)
    {
        if (Input::IsKey(DIK_W)) {
            fwd += 1;
        }
        if (Input::IsKey(DIK_S)) {
            fwd -= 1;
        }
        if (Input::IsKey(DIK_D)) {
            str += 1;
        }
        if (Input::IsKey(DIK_A)) {
            str -= 1;
        }
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

    // 移動のアニメーション
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

    if (isMovingNow)
    {
        // カメラ前方ベクトル（XZ）をDirectXMathで取得済みとする
        XMVECTOR vForward = XMLoadFloat3(&forward);

        // プレイヤーの現在の向き（Y軸回転）をラジアンで取得
        float currentYawRad = XMConvertToRadians(transform_.rotate_.y);

        // プレイヤーの前方ベクトル（XZ, Y=0）
        XMVECTOR vPlayerForward = XMVectorSet(-sinf(currentYawRad), 0.0f, -cosf(currentYawRad), 0.0f);

        // カメラ前方ベクトルの向きから目標ヨー角を計算
        float targetYawRad = atan2f(forward.x, forward.z); // DirectXMathにもXMVectorGetX/Yはあるが、ここはfloatでOK
        float targetYawDeg = XMConvertToDegrees(targetYawRad) + FACE_OFFSET_DEG;

        // 差分計算
        float diff = targetYawDeg - transform_.rotate_.y;
        // -180~180 に折り返し
        while (diff > 180.0f) diff -= 360.0f;
        while (diff < -180.0f) diff += 360.0f;

        float step = TURN_SPEED_DEG * dt_;
        if (fabsf(diff) <= step) {
            transform_.rotate_.y = targetYawDeg;
        }
        else {
            transform_.rotate_.y += (diff > 0 ? step : -step);
        }
    }

    // カメラ相対の移動ベクトルで移動
    XMVECTOR vMove = XMVectorZero();
    if (fwd != 0)
    {
        vMove = XMVectorAdd(vMove, XMVectorScale(vForward, static_cast<float>(fwd)));
    }
    if (str != 0)
    {
        vMove = XMVectorAdd(vMove, XMVectorScale(vRight, static_cast<float>(str)));
    }

    // 正規化
    if (XMVector3LengthSq(vMove).m128_f32[0] > 1e-5f)
    {
        vMove = XMVector3Normalize(vMove);
    }
    XMFLOAT3 moveVec;
    XMStoreFloat3(&moveVec, vMove);

    transform_.position_.x += moveVec.x * PLAYER_SPEED * dt_;
    transform_.position_.z += moveVec.z * PLAYER_SPEED * dt_;

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

    // 攻撃用の方向ベクトル計算(近接・魔法攻撃どちらも対応)
    float yawRad = XMConvertToRadians(transform_.rotate_.y);
    // 正面方向ベクトル（XZ平面、Yは0でOK）
    magicDir_ = {
        -sinf(yawRad), // X
        0.0f,          // Y
        -cosf(yawRad)  // Z
    };

    // 右クリックで魔法発射
	if (Input::IsMouseButtonDown(1) && onGround_)
	{
		// 魔法弾生成
		XMFLOAT3 spawnPos = transform_.position_;
		MagicSphere* sphere = Instantiate<MagicSphere>(GetRootJob());
		sphere->SetPosition(
			spawnPos.x + magicDir_.x * transform_.scale_.z * 60.0f,
			spawnPos.y + transform_.scale_.y * 100.0f,
			spawnPos.z + magicDir_.z * transform_.scale_.z * 60.0f
		);
		sphere->SetRotate(XMFLOAT3(0.0f, transform_.rotate_.y, 0.0f));
	}

    // ローカル基準オフセット（元に使っていた値）
    float forwardDist = transform_.scale_.z * 50.0f;
    float height = transform_.scale_.y * 100.0f;

    // 回転を反映した中心オフセット
	rotateCenter_ = XMFLOAT3(
		magicDir_.x * forwardDist,
		height,
		magicDir_.z * forwardDist
	);

    // カメラ更新
    plvision_.Update(transform_.position_);
}

void Player::Draw()
{
    // 現在のモデルを描画
	Model::Draw(nowModel_);
    pCollider_->Draw(transform_.position_, transform_.rotate_);

    if (attackCollider_)
    {
        attackCollider_->Draw(transform_.position_, transform_.rotate_);
    }
}

void Player::Release()
{
}

void Player::OnCollision(GameObject* pTarget)
{
    // 直近の衝突元が攻撃コライダー以外なら無視
    Collider* src = GetLastHitCollider();
    if (!src || src != attackCollider_) return;

    // ここに攻撃ヒット時の処理を書く
    // 例: pTarget->KillMe(); や ヒットエフェクト、ダメージ適用など
}