#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/Model.h"

class DungeonManager;
class ManaGauge;
class HealthGauge;
class Player;
class PausePanel;
class ObjectiveText;

//テストシーンを管理するクラス
class TestScene : public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	TestScene(GameObject* parent);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//開放
	void Release() override;

	bool GetPaused() const { return isPaused_; }
	
private:
	// ポーズ中かどうかのフラグ
	bool isPaused_;
	DungeonManager* dungeonManager_;
	ManaGauge* manaGauge_;
	HealthGauge* healthGauge_;
	Player* player_;

	// ポーズパネル
	PausePanel* pausePanel_;

	// 目的テキスト
	ObjectiveText* objectiveText_;
};