#include "BoxCollider.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Model.h"
#include "Transform.h"
#include <assert.h>

//コンストラクタ
Collider::Collider():
	pGameObject_(nullptr)
{
	hDebugModel_ = Model::Load("Box.fbx");
	assert(hDebugModel_ != -1);
}

//デストラクタ
Collider::~Collider()
{
}

//箱型同士の衝突判定
//引数：boxA	１つ目の箱型判定
//引数：boxB	２つ目の箱型判定
//戻値：接触していればtrue
bool Collider::IsHitBoxVsBox(BoxCollider* boxA, BoxCollider* boxB)
{

	XMFLOAT3 boxPosA = Transform::Float3Add(boxA->pGameObject_->GetWorldPosition(), boxA->center_);
	XMFLOAT3 boxPosB = Transform::Float3Add(boxB->pGameObject_->GetWorldPosition(), boxB->center_);


	if ((boxPosA.x + boxA->size_.x / 2) > (boxPosB.x - boxB->size_.x / 2) &&
		(boxPosA.x - boxA->size_.x / 2) < (boxPosB.x + boxB->size_.x / 2) &&
		(boxPosA.y + boxA->size_.y / 2) > (boxPosB.y - boxB->size_.y / 2) &&
		(boxPosA.y - boxA->size_.y / 2) < (boxPosB.y + boxB->size_.y / 2) &&
		(boxPosA.z + boxA->size_.z / 2) > (boxPosB.z - boxB->size_.z / 2) &&
		(boxPosA.z - boxA->size_.z / 2) < (boxPosB.z + boxB->size_.z / 2))
	{
		return true;
	}
	return false;
}

//箱型と球体の衝突判定
//引数：box	箱型判定
//引数：sphere	２つ目の箱型判定
//戻値：接触していればtrue
bool Collider::IsHitBoxVsCircle(BoxCollider* box, SphereCollider* sphere)
{
	XMFLOAT3 circlePos = Transform::Float3Add(sphere->pGameObject_->GetWorldPosition(), sphere->center_);
	XMFLOAT3 boxPos = Transform::Float3Add(box->pGameObject_->GetWorldPosition(), box->center_);

	// Box の半径（ハーフサイズ）
	const float hx = box->size_.x * 0.5f;
	const float hy = box->size_.y * 0.5f;
	const float hz = box->size_.z * 0.5f;

	// Sphere 半径（size_.x が半径である前提。もし直径なら 0.5 を掛けてください）
	const float r = sphere->size_.x;

	// ボックスの各軸の最小・最大
	const float minX = boxPos.x - hx;
	const float maxX = boxPos.x + hx;
	const float minY = boxPos.y - hy;
	const float maxY = boxPos.y + hy;
	const float minZ = boxPos.z - hz;
	const float maxZ = boxPos.z + hz;

	// 球中心をボックス範囲にクランプして最近接点を求める
	const float closestX = (std::max)(minX, (std::min)(circlePos.x, maxX));
	const float closestY = (std::max)(minY, (std::min)(circlePos.y, maxY));
	const float closestZ = (std::max)(minZ, (std::min)(circlePos.z, maxZ));

	// 最近接点と球中心の距離の二乗
	const float dx = circlePos.x - closestX;
	const float dy = circlePos.y - closestY;
	const float dz = circlePos.z - closestZ;

	const float dist2 = dx * dx + dy * dy + dz * dz;
	return dist2 <= r * r;
}

//球体同士の衝突判定
//引数：circleA	１つ目の球体判定
//引数：circleB	２つ目の球体判定
//戻値：接触していればtrue
bool Collider::IsHitCircleVsCircle(SphereCollider* circleA, SphereCollider* circleB)
{
	XMFLOAT3 centerA = circleA->center_;
	XMFLOAT3 positionA = circleA->pGameObject_->GetWorldPosition();
	XMFLOAT3 centerB = circleB->center_;
	XMFLOAT3 positionB = circleB->pGameObject_->GetWorldPosition();

	XMVECTOR v = (XMLoadFloat3(&centerA) + XMLoadFloat3(&positionA))
		- (XMLoadFloat3(&centerB) + XMLoadFloat3(&positionB));

	if (XMVector3Length(v).m128_f32[0] <= circleA->size_.x + circleB->size_.x)
	{
		return true;
	}

	return false;
}

//テスト表示用の枠を描画
//引数：position	オブジェクトの位置
void Collider::Draw(XMFLOAT3 position, XMFLOAT3 rotate)
{
	Transform transform;
	transform.position_ = XMFLOAT3(position.x + center_.x, position.y + center_.y, position.z + center_.z);
	transform.scale_ = size_;
	transform.rotate_ = rotate;
	transform.Calclation();
	Model::SetTransform(hDebugModel_, transform);
	Model::Draw(hDebugModel_);
}

PenetrationResult Collider::ComputeBoxVsBoxPenetration(BoxCollider* boxA, BoxCollider* boxB)
{
	PenetrationResult result = { false, {0,0,0},{0,0,0} };

	XMFLOAT3 posA = Transform::Float3Add(boxA->pGameObject_->GetWorldPosition(), boxA->center_);
	XMFLOAT3 posB = Transform::Float3Add(boxB->pGameObject_->GetWorldPosition(), boxB->center_);
	XMFLOAT3 halfA = { boxA->size_.x * 0.5f, 0.0f, boxA->size_.z * 0.5f };
	XMFLOAT3 halfB = { boxB->size_.x * 0.5f, 0.0f, boxB->size_.z * 0.5f };

	bool overlap = 
		(posA.x + halfA.x > posB.x - halfB.x) &&
		(posA.x - halfA.x < posB.x + halfB.x) &&
		(posA.z + halfA.z > posB.z - halfB.z) &&
		(posA.z - halfA.z < posB.z + halfB.z);
	if (!overlap) return result;

	float pushPosX = (posB.x + halfB.x) - (posA.x - halfA.x);
	float pushNegX = (posA.x + halfA.x) - (posB.x - halfB.x);
	float pushPosZ = (posB.z + halfB.z) - (posA.z - halfA.z);
	float pushNegZ = (posA.z + halfA.z) - (posB.z - halfB.z);

	float pushX = (pushPosX < pushNegX) ? pushPosX : -pushNegX;
	float pushZ = (pushPosZ < pushNegZ) ? pushPosZ : -pushNegZ;

	float ax = fabsf(pushX);
	float az = fabsf(pushZ);
	result.overlapped = true;
	if (ax <= az)
	{
		result.push = { pushX, 0.0f, 0.0f };
		result.normal = { pushX > 0 ? -1.0f : 1.0f, 0.0f, 0.0f };
	}
	else
	{
		result.push = { 0.0f, 0.0f, pushZ };
		result.normal = { 0.0f, 0.0f, pushZ > 0 ? -1.0f : 1.0f };
	}
	return result;
}
