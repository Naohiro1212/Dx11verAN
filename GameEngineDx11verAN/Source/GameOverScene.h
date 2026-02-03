#pragma once
#include "../Engine/GameObject.h"
#include <chrono>

class Button;

class GameOverScene : public GameObject
{
public:
	//コンストラクタ
	GameOverScene(GameObject* parent);
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//開放
	void Release() override;

private:
	// ゲームオーバーの背景
	int gameOverImage_;
	Transform bgTransform_;

	// BGM
	int bgmHandle_;


	// クリックでタイトルに戻るボタン
	Button* pButton_;
	int clickSoundHandle_;

	// 遅延実行用タイマー
	bool pendingStart_;
	std::chrono::steady_clock::time_point restartTriggerTime_;
	std::chrono::milliseconds restartDelay_;
};