#pragma once
#include "../Engine/GameObject.h"

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
    int modelHandle_;
	BoxCollider* pCollider_;
};