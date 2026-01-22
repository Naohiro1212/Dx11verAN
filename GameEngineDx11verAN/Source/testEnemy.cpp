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
    const float VIEW_DISTANCE = 100.0f;
	const float VIEW_HALF_ANGLE_DEG = 750.0f;
    const float TURN_SPEED_DEG = 8.5f;

    const float WALL_EPS = 1e-3f;
	const XMFLOAT3 ENEMY_SCALE = { 0.1f, 0.1f, 0.1f };

	const float BACK_TIME_LIMIT = 2.0f;

    // モデル切替の移動しきい値
    const float MOVE_EPS = 1e-2f;
	const float DAMAGE_COOLDOWN_TIME = 0.5f;
    const float DEATH_TIMER_LIMIT = 3.0f;

    // ノックバック関連
    const float KNOCKBACK_DURATION = 0.25f; // ノックバック継続時間（秒）
    const float KNOCKBACK_SPEED = 8.0f;     // ノックバック速度（単位/秒）
}

testEnemy::testEnemy(GameObject* parent) :GameObject(parent, "Enemy"), idleModel_(-1), walkModel_(-1), pCollider_(nullptr),
isSpoted_(false), velocity_{ 0.0f,0.0f,0.0f }, player_(nullptr), deathEffect_(nullptr)
{
    enemyWallColliders_.clear();
}

testEnemy::~testEnemy()
{
}

void testEnemy::Initialize()
{
	transform_.position_ = { 0.0f, 0.5f, 0.0f };
    transform_.scale_ = ENEMY_SCALE;

	// モデル読み込み
	idleModel_ = Model::Load("Models/mutantIdle.fbx");
	walkModel_ = Model::Load("Models/mutantWalk.fbx");
    deathModel_ = Model::Load("Models/mutantDeath.fbx");
    assert(idleModel_ != -1);
    assert(walkModel_ != -1);
    assert(deathModel_ != -1);

	nowModel_ = idleModel_;
    Model::SetAnimFrame(nowModel_,0, 427, 1.0f);

	pCollider_ = new BoxCollider(XMFLOAT3(0.0f,10.0f,0.0f), XMFLOAT3(transform_.scale_.x * 40.0f, transform_.scale_.y * 170.0f, transform_.scale_.z * 40.0f));
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

}

void testEnemy::Update()
{
    float dt_ = GameTime::DeltaTime();

    // ダメージクールタイム更新（抜けていた）
    if (damageCooldown_ > 0.0f)
    {
        damageCooldown_ -= dt_;
        if (damageCooldown_ < 0.0f) damageCooldown_ = 0.0f;
    }

    if (health_ <= 0.0f)
    {
        // 死亡初回フレームで当たり判定や移動を止める
        if (deathTimer_ == 0.0f)
        {
            isSpoted_ = false;
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

    // 生存時の処理（視認・追跡・復帰）
    LookAtPlayer();
    MoveToPlayer();

    // ここで velocity_ を moveVec_ に反映
    moveVec_ = { velocity_.x, 0.0f, velocity_.z };

    if (isSpoted_)
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

        transform_.position_.x += dirToInit.x * CHASE_SPEED * dt_;
        transform_.position_.z += dirToInit.z * CHASE_SPEED * dt_;
    }

    if (backTimer_ > BACK_TIME_LIMIT)
    {
        isSpoted_ = false;
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
        pCollider_->Draw(transform_.position_, transform_.rotate_);
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

    const bool isPlayer = (pTarget->GetObjectName() == "Player" || pTarget->GetObjectName() == "MagicSphere");

    if (anyAttack && isPlayer && damageCooldown_ <= 0.0f)
    {
        // プレイヤーの近接ダメージを1回だけ受ける
        health_ -= player_->GetStrength();
        damageCooldown_ = DAMAGE_COOLDOWN_TIME;

        // ノックバック設定: プレイヤー方向の逆（敵から見てプレイヤーへ向かうベクトルの反対）
        XMFLOAT3 playerPos = player_->GetPosition();
        XMFLOAT3 enemyPos = transform_.position_;
        XMFLOAT3 kbDir{
            enemyPos.x - playerPos.x,
            0.0f,
            enemyPos.z - playerPos.z
        };

        XMVECTOR vKb = XMLoadFloat3(&kbDir);
        if (XMVectorGetX(XMVector3LengthSq(vKb)) > 1e-6f)
        {
            vKb = XMVector3Normalize(vKb);
            vKb = XMVectorScale(vKb, KNOCKBACK_SPEED);
            XMStoreFloat3(&knockbackVec_, vKb);
            knockbackTimer_ = KNOCKBACK_DURATION;

            // ノックバック中は通常の velocity を止めておく（必要なら）
            velocity_ = { 0.0f, 0.0f, 0.0f };
            moveVec_ = { 0.0f, 0.0f, 0.0f };
        }
        else
        {
            // 近接しすぎて方向が取れない場合は軽く後ろへ押すだけ
            knockbackVec_ = { -0.1f * KNOCKBACK_SPEED, 0.0f, -0.1f * KNOCKBACK_SPEED };
            knockbackTimer_ = KNOCKBACK_DURATION * 0.3f;
        }
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

	isSpoted_ = (distSq_ <= viewDistSq_) && (angleDiffDig_ <= VIEW_HALF_ANGLE_DEG);
}

void testEnemy::MoveToPlayer()
{
    if (!isSpoted_) return;

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
    float maxTurnPerFrame = TURN_SPEED_DEG;

    // 実際に回す角度をクランプ
    float stepYaw = (diff > 0.0f)
        ? (std::min)(diff, maxTurnPerFrame)
        : (std::max)(diff, -maxTurnPerFrame);

    transform_.rotate_.y += stepYaw;
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
            Model::SetAnimFrame(nowModel_, 0, 427, 1.0f);
        }
        else if (nowModel_ == walkModel_)
        {
            Model::SetAnimFrame(nowModel_, 0, 43, 1.0f);
        }
        else if(nowModel_ == deathModel_)
        {
            Model::SetAnimFrame(nowModel_, 0, 139, 0.8f);
		}
    }

    if (nowModel_ == deathModel_)
    {
        const int cur = Model::GetAnimFrame(nowModel_);
        if (cur >= 139)
        {
			Model::SetAnimFrame(nowModel_, 139, 139, 0.0f); // 最終フレームで停止
        }
    }
}