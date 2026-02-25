#pragma once
#include <DirectXMath.h>
#include <vector>
#include "PlayerConfig.h"
#include "../Engine/BoxCollider.h"
#include "../Engine/Transform.h"
#include "../Source/PlayerCamera.h"

using namespace DirectX;

class Plane;
class BoxCollider;

struct Dir
{
	int fwd_; // 前後（正のとき前、負のとき後ろ）
	int str_; // 左右（正のとき右、負のとき左）
};

class PlayerMovement
{
public:
	/// <summary>
	/// 引数でコンフィグを受け取るコンストラクタ
	/// </summary>
	PlayerMovement(PlayerConfig cnf, Transform& transform, Plane* pPlane, BoxCollider* pCollider, PlayerCamera& plvision);
	~PlayerMovement();
	void Initialize();

	// 引数で体力を受け取るのは、体力に応じて移動可能かどうかを判断するため
	void Update(bool isAttacking_, float health, const std::vector<BoxCollider*>& wallColliders);

	Dir GetMoveDir() const { return moveDir_; }
	bool IsLandedThisFrame() const { return landedThisFrame_; }

	/// <summary>
	/// 壁のコライダーを受け取って、壁との衝突を解決する関数
	/// </summary>
	/// <param name="wallColliders"></param>
	void ResolveWallCollisions(std::vector<BoxCollider*> wallColliders);

	/// <summary>
	/// カメラの向きにプレイヤーの向きを合わせる関数
	/// </summary>
	void UpdateYawToCamera(const XMFLOAT3& cameraForward, float& playerYaw);

	void SetCameraVision(PlayerCamera plvision) { plvision_ = plvision; }
private:
	// 前後左右、入力方向の取得
	void MoveInput();

	// ジャンプ
	void Jump();

	// 移動状態の更新
	void UpdateMovement();

	// 重力更新
	void UpdateGravity();

	// 壁ずり処理
	XMFLOAT3 SlideAlongWall(const XMFLOAT3& f, const XMFLOAT3& n);

	// カメラ基準の前方・右方ベクトルの計算
	void CalcCameraDirectionXZ();

	/// <summary>
	/// プレイヤーの移動やアクションに関する変数
	/// </summary>
	bool wasMoving_ = false;
	bool isMovingNow_ = false;
	bool prevOnGround_ = true;
	bool prevMouseLeftDown_ = false;
	bool onGround_ = true;
	float JumpV0_ = 0.0f;
	float velocityY_ = 0.0f;
	size_t jumpCount_ = 0;
	XMVECTOR vAirMove_ = XMVectorZero();

	// このフレームで着地したかどうか（着地音再生のため）
	bool landedThisFrame_ = false;

	/// <summary>
	/// 入力処理や当たり判定関連の変数
	/// </summary>
	Dir moveDir_;

	//　プレイヤー設定読み込み用変数
	PlayerConfig cnf_;

	// deltaTime 保存用
	float dt_;

	// Transformへの参照
	Transform& transform_;

	// 地面判定レイキャスト用の平面
	Plane* pPlane_;

	// 壁との当たり判定用のコライダー
	BoxCollider* pCollider_;

	XMFLOAT3 forward_ = {};
	XMFLOAT3 moveVec_;
	XMVECTOR vForward_ = XMVectorZero();
	XMFLOAT3 right_ = {};
	XMVECTOR vRight_ = XMVectorZero();
	PlayerCamera& plvision_;
};