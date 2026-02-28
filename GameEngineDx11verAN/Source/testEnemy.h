#pragma once
#include "../Engine/GameObject.h"
#include <vector>
#include "../Engine/VFX.h"

class BoxCollider;
class Player;
class Plane;
class EnemyDeathEffect;
class EnemyDamageEffect;

//仮の敵を管理するクラス
class testEnemy : public GameObject
{

public:
    //コンストラクタ
    testEnemy(GameObject* parent);

    //デストラクタ
    ~testEnemy();

    //初期化
    void Initialize() override;

    //更新
    void Update() override;

    //描画
    void Draw() override;

    //開放
    void Release() override;

    //当たり判定
    void OnCollision(GameObject* pTarget) override;

    //Playerの位置を取得して、視認する
    void LookAtPlayer();

    //Playerに向かって移動する
    void MoveToPlayer();

    //Playerに対して攻撃する
    void AttackPlayer();

    void SetWallColliders(const std::vector<BoxCollider*>& colliders) { enemyWallColliders_ = colliders; }

    // セッター
    void SetPosition(const XMFLOAT3& pos);
    int GetAttackPower() { return attackPower_; }

    void ChangeModel();

private:
    // 死んだ際にアイテムドロップ
    void DropJewel(int numJewels);

    // 壁ずり処理
    XMFLOAT3 SlideAlongWall(const XMFLOAT3& f, const XMFLOAT3& n);

    // プレイヤーの位置を取得するためのポインタ
    Player* player_;

    BoxCollider* pCollider_;
    BoxCollider* attackCollider_;
    std::vector<BoxCollider*> enemyWallColliders_;

    // 敵の移動速度
    XMFLOAT3 velocity_;

    XMFLOAT3 moveVec_;

    // プレイヤーを見つけたかどうかのフラグ
    bool isSpotted_;

    // 開始地点に戻った後に徘徊するためのフラグ
    bool isReturning_;

    // プレイヤーを攻撃している状態かどうか
    bool isAttacking_;

    // 最初に居た座標
    XMFLOAT3 initPos_;
    float backTimer_;

    // 体力
    float health_;
    float damageCooldown_;

    // 死んだときのエフェクト
    EnemyDeathEffect* deathEffect_;
    float deathTimer_;

    // モデル
    int nowModel_;
    int idleModel_;
    int walkModel_;
    int deathModel_;
    int attackModel_;

    // ノックバック用の変数
    XMFLOAT3 knockbackVec_;
    float knockbackTimer_;

    float attackCooldown_;
    int lastAttackFrame_;

    // 攻撃力
    int attackPower_;

    // 影のビルボード
    BillBoard* shadowBillboard_;
    Plane* pPlane_;

    // 何秒に一回徘徊するかのタイマー
    float patrolTimer_;
    bool isPatrolMove_;

    // 流血エフェクト
    EnemyDamageEffect* damageEffect_;

};