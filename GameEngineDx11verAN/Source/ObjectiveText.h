#pragma once
#include "../Engine/GameObject.h"

class DungeonManager;
class Text;
class Player;

// シーンの状況に応じたテキスト表示
// 敵がいるかどうか、プレイヤーがレベルアップしたかどうかなど
class ObjectiveText : public GameObject
{
	// コンストラクタ
public:
	ObjectiveText(GameObject* parent);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;
private:
	Text* objectiveText_;
	DungeonManager* dungeonManager_;
	Player* player;
};