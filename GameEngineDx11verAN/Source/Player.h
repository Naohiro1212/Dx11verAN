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

	// 攻撃力取得
	float GetStrength() const { return strength_; }

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

	// レベルアップのステータス処理
	void LevelUp();

	// 移動に伴う音再生
	void PlayMoveSound();

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
	bool slashSoundPlayed_ = false;
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

	// 音関係変数
	int hitSEHandle_;
	int swingSEHandle_;
	int moveSEHandle_;
	int strafeSEHandle_;
	int shootSEHandle_;
	int jumpSEHandle_;
	int ongroundSEHandle_;

	// 近接攻撃の一振り中の状態管理
	bool attackHitThisSwing_ = false;
	bool attackSoundPlayedThisSwing_ = false;

	// プレイヤー設定読み込み用変数
	PlayerConfig cnf_;

	// deltaTime 保存用
	float dt_;

	// レベルアップエフェクト
	LevelUpEffect* levelUpEffect_;

	// 死亡判定
	bool isDead_ = false;
	bool deathAnimStopped_ = false;
};