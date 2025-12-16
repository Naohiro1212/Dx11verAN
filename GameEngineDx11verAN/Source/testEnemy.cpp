#include "testEnemy.h"
#include "../Engine/Model.h"
#include <assert.h>
#include "../Engine/BoxCollider.h"
#include "../Source/Jewel.h"
#include "../Source/DungeonManager.h"
#include "../Source/Player.h"
#include "../Engine/GameTime.h"

namespace
{
    const float CHASE_SPEED = 10.0f;
    const float VIEW_DISTANCE = 100.0f;
	const float VIEW_HALF_ANGLE_DEG = 50.0f;
    const float TURN_SPEED_DEG = 7.5f;

    const float WALL_EPS = 1e-3f;
}

testEnemy::testEnemy(GameObject* parent) :GameObject(parent, "testEnemy"), modelHandle_(-1), pCollider_(nullptr), isSpoted_(false), velocity_{ 0.0f,0.0f,0.0f }, player_(nullptr)
{
    enemyWallColliders_.clear();
}

testEnemy::~testEnemy()
{
}

void testEnemy::Initialize()
{
	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.scale_ = { 0.1f, 0.1f, 0.1f };

	// 仮にプレイヤーのアイドルモデルを使う
	modelHandle_ = Model::Load("Models/idle.fbx");

	pCollider_ = new BoxCollider(XMFLOAT3(0.0f,10.0f,0.0f), XMFLOAT3(transform_.scale_.x * 40.0f, transform_.scale_.y * 170.0f, transform_.scale_.z * 40.0f));
	AddCollider(pCollider_);
    pCollider_->SetRole(Collider::Role::Body);

	player_ = dynamic_cast<Player*>(FindObject("Player"));

    // velocity_ は MoveToPlayer で CHASE_SPEED を掛けた値になっている前提
    moveVec_ = { velocity_.x, 0.0f, velocity_.z };
}

void testEnemy::Update()
{
    float dt_ = GameTime::DeltaTime();

    // プレイヤーを視認・追跡
    LookAtPlayer();
    MoveToPlayer();

    moveVec_ = { velocity_.x, 0.0f, velocity_.z };

    transform_.position_.x += moveVec_.x * dt_;
    transform_.position_.z += moveVec_.z * dt_;

    // 壁ずり・貫通解消
    for (auto* wallCollider_ : enemyWallColliders_)
    {
        PenetrationResult res = Collider::ComputeBoxVsBoxPenetration(pCollider_, wallCollider_);
        if (res.overlapped)
        {
            transform_.position_.x += res.push.x + (res.push.x > 0 ? WALL_EPS : (res.push.x < 0 ? -WALL_EPS : 0.0f));
            transform_.position_.z += res.push.z + (res.push.z > 0 ? WALL_EPS : (res.push.z < 0 ? -WALL_EPS : 0.0f));

            // 次フレーム以降の移動方向を壁法線に沿ってスライドさせる
            moveVec_ = SlideAlongWall(moveVec_, res.normal);
        }
    }

    // モデルのワールド行列更新
    Model::SetTransform(modelHandle_, transform_);
}

void testEnemy::Draw()
{
	Model::Draw(modelHandle_);
	pCollider_->Draw(transform_.position_, transform_.rotate_);
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

    if (anyAttack && isPlayer)
    {
        DropJewel(5);
        KillMe();
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

    // 移動
    //transform_.position_.x += velocity_.x;
    //transform_.position_.z += velocity_.z;
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
