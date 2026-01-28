#include "testEnemy.h"
#include "../Engine/Model.h"
#include <assert.h>
#include "../Engine/BoxCollider.h"
#include "../Source/Jewel.h"
#include "../Source/DungeonManager.h"
#include "../Source/Player.h"
#include "../Engine/GameTime.h"
#include "../Source/EnemyDeathEffect.h"

namespace
{
    const float CHASE_SPEED = 25.0f;
    const float VIEW_DISTANCE = 75.0f;
	const float VIEW_HALF_ANGLE_DEG = 85.0f;
    const float TURN_SPEED_DEG = 85.0f;

    const float WALL_EPS = 1e-3f;
	const XMFLOAT3 ENEMY_SCALE = { 0.1f, 0.1f, 0.1f };

	const float BACK_TIME_LIMIT = 2.0f;
	const float ATTACK_DISTANCE = 15.0f;

    // モデル切替の移動しきい値
    const float MOVE_EPS = 1e-2f;
	const float DAMAGE_COOLDOWN_TIME = 0.5f;
    const float DEATH_TIMER_LIMIT = 3.0f;

    // ノックバック関連
    const float KNOCKBACK_DURATION = 0.35f; // ノックバック継続時間（秒）
    const float KNOCKBACK_SPEED = 120.0f;     // ノックバック速度（単位/秒）
    const float KNOCKBACK_DAMP = 0.85f;     // 毎フレーム減衰（必要なら調整）

	// モデルのアニメーションフレーム範囲
    const int ANIM_BASE_START = 0;
	const int ANIM_IDLE_END = 427;
	const int ANIM_WALK_END = 43;
	const int ANIM_DEATH_END = 139;
    const int ANIM_ATTACK_END = 34;
	const float ANIM_BASE_SPEED = 0.7f;
    const float ANIM_DEATH_SPEED = 0.6f;

    // 初期位置とのずれの許容値
    const float POSITION_EPS = 0.5f;

    const float ATTACK_INTERVAL = 1.5f;

    // 仮の攻撃力
    const int ATTACK_POWER = 10;
}

testEnemy::testEnemy(GameObject* parent)
    :GameObject(parent, "Enemy"), 
    idleModel_(-1),
    walkModel_(-1),
    pCollider_(nullptr),
	attackCollider_(nullptr),
    isSpotted_(false),
	isAttacking_(false),
    velocity_{ 0.0f,0.0f,0.0f }, 
    player_(nullptr),
    deathEffect_(nullptr)
{
    enemyWallColliders_.clear();
}

testEnemy::~testEnemy()
{
    pCollider_ = nullptr;
	attackCollider_ = nullptr;
}

void testEnemy::Initialize()
{
	transform_.position_ = { 0.0f, 0.5f, 0.0f }; // 初期位置
    transform_.scale_ = ENEMY_SCALE;

	// モデル読み込み
	idleModel_ = Model::Load("Models/mutantIdle.fbx");
	walkModel_ = Model::Load("Models/mutantWalk.fbx");
    deathModel_ = Model::Load("Models/mutantDeath.fbx");
    attackModel_ = Model::Load("Models/mutantAttack.fbx");
    assert(idleModel_ != -1);
    assert(walkModel_ != -1);
    assert(deathModel_ != -1);
    assert(attackModel_ != -1);

	nowModel_ = idleModel_;
    Model::SetAnimFrame(nowModel_,ANIM_BASE_START, ANIM_IDLE_END, ANIM_BASE_SPEED);

	pCollider_ = new BoxCollider(XMFLOAT3(0.0f,10.0f,0.0f), XMFLOAT3(transform_.scale_.x * 80.0f, transform_.scale_.y * 170.0f, transform_.scale_.z * 40.0f));
	AddCollider(pCollider_);
    pCollider_->SetRole(Collider::Role::Body);

	player_ = dynamic_cast<Player*>(FindObject("Player"));

    // velocity_ は MoveToPlayer で CHASE_SPEED を掛けた値になっている前提
    moveVec_ = { velocity_.x, 0.0f, velocity_.z };

    backTimer_ = 0.0f;
    health_ = 50.0f;
	damageCooldown_ = 0.0f;
	deathTimer_ = 0.0f;

    // ノックバック初期化
    knockbackVec_ = { 0.0f, 0.0f, 0.0f };
    knockbackTimer_ = 0.0f;

    attackCooldown_ = 0.0f;
    lastAttackFrame_ = ANIM_BASE_START;

    attackPower_ = ATTACK_POWER;
}

void testEnemy::Update()
{
    float dt_ = GameTime::DeltaTime();

    AttackPlayer();

    // ダメージクールタイム更新
    if (damageCooldown_ > 0.0f)
    {
        damageCooldown_ -= dt_;
        if (damageCooldown_ < 0.0f) damageCooldown_ = 0.0f;
    }

    // 攻撃クールタイム更新
    if (attackCooldown_ > 0.0f)
    {
        attackCooldown_ -= dt_;
		if (attackCooldown_ < 0.0f) attackCooldown_ = 0.0f;
    }

    if (health_ <= 0.0f)
    {
        // 死亡初回フレームで当たり判定や移動を止める
        if (deathTimer_ == 0.0f)
        {
            isSpotted_ = false;
            velocity_ = { 0.0f, 0.0f, 0.0f };
            moveVec_ = { 0.0f, 0.0f, 0.0f };
            // コライダー無効化
            RemoveCollider(pCollider_);
			pCollider_ = nullptr;
        }

        // 死亡タイマー進行
        deathTimer_ += dt_;

        // モーション更新
        ChangeModel();
        Model::SetTransform(nowModel_, transform_);

        // 一定時間後にエフェクト生成→ドロップ→消滅
        if (deathTimer_ >= DEATH_TIMER_LIMIT)
        {
            deathEffect_ = Instantiate<EnemyDeathEffect>(GetParent(), transform_.position_);
            DropJewel(3);
            KillMe();
        }
        return;
    }

    // ノックバック中は追跡を停止してノックバックのみ適用
    if (knockbackTimer_ > 0.0f)
    {
        // 位置更新（XZ のみ）
        transform_.position_.x += knockbackVec_.x * dt_;
        transform_.position_.z += knockbackVec_.z * dt_;

        // 減衰（必要なければこの行は削除）
        knockbackVec_.x *= KNOCKBACK_DAMP;
        knockbackVec_.z *= KNOCKBACK_DAMP;

        knockbackTimer_ -= dt_;
        if (knockbackTimer_ < 0.0f) knockbackTimer_ = 0.0f;

        // アニメ用に「移動中」とみなす
        moveVec_ = knockbackVec_;

        // 壁ずり・貫通解消
        for (auto* wallCollider_ : enemyWallColliders_)
        {
            PenetrationResult res = Collider::ComputeBoxVsBoxPenetration(pCollider_, wallCollider_);
            if (res.overlapped)
            {
                transform_.position_.x += res.push.x + (res.push.x > 0 ? WALL_EPS : (res.push.x < 0 ? -WALL_EPS : 0.0f));
                transform_.position_.z += res.push.z + (res.push.z > 0 ? WALL_EPS : (res.push.z < 0 ? -WALL_EPS : 0.0f));
                moveVec_ = SlideAlongWall(moveVec_, res.normal);
            }
        }

        // モデル更新して今フレーム終了
        ChangeModel();
        Model::SetTransform(nowModel_, transform_);
        return;
    }

    // 生存時の処理（視認・追跡・復帰）
    LookAtPlayer();
    MoveToPlayer();

    // ここで velocity_ を moveVec_ に反映
    moveVec_ = { velocity_.x, 0.0f, velocity_.z };

    if (isSpotted_)
    {
        backTimer_ += dt_;
        transform_.position_.x += moveVec_.x * dt_;
        transform_.position_.z += moveVec_.z * dt_;
    }
    else
    {
        XMFLOAT3 dirToInit{
            initPos_.x - transform_.position_.x,
            0.0f,
            initPos_.z - transform_.position_.z
        };
        XMVECTOR vDirToInit = XMLoadFloat3(&dirToInit);
        float lenSq = XMVectorGetX(XMVector3LengthSq(vDirToInit));
        if (lenSq > 1e-6f) vDirToInit = XMVector3Normalize(vDirToInit);
        else               vDirToInit = XMVectorZero();
        XMStoreFloat3(&dirToInit, vDirToInit);

		// 初期位置へ戻る
        // 初期位置の方向を向く
		transform_.rotate_.y = atan2f(-dirToInit.x, -dirToInit.z) * (180.0f / XM_PI);

        // 微小なずれを許容する
        float distToInitSq = (initPos_.x - transform_.position_.x) * (initPos_.x - transform_.position_.x)
            + (initPos_.z - transform_.position_.z) * (initPos_.z - transform_.position_.z);
        if (distToInitSq > POSITION_EPS)
        {
            // まだ離れている場合のみ移動
            transform_.position_.x += dirToInit.x * CHASE_SPEED * dt_;
            transform_.position_.z += dirToInit.z * CHASE_SPEED * dt_;
        }
        else
        {
            moveVec_ = { 0.0f, 0.0f, 0.0f };
        }
    }

    if (backTimer_ > BACK_TIME_LIMIT)
    {
        isSpotted_ = false;
        backTimer_ = 0.0f;
    }

    // 壁ずり・貫通解消
    for (auto* wallCollider_ : enemyWallColliders_)
    {
        PenetrationResult res = Collider::ComputeBoxVsBoxPenetration(pCollider_, wallCollider_);
        if (res.overlapped)
        {
            transform_.position_.x += res.push.x + (res.push.x > 0 ? WALL_EPS : (res.push.x < 0 ? -WALL_EPS : 0.0f));
            transform_.position_.z += res.push.z + (res.push.z > 0 ? WALL_EPS : (res.push.z < 0 ? -WALL_EPS : 0.0f));
            moveVec_ = SlideAlongWall(moveVec_, res.normal);
        }
    }

    // モデル更新
    ChangeModel();
    Model::SetTransform(nowModel_, transform_);
}

void testEnemy::Draw()
{
	Model::Draw(nowModel_);
    if (pCollider_)
    {
#ifdef _DEBUG
        pCollider_->Draw(transform_.position_, transform_.rotate_);
#endif 
    }

    if (attackCollider_)
    {
#ifdef _DEBUG
		attackCollider_->Draw(transform_.position_, transform_.rotate_);
#endif
    }
}

void testEnemy::Release()
{
}

void testEnemy::OnCollision(GameObject* pTarget)
{
    if (!pTarget) return;

    Collider* myCol = GetLastHitCollider();
    Collider* targetCol = pTarget->GetLastHitCollider();
    if (!myCol || !targetCol) return;

    const auto myRole = myCol->GetRole();
    const auto targetRole = targetCol->GetRole();

    const bool anyAttack = (myRole == Collider::Role::Body && targetRole == Collider::Role::Attack);

    const bool isPlayer = (pTarget->GetObjectName() == "Player");
	const bool isMagic = (pTarget->GetObjectName() == "MagicSphere");

    if (anyAttack && (isPlayer || isMagic) && damageCooldown_ <= 0.0f)
    {
        // ダメージ
        health_ -= player_->GetStrength();
        damageCooldown_ = DAMAGE_COOLDOWN_TIME;

        // ノックバック方向（攻撃発生源 → 敵 の反対方向）
        // Playerの位置を使う
        XMFLOAT3 srcPos = player_->GetPosition();

        XMFLOAT3 enemyPos = transform_.position_;
        XMFLOAT3 dir =
        {
            enemyPos.x - srcPos.x,
            0.0f,
            enemyPos.z - srcPos.z
        };

        // 正規化して速度ベクトルへ
        XMVECTOR vDir = XMLoadFloat3(&dir);
        float lenSq = XMVectorGetX(XMVector3LengthSq(vDir));
        if (lenSq > 1e-6f)
        {
            vDir = XMVector3Normalize(vDir);
            XMFLOAT3 n; XMStoreFloat3(&n, vDir);
            knockbackVec_.x = n.x * KNOCKBACK_SPEED;
            knockbackVec_.z = n.z * KNOCKBACK_SPEED;
            knockbackVec_.y = 0.0f;
            knockbackTimer_ = KNOCKBACK_DURATION;
        }

        // 追跡速度は一旦ゼロ（ノックバック優先）
        velocity_ = { 0.0f, 0.0f, 0.0f };
    }
    // Body×Body の場合、敵側ではダメージ適用しない（重複防止）
}

// 敵がプレイヤーをその場で視認して、追跡させる
void testEnemy::LookAtPlayer()
{
    // プレイヤーの位置を取得
    XMFLOAT3 playerPos = player_->GetPosition();
    XMFLOAT3 enemyPos = transform_.position_;

    // 敵からプレイヤーへのベクトルを計算
    XMFLOAT3 dir_
    {
        playerPos.x - enemyPos.x,
        0.0f, // 水平方向のみ
        playerPos.z - enemyPos.z
    };

	// 距離判定
	float distSq_ = dir_.x * dir_.x + dir_.z * dir_.z;
	float viewDistSq_ = VIEW_DISTANCE * VIEW_DISTANCE;
    
    // 敵の前方ベクトルを計算
	float yawRad_ = XMConvertToRadians(transform_.rotate_.y);
    XMFLOAT3 forward_{
        -sinf(yawRad_),
         0.0f,
        -cosf(yawRad_)
    };

    // 正規化
    XMVECTOR vToPlayer = XMLoadFloat3(&dir_);
    XMVECTOR vEnemyFwd = XMLoadFloat3(&forward_);
    if (XMVector3LengthSq(vToPlayer).m128_f32[0] > 1e-6f)
    {
        vToPlayer = XMVector3Normalize(vToPlayer);
    }
    vEnemyFwd = XMVector3Normalize(vEnemyFwd);

    // 角度差を計算
	float dot = XMVectorGetX(XMVector3Dot(vToPlayer, vEnemyFwd));
	dot = (std::max)(-1.0f, (std::min)(1.0f, dot)); // Clamp
	float angleDiffDig_ = XMConvertToDegrees(acosf(dot));

	isSpotted_ = (distSq_ <= viewDistSq_) && (angleDiffDig_ <= VIEW_HALF_ANGLE_DEG);
}

void testEnemy::MoveToPlayer()
{
    if (!isSpotted_ || isAttacking_) return;

    // プレイヤーと敵の位置
    XMFLOAT3 playerPos = player_->GetPosition();
    XMFLOAT3 enemyPos = transform_.position_;

    // 水平方向の差分ベクトル（Yは0）
    XMFLOAT3 dir{
        playerPos.x - enemyPos.x,
        0.0f,
        playerPos.z - enemyPos.z
    };

    // 正規化（ゼロ長チェック）
    XMVECTOR vDir = XMLoadFloat3(&dir);
    float lenSq = XMVectorGetX(XMVector3LengthSq(vDir));
    if (lenSq > 1e-6f) {
        vDir = XMVector3Normalize(vDir);
    }
    else {
        // 近すぎる場合は移動・回転しない
        velocity_ = { 0,0,0 };
        return;
    }

    // 速度
    XMVECTOR vStep = XMVectorScale(vDir, CHASE_SPEED);
    XMStoreFloat3(&velocity_, vStep);

	// 目標向きのヨー角を計算
    float targetYawDeg = XMConvertToDegrees(atan2f(dir.x, dir.z)) + 180.0f;

    // 現在
    float currentYawDeg = transform_.rotate_.y;

    // 差分を [-180, 180] に正規化
    float diff = targetYawDeg - currentYawDeg;
    while (diff > 180.0f)  diff -= 360.0f;
    while (diff < -180.0f) diff += 360.0f;

    // 毎秒の最大回転角 → 毎フレームに変換
	const float dt_ = GameTime::DeltaTime();
    float maxTurnPerFrame = TURN_SPEED_DEG * dt_;

    // 実際に回す角度をクランプ
    float stepYaw = (diff > 0.0f)
        ? (std::min)(diff, maxTurnPerFrame)
        : (std::max)(diff, -maxTurnPerFrame);

    transform_.rotate_.y += stepYaw;
}

void testEnemy::AttackPlayer()
{
	// プレイヤーor敵が死んでいるなら何もしない
    if (player_->IsDead()) return;

    // ノックバック中でも何もしない
    if(knockbackTimer_ > 0.0f) return;

    // 攻撃モーション中は他の動作を行えない
    if (isAttacking_)
    {
        // その場で停止
        velocity_ = { 0.0f, 0.0f, 0.0f };
        moveVec_ = { 0.0f, 0.0f, 0.0f };

		// 1週目の攻撃アニメが終了したら通常モーションへ戻す
        int cur = Model::GetAnimFrame(attackModel_);
        if(cur < lastAttackFrame_ || cur >= ANIM_ATTACK_END)
        {
            isAttacking_ = false;

			// 攻撃用コライダー破棄
            if (attackCollider_)
            {
                RemoveCollider(attackCollider_);
                attackCollider_ = nullptr;
            }
            nowModel_ = idleModel_;
			Model::SetAnimFrame(nowModel_, ANIM_BASE_START, ANIM_IDLE_END, ANIM_BASE_SPEED);
        }
        else
        {
            lastAttackFrame_ = cur;
        }

		Model::SetTransform(nowModel_, transform_);
        return;
    }

    // プレイヤーが取得できてないなら何もしない
    if (!player_) return;

    // プレイヤーと敵の位置
    XMFLOAT3 playerPos = player_->GetPosition();
    XMFLOAT3 enemyPos = transform_.position_;

    // 一定距離内なら、立ち止まって攻撃モーションを行う
    XMFLOAT3 dir{
        playerPos.x - enemyPos.x,
        0.0f,
        playerPos.z - enemyPos.z
    };

    float distSq = dir.x * dir.x + dir.z * dir.z;
    float attackDistSq = ATTACK_DISTANCE * ATTACK_DISTANCE;

    // 一定距離内かつ、クールタイム終了・体力があるなら攻撃開始
    if (distSq <= attackDistSq && attackCooldown_ <= 0.0f && health_ > 0.0f)
    {
        // 前方ベクトル（rotate_.y が度）
        float yawRad = XMConvertToRadians(transform_.rotate_.y);
        XMFLOAT3 forwardDir = { -sinf(yawRad), 0.0f, -cosf(yawRad) };

        // ローカル基準オフセット（前方へ）
        XMFLOAT3 localOffset = {
			forwardDir.x* transform_.scale_.z * 80.0f,
			transform_.scale_.y * 60.0f,
			forwardDir.z* transform_.scale_.z * 80.0f
        };

        // 攻撃用コライダー生成
		attackCollider_ = new BoxCollider(XMFLOAT3(0.0f, 10.0f, 0.0f), XMFLOAT3(transform_.scale_.x * 60.0f, transform_.scale_.y * 60.0f, transform_.scale_.z * 60.0f));
        attackCollider_->SetCenter(localOffset);
        attackCollider_->SetRole(Collider::Role::Attack);
        AddCollider(attackCollider_);

        // 状態初期化
        isAttacking_ = true;
        attackCooldown_ = ATTACK_INTERVAL;
        lastAttackFrame_ = ANIM_BASE_START;

        // アニメ開始
        nowModel_ = attackModel_;
        Model::SetAnimFrame(nowModel_, ANIM_BASE_START, ANIM_ATTACK_END, ANIM_BASE_SPEED);
        Model::SetTransform(nowModel_, transform_);


        return;
    }
}


void testEnemy::SetPosition(const XMFLOAT3& pos)
{
    transform_.position_ = pos;    
    initPos_ = pos;
}

// 敵が死んだときに宝石をドロップする処理
// 現在円状にドロップする実装
void testEnemy::DropJewel(int numJewels)
{
    XMFLOAT3 center = this->transform_.position_; // 敵の位置
    float radius = 5.0f; // 円の半径
    for (int i = 0; i < numJewels; ++i)
    {
        float angle = XM_2PI * i / numJewels;
        float x = center.x + radius * cosf(angle);
        float z = center.z + radius * sinf(angle);
        XMFLOAT3 dropPos(x, center.y, z);

        Jewel* jewel = Instantiate<Jewel>(GetParent());
        jewel->SetPosition(dropPos);
    }
}

XMFLOAT3 testEnemy::SlideAlongWall(const XMFLOAT3& f, const XMFLOAT3& n)
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

void testEnemy::ChangeModel()
{
    int prevModel = nowModel_;
    int targetModel = nowModel_;

    // 体力が0になったら死亡モーションへ
    if (health_ <= 0.0f)
    {
		// 死亡モーションへ変更
        targetModel = deathModel_;
    }
    else
    {
        if (isAttacking_) return;

        // 今フレームの水平移動ベクトルで移動/停止判定
        float moveLenSq = moveVec_.x * moveVec_.x + moveVec_.z * moveVec_.z;
        if (moveLenSq > MOVE_EPS * MOVE_EPS)
        {
            targetModel = walkModel_;
        }
        else
        {
            targetModel = idleModel_;
        }
    }

    // 変更時のみ適用とアニメ範囲設定
    if (prevModel != targetModel)
    {
        nowModel_ = targetModel;

        if (nowModel_ == idleModel_)
        {
            Model::SetAnimFrame(nowModel_, ANIM_BASE_START, ANIM_IDLE_END, ANIM_BASE_SPEED);
        }
        else if (nowModel_ == walkModel_)
        {
            Model::SetAnimFrame(nowModel_, ANIM_BASE_START, ANIM_WALK_END, ANIM_BASE_SPEED);
        }
        else if(nowModel_ == deathModel_)
        {
            Model::SetAnimFrame(nowModel_, ANIM_BASE_START, ANIM_DEATH_END, ANIM_DEATH_SPEED);
		}
    }

    if (nowModel_ == deathModel_)
    {
        const int cur = Model::GetAnimFrame(nowModel_);
        if (cur >= ANIM_DEATH_END)
        {
			Model::SetAnimFrame(nowModel_, ANIM_DEATH_END, ANIM_DEATH_END, 0.0f); // 最終フレームで停止
        }
    }
}