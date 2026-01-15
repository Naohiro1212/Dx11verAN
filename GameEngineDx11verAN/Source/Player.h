#pragma once
#include "../Engine/GameObject.h"
#include "PlayerCamera.h"
#include "PlayerConfig.h"
#include <vector>

class BoxCollider;
class LevelUpEffect;

class Player : public GameObject
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

	void SetWallColliders(const std::vector<BoxCollider*>& colliders) { wallColliders_ = colliders; }

	// マナ取得
	float GetMana() const { return mana_; }
	float GetExp() const { return exp_; }
	float GetMaxMana() const { return cnf_.MAX_MANA; }

	// 体力取得
	float GetHealth() const { return health_; }
	float GetMaxHealth() const { return cnf_.MAX_HEALTH; }

	// デスタイマー取得
	float GetDeathTimer() const { return deathTimer_; }

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

<<<<<<< HEAD
	// レベルアップのステータス処理
	void LevelUp();
=======
	// 移動に伴う音再生
	void PlayMoveSound();
>>>>>>> 2ca843891f497ac230d3f9410d2f01fcd588812d

	// 壁ずり処理
	XMFLOAT3 SlideAlongWall(const XMFLOAT3& f, const XMFLOAT3& n);

	// 固定したい高さ
	bool gFreezeY_ = true;

	//// モデル状態に関する変数
	int walkModel_ = -1;
	int runModel_ = -1;
	int leftStrafeModel_ = -1;
	int rightStrafeModel_ = -1;
	int backStrafeModel_ = -1;
	int idleModel_ = -1;
	int slashModel_ = -1;
	int nowModel_ = -1;
	int jumpModel_ = -1;
	int deathModel_ = -1;

	//// プレイヤーの移動やアクションに関する変数
	bool wasMoving_ = false;
	bool isMovingNow_ = false;
	bool prevOnGround_ = true;
	bool prevMouseLeftDown_ = false;
	bool onGround_ = true;
	float JumpV0_ = 0.0f;
	float velocityY_ = 0.0f;
	size_t jumpCount_ = 0;
	XMVECTOR vAirMove_ = XMVectorZero();

	//// カメラ関連の変数
	float camYawRad_ = 0.0f;
	float camPitchRad_ = 0.0f;
	float camDistance_ = 0.0f;
	float minPitchRad_ = -1.0f;
	float maxPitchRad_ = 1.0f;
	float minDistance_ = 1.0f;
	float maxDistance_ = 10.0f;
	XMFLOAT3 forward = {};
	XMVECTOR vForward = XMVectorZero();
	XMFLOAT3 right = {};
	XMVECTOR vRight = XMVectorZero();
	PlayerCamera plvision_;

	//// 戦闘関連の変数
	bool isAttacking_ = false;
	float attackTimer_ = 0.0f;
	float damageCooldown_ = 0.0f;
	float strength_ = 10.0f;
	float lastSlashFrame_ = 0.0f;
	XMFLOAT3 magicDir_ = { 0.0f, 0.0f, 0.0f };
	BoxCollider* attackCollider_ = nullptr;

	//// ステータス関連の変数
	float health_ = 0.0f;
	float mana_ = 0.0f;
	float exp_ = 0.0f;
	float deathTimer_ = 0.0f;

	//// 入力処理や当たり判定関連の変数
	int fwd_ = 0;
	int str_ = 0;
	BoxCollider* pCollider_ = nullptr;
	std::vector<BoxCollider*> wallColliders_;
	XMFLOAT3 rotateCenter_ = { 0.0f, 0.0f, 0.0f };

<<<<<<< HEAD
	//// その他
	float dt_ = 0.0f;
	LevelUpEffect* levelUpEffect_ = nullptr;
	PlayerConfig cnf_;
=======
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

	// 経験値
	float exp_;

	// 魔法を放つ際のマナ管理用変数
	float mana_;

	// 体力
	float health_;
	float damageCooldown_;

	// 死亡したのちに開始するタイマー
	// 一定時間経過後にタイトルへ戻る
	float deathTimer_;

	// 音関係変数
	int hitSEHandle_;
	int moveSEHandle_;
	int strafeSEHandle_;
	int shootSEHandle_;
	int jumpSEHandle_;
	int ongroundSEHandle_;
>>>>>>> 2ca843891f497ac230d3f9410d2f01fcd588812d
};