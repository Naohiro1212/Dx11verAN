#pragma once
#include "../Engine/GameObject.h"
#include "PlayerCamera.h"
#include "PlayerConfig.h"
#include <vector>

class BoxCollider;

class Player :
    public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	Player(GameObject* parent);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;

	void OnCollision(GameObject* pTarget) override;

	XMFLOAT3 SlideAlongWall(const XMFLOAT3& f, const XMFLOAT3& n);

private:
	//// 関数群
	// 前後左右、入力方向の取得
	void MoveInput();

	// モデルチェンジ処理
	void ChangeModel();

	// 重力処理
	void UpdateGravity();

	// 魔法発射
	void ShootMagic();

	// 近接攻撃
	void MeleeAttack();

	// カメラ基準の前方・右ベクトルを計算
	void CalcCameraDirectionXZ();

	// ジャンプ
	void Jump();

	//// 変数群
	// モデルハンドル
	int walkModel_;
	int runModel_;
	int leftStrafeModel_;
	int rightStrafeModel_;
	int backStrafeModel_;
	int idleModel_;
	int slashModel_;
	int nowModel_;

	// カメラパラメーター
	float camYawRad_;
	float camPitchRad_;
	float camDistance_;

	float minPitchRad_;
	float maxPitchRad_;
	float minDistance_;
	float maxDistance_;

	PlayerCamera plvision_;

	bool wasMoving_;
	// ジャンプ関連
	float JumpV0_;
	float velocityY_;

	// 攻撃時間計測
	float attackTimer_;

	// ジャンプ上限
	size_t jumpCount_;

	// フラグ群
	bool onGround_;
	bool isAttacking_;
	bool prevMouseLeftDown_;
	bool isMovingNow_;
	float lastSlashFrame_;

	// 当たり判定
	BoxCollider* pCollider_;
	BoxCollider* attackCollider_;
	// 攻撃用コライダーの回転を反映した中心オフセット
	XMFLOAT3 rotateCenter_;

	// 魔法攻撃用方向ベクトル
	XMFLOAT3 magicDir_;

	// プレイヤーのコンフィグ
	PlayerConfig cnf_;

	// 当たり判定用壁コライダー
	std::vector<BoxCollider*> wallColliders_;

	// 入力方向の処理
	int fwd_;
	int str_;

	// デルタタイム
	float dt_;

	// カメラ基準の前方ベクトル（XZ平面、Y成分は0）
	XMFLOAT3 forward;

	// カメラ基準の前方ベクトル（DirectXMath型、正規化済み）
	XMVECTOR vForward;

	// カメラ基準の右方向ベクトル（XZ平面、Y成分は0）
	XMFLOAT3 right;

	// カメラ基準の右方向ベクトル（DirectXMath型、正規化済み）
	XMVECTOR vRight;
};