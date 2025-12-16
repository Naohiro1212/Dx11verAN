#pragma once
#include "../Engine/GameObject.h"
#include <vector>

class BoxCollider;
class Player;

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

    void SetWallColliders(const std::vector<BoxCollider*>& colliders) { enemyWallColliders_ = colliders; }

private:
    // 死んだ際にアイテムドロップ
    void DropJewel(int numJewels);

    // 壁ずり処理
	XMFLOAT3 SlideAlongWall(const XMFLOAT3& f, const XMFLOAT3& n);

    // プレイヤーの位置を取得するためのポインタ
    Player* player_;

    int modelHandle_;
	BoxCollider* pCollider_;
    std::vector<BoxCollider*> enemyWallColliders_;

    // 敵の移動速度
    XMFLOAT3 velocity_;

    XMFLOAT3 moveVec_;

    // プレイヤーを見つけたかどうかのフラグ
    bool isSpoted_;
};