#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/VFX.h"
#include <vector>

class SphereCollider;
class BoxCollider;
class testEnemy;

class HomingMagicSphere : public GameObject
{
public:
    // コンストラクタ
    HomingMagicSphere(GameObject* parent);
	HomingMagicSphere(GameObject* parent, const std::vector<BoxCollider*> _wallColliders, testEnemy* _target);
    
	// デストラクタ
	~HomingMagicSphere();

	// 初期化
    void Initialize() override;
    
	// 更新
    void Update() override;
    
	// 描画
    void Draw() override;
    
	// 解放
    void Release() override;

	void OnCollision(GameObject* pTarget) override;

private:
    std::vector<BoxCollider*> wallColliders_;
	SphereCollider* pCollider_;
    testEnemy* target_;
    float attackTimer_;
    float speed_;

    int magicModel_;
	EmitterData effectData_;
	int hEmit_;
};