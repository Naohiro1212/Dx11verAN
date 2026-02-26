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
#include "../Source/DungeonManager.h"
#include "PopUpDamage.h"
#include "PopUpLevelUp.h"
#include "../Engine/RandomNum.h"
#include "../Source/LevelUpEffect.h"
#include "../Engine/Audio.h"
#include "../Source/testEnemy.h"
#include "../Engine/BillBoard.h"
#include "PlayerMovement.h"

using namespace DirectX;

Player::Player(GameObject* parent) : GameObject(parent, "Player"), 
    dt_(0.0f),
    hitSEHandle_(-1),
    jumpSEHandle_(-1),
    levelUpEffect_(nullptr),
	levelUpSEHandle_(-1),
	moveSEHandle_(-1),
    ongroundSEHandle_(-1),
    pPlane_(nullptr),
    plvision_(),
    shadowBillboard_(nullptr),
	shootSEHandle_(-1),
	strafeSEHandle_(-1),
	swingSEHandle_(-1),
	movement_()
{
	//先端までのベクトルとして（0,1,0)を代入しておく
	//初期位置は原点
	wallColliders_.clear();
}

void Player::Initialize()
{
    // アニメーション読み込み
	walkModel_ = Model::Load("Models/walk.fbx");
    runModel_ = Model::Load("Models/run.fbx");
    leftStrafeModel_ = Model::Load("Models/leftstrafe.fbx");
    rightStrafeModel_ = Model::Load("Models/rightstrafe.fbx");
    backStrafeModel_ = Model::Load("Models/backstrafe.fbx");
	idleModel_ = Model::Load("Models/idle.fbx");
    slashModel_ = Model::Load("Models/slash.fbx");
	jumpModel_ = Model::Load("Models/jump.fbx");
	deathModel_ = Model::Load("Models/death.fbx");

	assert(walkModel_ != -1);
    assert(runModel_ != -1);
    assert(leftStrafeModel_ != -1);
    assert(rightStrafeModel_ != -1);
    assert(backStrafeModel_ != -1);
    assert(idleModel_ != -1);
    assert(slashModel_ != -1);
    assert(jumpModel_ != -1);
	assert(deathModel_ != -1);

    // サウンド読み込み
    // ヒット音のみ2つ用意し、ヒットしたら切り替え
	hitSEHandle_ = Audio::Load("Audio/hitsound.wav");
	swingSEHandle_ = Audio::Load("Audio/slash.wav");
	moveSEHandle_ = Audio::Load("Audio/move.wav", true,1);
	strafeSEHandle_ = Audio::Load("Audio/strafe.wav", true,1);
	shootSEHandle_ = Audio::Load("Audio/shootmagic.wav", false, 15);
	jumpSEHandle_ = Audio::Load("Audio/jump.wav");
    ongroundSEHandle_ = Audio::Load("Audio/onGround.wav");
	//levelUpSEHandle_ = Audio::Load("Audio/levelup.wav");

    Audio::SetMasterVolume(cnf_.MASTER_VOLUME);

	assert(hitSEHandle_ != -1);
    assert(swingSEHandle_ != -1);
	assert(moveSEHandle_ != -1);
	assert(strafeSEHandle_ != -1);
	assert(shootSEHandle_ != -1);
    assert(jumpSEHandle_ != -1);
	assert(ongroundSEHandle_ != -1);

	// 初期位置・スケール設定
	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0, 0.0, 0.0 };
	transform_.scale_ = { cnf_.PLAYER_SCALE, cnf_.PLAYER_SCALE, cnf_.PLAYER_SCALE };
	Camera::SetTarget(transform_.position_);
    wasMoving_ = false;
	// プレイヤーの後方上位位置にカメラを設定
	Camera::SetPosition(transform_.position_.x, transform_.position_.y + cnf_.CAMERA_INIT_POS_Y, transform_.position_.z - cnf_.CAMERA_INIT_POS_Z);

    nowModel_ = idleModel_;
	plvision_.Initialize(cnf_.VISION_INIT_YAW_DEG, cnf_.VISION_INIT_PITCH_DEG, cnf_.VISION_INIT_DISTANCE);
    Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_IDLE_END, cnf_.ANIM_BASE_SPEED);

    pCollider_ = new BoxCollider(
        cnf_.COLLIDER_BASE_POS,
        cnf_.COLLIDER_SCALE);
    AddCollider(pCollider_);
    pCollider_->SetRole(Collider::Role::Body);

    // 経験値リセット
    exp_ = 0.0f;

    // マナ初期化
    mana_ = cnf_.MAX_MANA;

	// 体力初期化
	health_ = cnf_.MAX_HEALTH;

	// ダメージを受けたときの無敵時間用タイマー初期化
    damageCooldown_ = 0.0f;

    // プレイヤーの下の丸影
    shadowBillboard_ = new BillBoard();
    shadowBillboard_->Load("circle_B.png");
	assert(shadowBillboard_ != nullptr);

	// Planeオブジェクトの取得
    pPlane_ = static_cast<Plane*>(FindObject("plane"));
    assert(pPlane_ != nullptr);

	// playermovementクラスの初期化
	movement_ = new PlayerMovement(cnf_, transform_, pPlane_, pCollider_);
    movement_->Initialize();
}

void Player::Update()
{
    // デルタタイム取得
    dt_ = GameTime::DeltaTime();

    // 1) 死亡を最優先（ここで初期化は1回だけ）
    if (health_ <= 0.0f)
    {
        if (!isDead_)
        {
            isDead_ = true;
            deathTimer_ = 0.0f;
            deathAnimStopped_ = false;

            // 攻撃の後始末（任意）
            isAttacking_ = false;
            if (attackCollider_) { RemoveCollider(attackCollider_); attackCollider_ = nullptr; }

            nowModel_ = deathModel_;
            // 初期化はこの1回だけ
            Model::SetAnimFrame(
                nowModel_,
                cnf_.ANIM_BASE_START,
                cnf_.ANIM_DEATH_END,
                cnf_.ANIM_DEATH_PLAY_SPEED
            );
        }

        // 死亡中の進行と停止固定
        deathTimer_ += dt_;
        if (!deathAnimStopped_)
        {
            const int cur = Model::GetAnimFrame(nowModel_);
			// 猶予を持ってアニメ終了で停止
            if (cur >= cnf_.ANIM_DEATH_END - cnf_.ANIM_DEATH_BUFFER)
            {
                Model::SetAnimFrame(nowModel_, cnf_.ANIM_DEATH_END, cnf_.ANIM_DEATH_END, 0.0f);
                deathAnimStopped_ = true;
            }
        }
        Model::SetTransform(nowModel_, transform_);
        plvision_.Update(transform_.position_);
        return; // 以降の通常処理は走らせない
    }

    // 攻撃モーション中は他の動作を行えない
    MeleeAttack();

	// ダメージクールタイム更新
    if (damageCooldown_ > 0.0f)
    {
		damageCooldown_ -= dt_;
    }

    if (!isAttacking_)
    {
        PlayMoveSound();

        // 入力によるモデル切り替え
        // 切り替えたタイミングでアニメーションを最初から再生
        ChangeModel();
    }

	// 移動処理はまとめてPlayerMovementクラスに任せる
	movement_->Update(isAttacking_, health_, wallColliders_, &plvision_);

	// アニメーションに使うので向きは取得する
	moveDir_ = movement_->GetMoveDir();

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

    // 経験値100に達したらレベルアップでステータスアップ
    LevelUp();

	// 体力・マナ回復処理
    if(mana_ < cnf_.MAX_MANA)
    {
		mana_ += cnf_.MANA_RECOVERY_RATE * dt_;
    }

    // カメラ更新
    plvision_.Update(transform_.position_);
 }

void Player::Draw()
{
    // 現在のモデルを描画
	Model::SetTransform(nowModel_, transform_);
	Model::Draw(nowModel_);

#ifdef _DEBUG
    pCollider_->Draw(transform_.position_, transform_.rotate_);

    if (attackCollider_)
    {
        attackCollider_->Draw(transform_.position_, transform_.rotate_);
    }
#endif

    Direct3D::SetShader(Direct3D::SHADER_BILLBOARD);
    Direct3D::SetBlendMode(Direct3D::BLEND_ALPHA);

    // 丸影の描画
    float yawRad_ = XMConvertToRadians(transform_.rotate_.y);
    XMFLOAT3 shadowPos_ = XMFLOAT3(transform_.position_.x, pPlane_->GetPlanePos().y, transform_.position_.z);
    XMMATRIX S = XMMatrixScaling(cnf_.SHADOW_SCALE.x, cnf_.SHADOW_SCALE.y, cnf_.SHADOW_SCALE.z);
	// 90度回転して地面に平行に
	XMMATRIX R = XMMatrixRotationX(XM_PIDIV2) * XMMatrixRotationY(yawRad_);
    // プレイヤーの正面に合わせて描画
	XMMATRIX T = XMMatrixTranslation(shadowPos_.x, shadowPos_.y + cnf_.SHADOW_OFFSET_Y, shadowPos_.z);

	XMMATRIX world = S * R * T;
	shadowBillboard_->Draw(world, cnf_.SHADOW_COLOR);

    Direct3D::SetShader(Direct3D::SHADER_3D);
    Direct3D::SetBlendMode(Direct3D::BLEND_DEFAULT);
}

void Player::Release()
{
    shadowBillboard_->Release();
    delete movement_;
    movement_ = nullptr;
}

void Player::OnCollision(GameObject* pTarget)
{
    if (!pTarget) return;
    // 自分との衝突は無視
    if (pTarget == this) return;

    Collider* myCol = GetLastHitCollider();
    Collider* targetCol = pTarget->GetLastHitCollider();
    if (!myCol || !targetCol) return;

    const auto myRole = myCol->GetRole();
    const auto targetRole = targetCol->GetRole();

    std::string eName = pTarget->GetObjectName();

    // 宝石取得時
    if (pTarget->GetObjectName() == "Jewel" && !pTarget->IsDead())
    {
        exp_ += cnf_.JEWEL_EXP; // 経験値加算
    }

    // 近接攻撃のヒット判定（自分のAttack -> 相手Body が "Enemy"）
    if (myRole == Collider::Role::Attack && targetRole == Collider::Role::Body)
    {
        const bool isEnemy = (pTarget->GetObjectName() == std::string("Enemy"));
        if (isEnemy && isAttacking_)
        {
            attackHitThisSwing_ = true;

            // まだ効果音を鳴らしていなければ、ヒット瞬間に鳴らす
            if (!attackSoundPlayedThisSwing_)
            {
                Audio::Play(hitSEHandle_);
                attackSoundPlayedThisSwing_ = true;
            }
        }
    }

    // Body×Body or Body×Attack の接触で敵とぶつかった場合のダメージ（従来の挙動）
    if (myRole == Collider::Role::Body && (targetRole == Collider::Role::Body || targetRole == Collider::Role::Attack))
    {
        bool isEnemy = (pTarget->GetObjectName() == "Enemy");
        if (isEnemy && damageCooldown_ <= 0.0f)
        {
            // 敵の攻撃力を持ってくる
            auto* enemy = dynamic_cast<testEnemy*>(pTarget);

            int attackPower_;
            attackPower_ = enemy->GetAttackPower();
            health_ -= attackPower_;
            damageCooldown_ = cnf_.DAMAGE_INVINCIBLE_TIME;

            // PopupDamageオブジェクト生成
            PopUpDamage* popup_ = Instantiate<PopUpDamage>(GetParent());
			assert(popup_ != nullptr);
            if (popup_ && health_ > 0.0f)
            {
				popup_->SetDamageType(DamageType::FromEnemy);
				// popupのステータス設定
                popup_->SetDamage(attackPower_);
                // popupの位置と回転をプレイヤーに合わせる
                popup_->SetPosition(transform_.position_);
                popup_->Initialize();
            }
        }
    }
}

void Player::ChangeModel()
{
    // 1) すでに死亡モーション再生中で、終端に到達していたら固定して抜ける
    if (isDead_)
    {
        return;
    }

    int prevModel = nowModel_;
    int targetModel = nowModel_;

    // 2) 体力が0なら死亡モーションへ（切り替え時に再生開始）
    if (isDead_)
    {
        targetModel = deathModel_;
    }
    else
    {
        if (!onGround_)
        {
            targetModel = jumpModel_;
        }
        else 
        {
            if (isAttacking_) return;

            if (moveDir_.x > 0)
            {
                if (moveDir_.y > 0)       targetModel = rightStrafeModel_;
                else if (moveDir_.y < 0)  targetModel = leftStrafeModel_;
                else                targetModel = walkModel_;
            }
            else if (moveDir_.x < 0) 
            {
                targetModel = backStrafeModel_;
            }
            else if (moveDir_.y > 0) 
            {
                targetModel = rightStrafeModel_;
            }
            else if (moveDir_.y < 0) 
            {
                targetModel = leftStrafeModel_;
            }
            else
            {
                targetModel = idleModel_;
            }
        }
    }

    // 3) 切り替え時だけフレーム設定
    if (prevModel != targetModel)
    {
        nowModel_ = targetModel;

        if (nowModel_ == rightStrafeModel_ || nowModel_ == leftStrafeModel_)
        {
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_STRAFE_END, cnf_.ANIM_BASE_SPEED);
        }
        else if (nowModel_ == walkModel_)
        {
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_WALK_END, cnf_.ANIM_BASE_SPEED);
        }
        else if (nowModel_ == backStrafeModel_)
        {
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_BACK_END, cnf_.ANIM_BASE_SPEED);
        }
        else if (nowModel_ == idleModel_) 
        {
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_IDLE_END, cnf_.ANIM_BASE_SPEED);
        }
        else if (nowModel_ == jumpModel_) 
        {
            float jumpAnimSpeed = cnf_.ANIM_BASE_SPEED * (JumpV0_ / (JumpV0_ + cnf_.GRAVITY)) + cnf_.ANIM_JUMP_BUFFER;
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_JUMP_END, jumpAnimSpeed);
        }
        else if (nowModel_ == deathModel_) 
        {
            // 死亡アニメ再生開始（非ループ化は上の固定ロジックで担保）
            Model::SetAnimFrame(nowModel_, cnf_.ANIM_BASE_START, cnf_.ANIM_DEATH_END, cnf_.ANIM_DEATH_PLAY_SPEED);
        }
    }
}

void Player::ShootMagic()
{
    // 右クリックで魔法発射
    if (Input::IsMouseButtonDown(1) && mana_ >= cnf_.MAGIC_MANA_COST)
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
		mana_ -= cnf_.MAGIC_MANA_COST;

        // 魔法発射音
		Audio::Play(shootSEHandle_);
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
        // 経過時間を積算
        attackTimer_ += dt_;

        // サウンド再生タイミング到達
        if (!attackSoundPlayedThisSwing_ && attackTimer_ >= cnf_.SLASH_SOUND_DELAY)
        {
            // 当たっていればヒットSE、当たっていなければ空振りSE
            Audio::Play(attackHitThisSwing_ ? hitSEHandle_ : swingSEHandle_);
            attackSoundPlayedThisSwing_ = true;
        }

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

            // 次回に向けて状態を戻す
            attackHitThisSwing_ = false;
            attackSoundPlayedThisSwing_ = false;
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
    if (Input::IsMouseButtonDown(0) && onGround_ && !isDead_)
    {
        // その場で向きベクトルを作る（magicDir_ に依存しない）
        float yawRad = XMConvertToRadians(transform_.rotate_.y);
        XMFLOAT3 forwardDir = { -sinf(yawRad), 0.0f, -cosf(yawRad) };

        // ローカル基準オフセット
        XMFLOAT3 localOffset = {
            forwardDir.x * transform_.scale_.z * cnf_.ATTACK_COLLIDER_FORWARD_OFFSET,
            transform_.scale_.y * cnf_.HEIGHT_OFFSET,
            forwardDir.z * transform_.scale_.z * cnf_.ATTACK_COLLIDER_FORWARD_OFFSET
        };

        // 攻撃用コライダー生成
        attackCollider_ = new BoxCollider(cnf_.ATTACK_COLLIDER_BASE_POS, cnf_.ATTACK_COLLIDER_SCALE);
        attackCollider_->SetCenter(localOffset);
        attackCollider_->SetRole(Collider::Role::Attack);
        AddCollider(attackCollider_);

        // 入力・状態初期化
        isMovingNow_ = false;
        isAttacking_ = true;

        // タイマー初期化
        attackTimer_ = 0.0f;
        lastSlashFrame_ = cnf_.SLASH_ANIM_START;
        attackHitThisSwing_ = false;
        attackSoundPlayedThisSwing_ = false; 

        nowModel_ = slashModel_;
        Model::SetAnimFrame(nowModel_, cnf_.SLASH_ANIM_START, cnf_.SLASH_ANIM_END, cnf_.SLASH_PLAY_SPEED);
        Model::SetTransform(nowModel_, transform_);
        plvision_.Update(transform_.position_);
        return;
    }
}

void Player::LevelUp()
{
    // レベルアップ時の処理
    if (exp_ >= 100.0f)
    {
        exp_ = 0.0f;
        strength_ += cnf_.LEVELUP_STRENGTH;
        level_++;
		// レベルアップエフェクト生成
		levelUpEffect_ = Instantiate<LevelUpEffect>(GetParent(), transform_.position_);
        
        // レベルアップポップアップ生成
		PopUpLevelUp* popup_ = Instantiate<PopUpLevelUp>(GetParent());
		assert(popup_ != nullptr);
		if (popup_)
		{
			popup_->SetPosition(transform_.position_);
			popup_->Initialize();
		}

		// レベルアップ音再生
        //Audio::Play(levelUpSEHandle_);
    }
}

void Player::PlayMoveSound()
{
    // 地上のみサウンド再生／停止を扱う
    if (onGround_)
    {
        const bool isMoving = (moveDir_.y != 0 || moveDir_.x != 0);
        const bool isDash = Input::IsKey(DIK_LSHIFT);

        // ストレイフ：左右入力あり または ダッシュ+前後移動中
        if (moveDir_.y != 0 || (isDash && moveDir_.x != 0))
        {
            Audio::Play(strafeSEHandle_);
            Audio::Stop(moveSEHandle_); // 混ざり防止
        }
        // それ以外で前後移動のみなら歩行音
        else if (moveDir_.x != 0)
        {
            Audio::Play(moveSEHandle_);
            Audio::Stop(strafeSEHandle_);
        }
        // 無入力なら両方停止
        else
        {
            Audio::Stop(strafeSEHandle_);
            Audio::Stop(moveSEHandle_);
        }
    }
    else
    {
        // 空中では両方停止（地上専用のループSEのため）
        Audio::Stop(strafeSEHandle_);
        Audio::Stop(moveSEHandle_);
    }
}