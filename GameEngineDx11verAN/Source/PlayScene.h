#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/Model.h"

class DungeonManager;
class ManaGauge;
class HealthGauge;
class Player;
class PausePanel;
class SkillPanel;
class ObjectiveText;

//テストシーンを管理するクラス
class PlayScene : public GameObject
{
public:
	//コンストラクタ
	//引数：parent  親オブジェクト（SceneManager）
	PlayScene(GameObject* parent);

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

	SkillPanel* skillPanel_;
	bool isSelecting_;

	// 目的テキスト
	ObjectiveText* objectiveText_;

	// BGM
	int bgmHandle_;
};