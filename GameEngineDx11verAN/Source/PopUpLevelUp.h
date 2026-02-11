#pragma once
#include "../Engine/GameObject.h"

class BillBoard;

class PopUpLevelUp : public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	PopUpLevelUp(GameObject* parent);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//開放
	void Release() override;
private:
	float displayTime_;
	BillBoard* levelUpBillboard_;
	XMFLOAT4 color_;
};