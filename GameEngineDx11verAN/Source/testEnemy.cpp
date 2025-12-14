#include "testEnemy.h"
#include "../Engine/Model.h"
#include <assert.h>
#include "../Engine/BoxCollider.h"
#include "../Source/Jewel.h"
#include "../Source/DungeonManager.h"
#include "../Source/Player.h"

namespace
{
    const float CHASE_SPEED = 0.5f;
    const float VIEW_DISTANCE = 25.0f;
	const float VIEW_HALF_ANGLE_DEG = 27.5f;
    const float TURN_SPEED_DEG = 5.0f;
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
	transform_.position_ = { 0.0f, 0.0f, 30.0f };
	transform_.scale_ = { 0.1f, 0.1f, 0.1f };

	// 仮にプレイヤーのアイドルモデルを使う
	modelHandle_ = Model::Load("Models/idle.fbx");

	pCollider_ = new BoxCollider(XMFLOAT3(0.0f,10.0f,0.0f), XMFLOAT3(transform_.scale_.x * 40.0f, transform_.scale_.y * 170.0f, transform_.scale_.z * 40.0f));
	AddCollider(pCollider_);
    pCollider_->SetRole(Collider::Role::Body);

    // DungeonManager経由で壁コライダー取得
	GameObject* dungeonManager_ = FindObject("DungeonManager");
	if (dungeonManager_)
	{
		DungeonManager* dm = dynamic_cast<DungeonManager*>(dungeonManager_);
		if (dm)
		{
            enemyWallColliders_ = dm->GetWallColliders();
		}
	}

	player_ = dynamic_cast<Player*>(FindObject("Player"));
}

void testEnemy::Update()
{
}

void testEnemy::Draw()
{
    Model::SetTransform(modelHandle_, transform_);
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
    XMFLOAT3 dir_{
    dir_.x = playerPos.x - enemyPos.x,
    dir_.y = 0.0f, // 水平方向のみ
    dir_.z = playerPos.z - enemyPos.z
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
    if(isSpoted_)
    {
        // プレイヤーの位置を取得
        XMFLOAT3 playerPos = player_->GetPosition();
        XMFLOAT3 enemyPos = transform_.position_;
        // 敵からプレイヤーへのベクトルを計算
        XMFLOAT3 dir_{
        dir_.x = playerPos.x - enemyPos.x,
        dir_.y = 0.0f, // 水平方向のみ
        dir_.z = playerPos.z - enemyPos.z
        };
        // 正規化
        XMVECTOR vDir = XMLoadFloat3(&dir_);
        if (XMVector3LengthSq(vDir).m128_f32[0] > 1e-6f)
        {
            vDir = XMVector3Normalize(vDir);
        }
        // 速度計算
        XMVECTOR vVelocity = XMVectorScale(vDir, CHASE_SPEED);
        XMStoreFloat3(&velocity_, vVelocity);
        // 移動処理
        transform_.position_.x += velocity_.x;
        transform_.position_.z += velocity_.z;
	}
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
