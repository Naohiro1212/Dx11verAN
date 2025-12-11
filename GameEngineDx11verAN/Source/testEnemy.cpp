#include "testEnemy.h"
#include "../Engine/Model.h"
#include <assert.h>
#include "../Engine/BoxCollider.h"
#include "../Source/Jewel.h"

testEnemy::testEnemy(GameObject* parent) :GameObject(parent, "testEnemy"), modelHandle_(-1), pCollider_(nullptr)
{
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
