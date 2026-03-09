#pragma once
#include "../Engine/GameObject.h"
#include "PlayerCamera.h"
#include "PlayerConfig.h"
#include <vector>

class testEnemy;
class BillBoard;
class BoxCollider;
class LevelUpEffect;
class Plane;
class PlayerMovement;

enum ModelState
{
	IDLE,
	WALK,
	RUN,
	LEFTSTRAFE,
	RIGHTSTRAFE,
	BACKSTRAFE,
	JUMP,
	SLASH,
	DEATH
};

enum PlayerState
{
	NORMAL,
	ATTACK,
	MAGIC,
	LEVELUP,
	DEAD
};

enum MacicType
{
	NORMALMAGIC,
	HOMINGMAGIC
};

class Player : public GameObject
{
public:
	///コンストラクタ
	///引数：parent  親オブジェクト（SceneManager）
	Player(GameObject* parent);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 解放
	/// </summary>
	void Release() override;

	/// <summary>
	/// 衝突した場合の判定
	/// </summary>
	/// <param name="pTarget"></param>
	void OnCollision(GameObject* pTarget) override;

	/// <summary>
	/// 壁のコライダーをセットする
	/// </summary>
	/// <param name="colliders"></param>
	void SetWallColliders(const std::vector<BoxCollider*>& colliders) { wallColliders_ = colliders; }

	// マナ取得
	float GetMana() const { return mana_; }
	float GetExp() const { return exp_; }
	float GetMaxMana() const { return maxMana_; }

	// 体力取得
	int GetHealth() const { return health_; }
	int GetMaxHealth() const { return maxHealth_; }

	// デスタイマー取得
	float GetDeathTimer() const { return deathTimer_; }

	// 攻撃力取得
	int GetStrength() const { return strength_; }
	
	// 現在レベル取得
	int GetLevel() const {return level_; }

	PlayerCamera* GetCamera() const { return plvision_; }

	void UpdateState();

	/// <summary>
	/// 攻撃力増加
	/// </summary>
	void AddStrength(int amount) { strength_ += amount; }

	/// <summary>
	/// 最大マナ増加
	/// </summary>
	void AddMaxMana(int amount) { maxMana_ += amount; }

	/// <summary>
	/// 最大体力増加
	/// </summary>
	void AddMaxHealth(int amount) { maxHealth_ += amount; }

	/// <summary>
	/// 魔法の弾の種類を変更する
	/// </summary>
	bool ChangeMagicType();

private:
	//// 関数群

	/// <summary>
	/// モデルを変更する
	/// </summary>
	void ChangeModel();

	/// <summary>
	/// 魔法を発射する
	/// </summary>
	void ShootMagic();

	/// <summary>
	/// 近接攻撃
	/// </summary>
	void MeleeAttack();

	/// <summary>
	/// レベルアップに伴う実装
	/// </summary>
	void LevelUp();

	/// <summary>
	/// 移動に伴う音再生
	/// </summary>
	void PlayMoveSound();

	/// <summary>
	/// 死んだときの処理
	/// </summary>
	bool HandleDeath();

	/// <summary>
	/// マナ回復
	/// </summary>
	void RecoverMana();

	/// <summary>
	/// 攻撃用の方向ベクトル計算
	/// </summary>
	void CalculateAttackDir();

	// 固定したい高さ
	bool gFreezeY_ = true;

	int nowModel_ = -1;
	std::vector<int> Models_;

	MacicType magicType_;

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
	PlayerCamera* plvision_;

	//// 戦闘関連の変数
	bool isAttacking_ = false;
	float attackTimer_ = 0.0f;
	bool slashSoundPlayed_ = false;
	float damageCooldown_ = 0.0f;
	int strength_ = 10;
	int lastSlashFrame_ = 0;
	XMFLOAT3 magicDir_ = { 0.0f, 0.0f, 0.0f };
	BoxCollider* attackCollider_ = nullptr;

	//// ステータス関連の変数
	float health_ = 0.0f;
	float maxHealth_ = 100.0f;
	float mana_ = 0.0f;
	float maxMana_ = 100.0f;
	float exp_ = 0.0f;
	int level_ = 1;
	float deathTimer_ = 0.0f;

	//// 入力処理や当たり判定関連の変数
	XMFLOAT2 moveDir_;
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
	int levelUpSEHandle_;

	// 死亡判定
	bool isDead_ = false;
	bool deathAnimStopped_ = false;

	// 影のビルボード
	BillBoard* shadowBillboard_;
	// 地面判定用平面
	Plane* pPlane_;

	PlayerMovement* movement_;

	// 敵の配列
	std::vector<testEnemy*> enemies_;

	// プレイヤーのState
	PlayerState state_;
};