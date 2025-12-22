#pragma once
#include "../Engine/GameObject.h"

// 階層移動するためのポータルクラス
class Portal : public GameObject
{
public:
	// コンストラクタ
	// 引数：parent  親オブジェクト（SceneManager）
	Portal(GameObject* parent);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;

private:
	int portalModel_;

	// ポータルに近づいたらムービー演出と共にダンジョン再生成する
	// 描画を暗くして、ムービー演出を入れる予定
};