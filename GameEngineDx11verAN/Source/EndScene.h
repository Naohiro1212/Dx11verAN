#pragma once
#include "../Engine/GameObject.h"
#include <chrono>

class Button;
class Text;

class EndScene : public GameObject
{
public:
	// コンストラクタ
	// 引数：parent  親オブジェクト（SceneManager）
	EndScene(GameObject* parent);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;

private:
	int EndImage_;

	// クリックしたらタイトルに戻るボタン
	Button* pButton_;

	Transform bgTransform_;

	// スコアテキスト
	Text* pScoreText_;

	// クリアタイマー
	Text* pClearTimeText_;

	// BGM
	int bgmHandle_;
	int clickSoundHandle_;

	// 遅延実行用タイマー
	bool pendingStart_;
	std::chrono::steady_clock::time_point startTriggerTime_;
	std::chrono::milliseconds startDelay_;
};