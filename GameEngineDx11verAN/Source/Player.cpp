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
#include "../Source/Plane.h"

using namespace DirectX;

Player::Player(GameObject* parent)
    :GameObject(parent, "Player"), walkModel_(-1), runModel_(-1), 
    leftStrafeModel_(-1), rightStrafeModel_(-1), backStrafeModel_(-1), 
    idleModel_(-1), wasMoving_(false), velocityY_(0.0f), jumpCount_(0), onGround_(true), 
    nowModel_(-1), attackTimer_(0.0f), isAttacking_(false),
    prevMouseLeftDown_(false),pCollider_(nullptr), magicDir_(0.0f, 0.0f, 0.0f), cnf_(),
	attackCollider_(nullptr), wallCollider_(nullptr), lastSlashFrame_(0.0f),
	rotateCenter_(0.0f, 0.0f, 0.0f), dt_(0.0f)
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
    assert(slashModel_ != -1);
	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0, 0.0, 0.0 };
	transform_.scale_ = { cnf_.PLAYER_SCALE, cnf_.PLAYER_SCALE, cnf_.PLAYER_SCALE };
	Camera::SetTarget(transform_.position_);
    wasMoving_ = false;
	// プレイヤーの後方上位位置にカメラを設定
	Camera::SetPosition(transform_.position_.x, transform_.position_.y + cnf_.CAMERA_INIT_POS_Y, transform_.position_.z - cnf_.CAMERA_INIT_POS_Z);

    // ジャンプの初速度
    JumpV0_ = sqrtf(2.0f * cnf_.GRAVITY * cnf_.JUMP_HEIGHT);
    velocityY_ = 0.0f;

    // ジャンプ初期化
    jumpCount_ = 0;
    onGround_ = true;

    nowModel_ = idleModel_;
	plvision_.Initialize(cnf_.VISION_INIT_YAW_DEG, cnf_.VISION_INIT_PITCH_DEG, cnf_.VISION_INIT_DISTANCE);
    Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_IDLE_END, cnf_.ANIM_BASE_SPEED);

    pCollider_ = new BoxCollider(cnf_.COLLIDER_BASE_POS, cnf_.COLLIDER_SCALE);
    AddCollider(pCollider_);
    pCollider_->SetRole(Collider::Role::Body);

    // Plane経由で仮壁コライダー取得
    Plane* pPlane = (Plane*)FindObject("plane");
	assert(pPlane);
	wallCollider_ = pPlane->GetWallCollider();
}

void Player::Update()
{
    dt_ = GameTime::DeltaTime();

    // 攻撃
    // 攻撃モーション中は他の動作を行えない
    MeleeAttack();

    if (!isAttacking_)
    {
        // カメラ基準の前方・右ベクトル計算
        CalcCameraDirectionXZ();

        // 移動入力取得
        MoveInput();

        // 入力によるモデル切り替え
        // 切り替えたタイミングでアニメーションを最初から再生
        ChangeModel();
    }

    if (isMovingNow_)
    {
        // カメラ前方ベクトル（XZ）をDirectXMathで取得済みとする
        XMVECTOR vForward = XMLoadFloat3(&forward);

        // プレイヤーの現在の向き（Y軸回転）をラジアンで取得
        float currentYawRad = XMConvertToRadians(transform_.rotate_.y);

        // プレイヤーの前方ベクトル（XZ, Y=0）
        XMVECTOR vPlayerForward = XMVectorSet(-sinf(currentYawRad), 0.0f, -cosf(currentYawRad), 0.0f);

        // カメラ前方ベクトルの向きから目標ヨー角を計算
        float targetYawRad = atan2f(forward.x, forward.z); // DirectXMathにもXMVectorGetX/Yはあるが、ここはfloatでOK
        float targetYawDeg = XMConvertToDegrees(targetYawRad) + cnf_.FACE_OFFSET_DEG;

        // 差分計算
        float diff = targetYawDeg - transform_.rotate_.y;
        // -180~180 に折り返し
        while (diff > cnf_.HALF_TURN) diff -= cnf_.FULL_TURN;
        while (diff < -cnf_.HALF_TURN) diff += cnf_.FULL_TURN;

        float step = cnf_.TURN_SPEED_DEG * dt_;
        if (fabsf(diff) <= step) {
            transform_.rotate_.y = targetYawDeg;
        }
        else {
            transform_.rotate_.y += (diff > 0 ? step : -step);
        }
    }

    // カメラ相対の移動ベクトルで移動
    XMVECTOR vMove = XMVectorZero();
    if (fwd_ != 0)
    {
        vMove = XMVectorAdd(vMove, XMVectorScale(vForward, static_cast<float>(fwd_)));
    }
    if (str_ != 0)
    {
        vMove = XMVectorAdd(vMove, XMVectorScale(vRight, static_cast<float>(str_)));
    }

    // 正規化
    if (XMVector3LengthSq(vMove).m128_f32[0] > 1e-5f)
    {
        vMove = XMVector3Normalize(vMove);
    }
    XMFLOAT3 moveVec;
    XMStoreFloat3(&moveVec, vMove);
    transform_.position_.x += moveVec.x * cnf_.PLAYER_SPEED * dt_;
    transform_.position_.z += moveVec.z * cnf_.PLAYER_SPEED * dt_;

    // 仮壁1枚だけ処理
    if (wallCollider_)
    {
		PenetrationResult res = Collider::ComputeBoxVsBoxPenetration(pCollider_, wallCollider_);
        if (res.overlapped)
        {
            transform_.position_.x += res.push.x + (res.push.x > 0 ? cnf_.WALL_EPS : (res.push.x < 0 ? -cnf_.WALL_EPS : 0.0f));
            transform_.position_.z += res.push.z + (res.push.z > 0 ? cnf_.WALL_EPS : (res.push.z < 0 ? -cnf_.WALL_EPS : 0.0f));
            if (fabsf(res.normal.y) < 0.4f)
            {
                moveVec = SlideAlongWall(moveVec, res.normal);
            }
        }
    }

    // 入力状態を保存（エッジ検出用）
    wasMoving_ = isMovingNow_;

    // ジャンプ
    Jump();

    // 重力更新
    UpdateGravity();

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

    // 右クリックで魔法攻撃
    ShootMagic();

    // カメラ更新
    plvision_.Update(transform_.position_);
}

void Player::Draw()
{
    // 現在のモデルを描画
	Model::SetTransform(nowModel_, transform_);
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
    if (!pTarget) return;
    Collider* src = GetLastHitCollider();

    // ここに攻撃ヒット時の処理を書く
    // 例: pTarget->KillMe(); や ヒットエフェクト、ダメージ適用など

    // 宝石取得時
	if (pTarget->GetObjectName() == "Jewel" && !pTarget->IsDead())
	{
        transform_.scale_.x += 0.02f;
		transform_.scale_.y += 0.02f;
		transform_.scale_.z += 0.02f;
	}
}

void Player::MoveInput()
{
    // 入力を +1/0/-1 に畳む（カメラ相対移動: W/S=前後, A/D=ストレーフ）
    fwd_ = 0;
    str_ = 0;
    if (!isAttacking_)
    {
        if (Input::IsKey(DIK_W)) {
            fwd_ += 1;
        }
        if (Input::IsKey(DIK_S)) {
            fwd_ -= 1;
        }
        if (Input::IsKey(DIK_D)) {
            str_ += 1;
        }
        if (Input::IsKey(DIK_A)) {
            str_ -= 1;
        }
    }

    isMovingNow_ = false;
    if (fwd_ != 0 || str_ != 0) {
        isMovingNow_ = true;
    }
}

void Player::ChangeModel()
{
    if (isAttacking_) return;

    int prevModel = nowModel_;
    int targetModel = nowModel_;

    if (fwd_ > 0) {
        if (str_ > 0) {
            targetModel = rightStrafeModel_;
        }
        else if (str_ < 0) {
            targetModel = leftStrafeModel_;
        }
        else {
            targetModel = walkModel_;
        }
    }
    else if (fwd_ < 0) {
        targetModel = backStrafeModel_;
    }
    else {
        if (str_ > 0) {
            targetModel = rightStrafeModel_;
        }
        else if (str_ < 0) {
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
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_STRAFE_END, cnf_.ANIM_BASE_SPEED);
        }
        else if (nowModel_ == leftStrafeModel_) {
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_STRAFE_END, cnf_.ANIM_BASE_SPEED);
        }
        else if (nowModel_ == walkModel_) {
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_WALK_END, cnf_.ANIM_BASE_SPEED);
        }
        else if (nowModel_ == backStrafeModel_) {
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_BACK_END, cnf_.ANIM_BASE_SPEED);
        }
        else if (nowModel_ == idleModel_) {
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_IDLE_END, cnf_.ANIM_BASE_SPEED);
        }
    }
}

void Player::UpdateGravity()
{
    float g = cnf_.GRAVITY;
    if (velocityY_ < 0.0f) {
        g *= cnf_.GRAVITY_MULTIPLIER;
    }

    // 地面に接地していて下向きの速度なら、まず速度をゼロクリアして自己貫通を防ぐ
    if (onGround_ && velocityY_ <= 0.0f) {
        velocityY_ = 0.0f;
    }

    float nextVelY = velocityY_ + (-g) * dt_;
    float nextY = transform_.position_.y + velocityY_ * dt_ + (-g) * 0.5f * dt_ * dt_;

    // 2) レイを現在位置（または probe 上方）から下向きに飛ばして
    //    今フレームの移動範囲内に床があるかを判定する
    Plane* pPlane = (Plane*)FindObject("plane");
    assert(pPlane != nullptr);
    int hPlaneModel = pPlane->GetPlaneHandle();

    RayCastData hitData;
    // レイの開始は現在の transform_.position_ の上に取る
    hitData.start = transform_.position_;
    hitData.start.y += cnf_.PROBE_UP_OFFSET;
    hitData.dir = { 0.0f, -1.0f, 0.0f };
    Model::RayCastWorld(hPlaneModel, &hitData);

    const float EPS = 1e-3f;
    const float ENTER_EPS = cnf_.GROUND_EPS;          // 例: 0.02f
    const float EXIT_EPS = cnf_.GROUND_EPS * 2.0f;   // 例: 0.04f

    bool willGroundThisFrame = false;
    float groundY = -INFINITY;

    if (hitData.hit) {
        groundY = hitData.hitPos.y; // ← RayCastWorld が hitPos を返す前提。無ければ start.y - dist でもOK
        float maxTravel = (std::max)(0.0f, hitData.start.y - nextY);

        // 既に接地しているなら緩めの閾値、未接地なら厳しめ
        float threshold = onGround_ ? EXIT_EPS : ENTER_EPS;

        if (hitData.dist <= maxTravel + EPS && nextY <= groundY + threshold) {
            willGroundThisFrame = true;
        }
    }

    if (willGroundThisFrame) {
        transform_.position_.y = groundY; // スナップ固定
        velocityY_ = 0.0f;
        onGround_ = true;
        jumpCount_ = 0;
    }
    else {
        transform_.position_.y = nextY;
        velocityY_ = nextVelY;

        // 離地は「上限＋EXIT_EPS」を超えた場合のみ false にする（微小誤差で揺れない）
        if (groundY != -INFINITY)
        {
            onGround_ = (transform_.position_.y <= groundY + EXIT_EPS);
        }
        else {
            onGround_ = false;
        }
    }
}

void Player::ShootMagic()
{
    // 右クリックで魔法発射
    if (Input::IsMouseButtonDown(1) && onGround_)
    {
        // 魔法弾生成
        XMFLOAT3 spawnPos = transform_.position_;
        MagicSphere* sphere = Instantiate<MagicSphere>(GetParent());
        sphere->SetPosition(
            spawnPos.x + magicDir_.x * transform_.scale_.z * cnf_.MAGIC_SPHERE_SPAWN_OFFSET.x,
            spawnPos.y + transform_.scale_.y * cnf_.MAGIC_SPHERE_SPAWN_OFFSET.y,
            spawnPos.z + magicDir_.z * transform_.scale_.z * cnf_.MAGIC_SPHERE_SPAWN_OFFSET.z
        );
        sphere->SetRotate(XMFLOAT3(0.0f, transform_.rotate_.y, 0.0f));
    }

    // ローカル基準オフセット（元に使っていた値）
    float forwardDist = transform_.scale_.z * cnf_.FORWARDDIST_OFFSET;
    float height = transform_.scale_.y * cnf_.HEIGHT_OFFSET;

    // 回転を反映した中心オフセット
    rotateCenter_ = XMFLOAT3(
        magicDir_.x * forwardDist,
        height,
        magicDir_.z * forwardDist
    );
}

void Player::MeleeAttack()
{
    // 攻撃モーション中は他の動作を行えない
    if (isAttacking_)
    {
        // 1周目の途中でループ（startに戻る）したら終了
        int cur = Model::GetAnimFrame(slashModel_);
        if (cur < lastSlashFrame_) // startへ巻き戻った＝ループ発生
        {
            isAttacking_ = false;

            // 攻撃用コライダー破棄
            if (attackCollider_)
            {
                RemoveCollider(attackCollider_);
                attackCollider_ = nullptr;
            }
            nowModel_ = idleModel_;
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_IDLE_END, cnf_.ANIM_BASE_SPEED);
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
        attackCollider_ = new BoxCollider(cnf_.ATTACK_COLLIDER_BASE_POS, cnf_.ATTACK_COLLIDER_SCALE);

        AddCollider(attackCollider_);
        attackCollider_->SetRole(Collider::Role::Attack);
        attackCollider_->SetCenter(rotateCenter_);

        // 移動リセット
        fwd_ = 0;
        str_ = 0;
        isMovingNow_ = false;
        isAttacking_ = true;

        nowModel_ = slashModel_;
        Model::SetAnimFrame(nowModel_, cnf_.SLASH_ANIM_START, cnf_.SLASH_ANIM_END, cnf_.SLASH_PLAY_SPEED);
        lastSlashFrame_ = cnf_.SLASH_ANIM_START; // 巻き戻り検知の基準
        Model::SetTransform(nowModel_, transform_);
        plvision_.Update(transform_.position_);
        return;
    }
}

void Player::CalcCameraDirectionXZ()
{
    // カメラ前方（XZ）を正規化
    XMFLOAT3 focus = plvision_.GetFocus();
    XMFLOAT3 camPos = plvision_.GetCameraPosition();
    forward = {
        focus.x - camPos.x,
        0.0f,
        focus.z - camPos.z
    };
    vForward = XMLoadFloat3(&forward);
    vForward = XMVector3Normalize(vForward);
    XMStoreFloat3(&forward, vForward);

    // 右ベクトル（XZ）
    XMFLOAT3 right = {
        forward.z,
        0.0f,
        -forward.x
    };
    vRight = XMLoadFloat3(&right);
    vRight = XMVector3Normalize(vRight);
    XMStoreFloat3(&right, vRight);
}

void Player::Jump()
{
    // ジャンプや重力処理
    if (Input::IsKeyDown(DIK_SPACE) && (onGround_ || jumpCount_ < cnf_.JUMP_MAX_COUNT))
    {
        velocityY_ = JumpV0_;   // 上向き初速
        onGround_ = false;
        ++jumpCount_;
    }
}

XMFLOAT3 Player::SlideAlongWall(const XMFLOAT3& f, const XMFLOAT3& n)
{
    XMVECTOR vf = XMLoadFloat3(&f);
    XMVECTOR vn = XMLoadFloat3(&n);

    // Y成分をゼロにして水平法線へ
	vn = XMVectorSet(XMVectorGetX(vn), 0.0f, XMVectorGetZ(vn), 0.0f); 
	vn = XMVector3Normalize(vn);

    float d = XMVectorGetX(XMVector3Dot(vf, vn));
    XMVECTOR vw = XMVectorSubtract(vf, XMVectorScale(vn, d));

    XMFLOAT3 w;
    XMStoreFloat3(&w, vw);
    return w;
}
