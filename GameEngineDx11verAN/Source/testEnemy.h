#pragma once
#include "../Engine/GameObject.h"
#include <vector>

class BoxCollider;

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

private:
    // 死んだ際にアイテムドロップ
    void DropJewel(int numJewels);

    // 壁ずり処理
	XMFLOAT3 SlideAlongWall(const XMFLOAT3& f, const XMFLOAT3& n);

    int modelHandle_;
	BoxCollider* pCollider_;
    std::vector<BoxCollider*> enemyWallColliders_;
};