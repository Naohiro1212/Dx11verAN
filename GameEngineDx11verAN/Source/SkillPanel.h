#pragma once
#include "../Engine/GameObject.h"
#include <vector>
#include <string>

class Player;
class Button;
class Text;

enum SkillType
{
	None = 0,
	Homing,
	PowerUp,
	HealthUp,
	ManaUp
};

/// <summary>
/// レベルアップに伴うスキル選択パネルの情報をまとめる構造体
/// </summary>
struct SkillSelectPanelInfo
{
	SkillType skillType; // スキルの種類
	Button* button_; // スキル選択のボタンオブジェクト
	bool onButton_;
	Transform buttonTransform_;
};

class SkillPanel : public GameObject
{
public:
	// コンストラクタ
	// 引数：parent  親オブジェクト（PlayScene）
	SkillPanel(GameObject* parent);
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 開放
	void Release() override;

	bool GetPanelOnButton(int _index);
	bool IsSelecting() { return nowSelecting_; }
	void SetSelecting(bool _selecting);

private:

	/// <summary>
	/// 最初にスキル選択パネルの情報をまとめてロードしておく関数
	/// </summary>
	void LoadPanelInfo(); 

	/// <summary>
	/// ランダムでパネルが選ばれたときの処理をする関数
	/// </summary>
	void RandomSelectPanel();

	// スキル選択パネルの情報をまとめる構造体のベクター
	std::vector<SkillSelectPanelInfo> skillSelectPanels_;

	bool nowSelecting_;

	Player* player_;

	// ランダムで選ばれたパネルの情報をまとめる構造体のベクター
	std::vector<SkillSelectPanelInfo> setPanels_;

	// ホーミングカードが出るのは一回きりなので、選択されたかどうかを管理するフラグ
	bool homingSelected_;

	// 表示するたびにランダムで選ばせるためのフラグ
	bool randomSelected_;
};