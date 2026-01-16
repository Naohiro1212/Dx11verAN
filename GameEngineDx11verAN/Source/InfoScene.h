#pragma once
#include "../Engine/GameObject.h"

class Text;

class InfoScene : public GameObject
{
	public:
	// コンストラクタ
	// 引数：parent  親オブジェクト（SceneManager）
	InfoScene(GameObject* parent);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;

private:
	int InfoImages_[2];

	Text* pInfoText_;
	Transform bgTransform_;
	int currentPage = 0;

	int bgmHandle_;

	bool prevMouseLeftDown_ = false;
};