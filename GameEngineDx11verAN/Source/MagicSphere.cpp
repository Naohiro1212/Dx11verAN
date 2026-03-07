#include "MagicSphere.h"
#include "../Engine/Model.h"
#include "../Engine/GameTime.h"
#include "../Engine/SphereCollider.h"

namespace
{
	const float	   MAGIC_SPEED = 100.0f;
	const float	   ATTACK_DURATION = 5.0f;
	const float    MAGIC_SCALE = 1.2f;

	const float    COLLILDER_RADIUS = 2.5f;

	// エフェクト用定数
	const XMFLOAT3 MAGIC_SPHERE_EFFECT_POS_RND = XMFLOAT3(0.12f, 0.12f, 0.12f);

	const XMFLOAT3 MAGIC_SPHERE_EFFECT_DIRECTION = XMFLOAT3(0.0f, 0.0f, 0.0f);
	const XMFLOAT3 MAGIC_SPHERE_EFFECT_DIRECTION_RND = XMFLOAT3(0.0f, 0.0f, 0.0f);

	const float    MAGIC_SPHERE_EFFECT_SPEED = 0.01f;
	const float    MAGIC_SPHERE_EFFECT_SPEED_RND = 0.1f;
	const float    MAGIC_SPHERE_EFFECT_ACCEL = 1.0f;
	const float    MAGIC_SPHERE_EFFECT_GRAVITY = 0.0f;

	const XMFLOAT4 MAGIC_SPHERE_EFFECT_COLOR = XMFLOAT4(0.85f, 0.95f, 1.0f, 0.35f);
	const XMFLOAT4 MAGIC_SPHERE_EFFECT_DELTA_COLOR = XMFLOAT4(-0.001f, -0.001f, -0.001f, -0.003f);

	const XMFLOAT3 MAGIC_SPHERE_EFFECT_ROTATE = XMFLOAT3(0.0f, 0.0f, 0.0f);
	const XMFLOAT3 MAGIC_SPHERE_EFFECT_ROTATE_RND = XMFLOAT3(0.0f, 15.0f, 0.0f);
	const XMFLOAT3 MAGIC_SPHERE_EFFECT_SPIN = XMFLOAT3(0.0f, 1.0f, 0.0f);

	const XMFLOAT2 MAGIC_SPHERE_EFFECT_SIZE = XMFLOAT2(1.2f, 1.2f);
	const XMFLOAT2 MAGIC_SPHERE_EFFECT_SIZE_RND = XMFLOAT2(0.2f, 0.2f);
	const XMFLOAT2 MAGIC_SPHERE_EFFECT_SCALE = XMFLOAT2(1.005f, 1.005f);

	const int      MAGIC_SPHERE_EFFECT_LIFETIME = 40;
	const int      MAGIC_SPHERE_EFFECT_DELAY = 3;
	const int      MAGIC_SPHERE_EFFECT_NUMBER = 2;
}

MagicSphere::MagicSphere(GameObject* parent)
{
}

MagicSphere::MagicSphere(GameObject* parent, const std::vector<BoxCollider*>& _wallColliders)
	: GameObject(parent, "MagicSphere"), 
	  magicModel_(-1),
	  attackTimer_(0.0f),
	  effectData_(),
	  hEmit_(-1),
	  pCollider_(nullptr),
	  wallColliders_(_wallColliders)
{
}

MagicSphere::~MagicSphere()
{
}

void MagicSphere::Initialize()
{
	//仮に箱モデルを使う
	magicModel_ = Model::Load("Models/magicsphere.fbx");
	assert(magicModel_ != -1);

	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };
	transform_.scale_ = { MAGIC_SCALE, MAGIC_SCALE, MAGIC_SCALE };

	pCollider_ = new SphereCollider(XMFLOAT3(0.0f, 0.0f, 0.0f),COLLILDER_RADIUS);
	AddCollider(pCollider_);
	pCollider_->SetRole(Collider::Role::Attack);

	// 魔法のスフィアにエフェクトを追加
	effectData_.textureFileName = "Effects/flashC_B.png";
	effectData_.position     =	transform_.position_;
	effectData_.positionRnd  =	MAGIC_SPHERE_EFFECT_POS_RND;
	effectData_.direction    =	MAGIC_SPHERE_EFFECT_DIRECTION;      // その場で揺らぐ
	effectData_.directionRnd =	MAGIC_SPHERE_EFFECT_DIRECTION_RND;
	effectData_.speed		 =	MAGIC_SPHERE_EFFECT_SPEED;
	effectData_.speedRnd	 =	MAGIC_SPHERE_EFFECT_SPEED_RND;
	effectData_.accel        =	MAGIC_SPHERE_EFFECT_ACCEL;
	effectData_.gravity      =	MAGIC_SPHERE_EFFECT_GRAVITY;
	effectData_.color		 =	MAGIC_SPHERE_EFFECT_COLOR;   // 少し青白
	effectData_.deltaColor   =	MAGIC_SPHERE_EFFECT_DELTA_COLOR;
	effectData_.rotate		 =	MAGIC_SPHERE_EFFECT_ROTATE;
	effectData_.rotateRnd	 =	MAGIC_SPHERE_EFFECT_ROTATE_RND;
	effectData_.spin		 =	MAGIC_SPHERE_EFFECT_SPIN;
	effectData_.size		 =	MAGIC_SPHERE_EFFECT_SIZE;
	effectData_.sizeRnd      =	MAGIC_SPHERE_EFFECT_SIZE_RND;
	effectData_.scale        =	MAGIC_SPHERE_EFFECT_SCALE;   // ゆっくり膨らむ
	effectData_.lifeTime     =	MAGIC_SPHERE_EFFECT_LIFETIME;
	effectData_.delay        =	MAGIC_SPHERE_EFFECT_DELAY;                          // 継続的に発生
	effectData_.number       =	MAGIC_SPHERE_EFFECT_NUMBER;
	effectData_.isBillBoard = true;

	hEmit_ = VFX::Start(effectData_);
}

void MagicSphere::Update()
{
	float dt_ = GameTime::DeltaTime();
	attackTimer_ += dt_;

	float yawRad = DirectX::XMConvertToRadians(transform_.rotate_.y);
	float vx = -sinf(yawRad);
	float vz = -cosf(yawRad);

	transform_.position_.x += vx * MAGIC_SPEED * dt_;
	transform_.position_.z += vz * MAGIC_SPEED * dt_;

	//transform_.rotate_.z += 120.0f * dt_; //回転
	effectData_.position = transform_.position_;

	if (hEmit_ >= 0)
	{
		VFX::SetEmitterPosition(hEmit_, effectData_.position);
	}

	//一定時間経過で消える
	if (attackTimer_ >= ATTACK_DURATION)
	{
		VFX::End(hEmit_);
		KillMe();
	}

	// --- ここから「弾 vs 壁」の自前当たり判定 ---
	if (pCollider_)
	{
		for (auto* wallCol : wallColliders_)
		{
			if (!wallCol)
			{
				continue;
			}

			// 弾の SphereCollider と 壁の BoxCollider の当たり判定
			if (pCollider_->IsHitBoxVsCircle(wallCol, pCollider_))
			{
				// GameObject::Collision 相当の情報だけセットして自分の OnCollision を呼ぶ
				lastHitCollider_ = pCollider_;

				VFX::End(hEmit_);
				KillMe();

				// 一度でも当たったら抜ける
				break;
			}
		}
	}
}

void MagicSphere::Draw()
{
	Model::SetTransform(magicModel_, transform_);
	Model::Draw(magicModel_);

//	pCollider_->Draw(transform_.position_, transform_.rotate_);
}

void MagicSphere::Release()
{
}

void MagicSphere::OnCollision(GameObject* pTarget)
{
	if (!pTarget) return;

	Collider* myCol = GetLastHitCollider();
	Collider* targetCol = pTarget->GetLastHitCollider();
	if (!myCol || !targetCol) return;

	const auto myRole = myCol->GetRole();
	const auto targetRole = targetCol->GetRole();

	const bool isAttack = (myRole == Collider::Role::Attack && targetRole == Collider::Role::Body);
	const bool isEnemy = (pTarget->GetObjectName() == "Enemy");
	const bool isWall = (targetRole == Collider::Role::Static);

	if ((isAttack && isEnemy) || isWall)
	{
		VFX::End(hEmit_);
		KillMe();
	}
}
