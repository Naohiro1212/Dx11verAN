#include "HomingMagicSphere.h"
#include "../Engine/GameTime.h"
#include "../Engine/Model.h"

#include <cmath>
#include <algorithm>

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

HomingMagicSphere::HomingMagicSphere(GameObject* parent)
{
}

HomingMagicSphere::HomingMagicSphere(GameObject* parent, const std::vector<BoxCollider*>& _wallColliders, GameObject* _target)
    : GameObject(parent, "HomingMagicSphere"), 
	  magicModel_(-1), 
	  attackTimer_(0.0f), 
	  effectData_(), 
	  hEmit_(-1), 
	  pCollider_(nullptr),
	  wallColliders_(_wallColliders), 
      target_(_target)
{
}

HomingMagicSphere::~HomingMagicSphere()
{
}

void HomingMagicSphere::Initialize()
{
	//仮に箱モデルを使う
	magicModel_ = Model::Load("Models/magicsphere.fbx");
	assert(magicModel_ != -1);

	transform_.position_ = { 0.0f, 0.0f, 0.0f };
	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };
	transform_.scale_ = { MAGIC_SCALE, MAGIC_SCALE, MAGIC_SCALE };

	pCollider_ = new SphereCollider(XMFLOAT3(0.0f, 0.0f, 0.0f), COLLILDER_RADIUS);
	AddCollider(pCollider_);
	pCollider_->SetRole(Collider::Role::Attack);

	// 魔法のスフィアにエフェクトを追加
	effectData_.textureFileName = "Effects/flashC_B.png";
	effectData_.position = transform_.position_;
	effectData_.positionRnd = MAGIC_SPHERE_EFFECT_POS_RND;
	effectData_.direction = MAGIC_SPHERE_EFFECT_DIRECTION;      // その場で揺らぐ
	effectData_.directionRnd = MAGIC_SPHERE_EFFECT_DIRECTION_RND;
	effectData_.speed = MAGIC_SPHERE_EFFECT_SPEED;
	effectData_.speedRnd = MAGIC_SPHERE_EFFECT_SPEED_RND;
	effectData_.accel = MAGIC_SPHERE_EFFECT_ACCEL;
	effectData_.gravity = MAGIC_SPHERE_EFFECT_GRAVITY;
	effectData_.color = MAGIC_SPHERE_EFFECT_COLOR;   // 少し青白
	effectData_.deltaColor = MAGIC_SPHERE_EFFECT_DELTA_COLOR;
	effectData_.rotate = MAGIC_SPHERE_EFFECT_ROTATE;
	effectData_.rotateRnd = MAGIC_SPHERE_EFFECT_ROTATE_RND;
	effectData_.spin = MAGIC_SPHERE_EFFECT_SPIN;
	effectData_.size = MAGIC_SPHERE_EFFECT_SIZE;
	effectData_.sizeRnd = MAGIC_SPHERE_EFFECT_SIZE_RND;
	effectData_.scale = MAGIC_SPHERE_EFFECT_SCALE;   // ゆっくり膨らむ
	effectData_.lifeTime = MAGIC_SPHERE_EFFECT_LIFETIME;
	effectData_.delay = MAGIC_SPHERE_EFFECT_DELAY;                          // 継続的に発生
	effectData_.number = MAGIC_SPHERE_EFFECT_NUMBER;
	effectData_.isBillBoard = true;

	hEmit_ = VFX::Start(effectData_);

	attackTimer_ = 0.0f;
	speed_ = 60.0f;
}

void HomingMagicSphere::Update()
{
	float dt_ = GameTime::DeltaTime();
	attackTimer_ += dt_;

	if (target_ && !target_->IsDead())
	{
		// 自分とターゲットの位置
		XMFLOAT3 pos = transform_.position_;
		XMFLOAT3 targetPos = target_->GetPosition();

		const float dx = targetPos.x - pos.x;
		const float dy = targetPos.y - pos.y;
		const float dz = targetPos.z - pos.z;

		// 目標方向ベクトルの長さ
		const float distSq_ = dx * dx + dy * dy + dz * dz;
		if (distSq_ > 0.0001f)
		{
			const float dist_ = std::sqrt(distSq_);

			// ヨー（左右回転） : XZ平面での角度
			float desiredYawRad = std::atan2(dx, dz); // XZ平面での角度

			// ピッチ（上下回転） : 高さdyと水平距離から計算
			float horizontalDist = std::sqrt(dx * dx + dz * dz);
			// 上方向を+pitchとする
			float desiredPitchRad = std::atan2f(dy, horizontalDist);

			// 現在の回転をラジアンで取得
			float currentYawRad = XMConvertToRadians(transform_.rotate_.y);
			float currentPitchRad = XMConvertToRadians(transform_.rotate_.x);

			// 差分を計算
			float diffYaw = desiredYawRad - currentYawRad;
			while (diffYaw > XM_PI) diffYaw -= 2 * XM_PI;
			while (diffYaw < -XM_PI) diffYaw += 2 * XM_PI;

			// ピッチは-90度から+90度の範囲に収める
			float diffPitch = desiredPitchRad - currentPitchRad;

			// LERP係数
			const float turnLerp = std::clamp(5.0f * dt_, 0.0f, 1.0f);

			float newYawRad = currentYawRad + diffYaw * turnLerp;
			float newPitchRad = currentPitchRad + diffPitch * turnLerp;

			// 回転を度に変換して保存
			transform_.rotate_.y = XMConvertToDegrees(newYawRad);
			transform_.rotate_.x = XMConvertToDegrees(newPitchRad);

			// ロールは0と仮定する
			float cp = std::cosf(newPitchRad);
			float sp = std::sinf(newPitchRad);
			float cy = std::cosf(newYawRad);
			float sy = std::sinf(newYawRad);

			// 前方向ベクトルを計算
			float vx = -sy * cp;
			float vy = sp;
			float vz = -cy * cp;

			transform_.position_.x += vx * speed_ * dt_;
			transform_.position_.y += vy * speed_ * dt_;
			transform_.position_.z += vz * speed_ * dt_;
		}
		else
		{
			// ターゲットがいないときはそのまま直進（3D）
			float yawRad = DirectX::XMConvertToRadians(transform_.rotate_.y);
			float pitchRad = DirectX::XMConvertToRadians(transform_.rotate_.x);

			float cp = std::cosf(pitchRad);
			float sp = std::sinf(pitchRad);
			float cy = std::cosf(yawRad);
			float sy = std::sinf(yawRad);

			float vx = -sy * cp;
			float vy = sp;
			float vz = -cy * cp;

			transform_.position_.x += vx * speed_ * dt_;
			transform_.position_.y += vy * speed_ * dt_;
			transform_.position_.z += vz * speed_ * dt_;
		}

		// エフェクトの追従
		effectData_.position = transform_.position_;
		if (hEmit_ >= 0)
		{
			VFX::SetEmitterPosition(hEmit_, effectData_.position);
		}

		if (attackTimer_ >= ATTACK_DURATION)
		{
			VFX::End(hEmit_);
			KillMe();
		}
	}
	
}

void HomingMagicSphere::Draw()
{
}

void HomingMagicSphere::Release()
{
}

void HomingMagicSphere::OnCollision(GameObject* pTarget)
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
