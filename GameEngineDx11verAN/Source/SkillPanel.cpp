#include "SkillPanel.h"
#include "../Engine/Button.h"
#include "../Engine/Text.h"
#include "../Source/Player.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Input.h"
#include <random>
#include <algorithm>
#include <string>

namespace
{
	// 現段階でのスキルの最大枚数
	const int MAX_PANEL_NUM = 4;
	const int SHOW_PANEL_NUM = 2; // 表示するパネルの数
	const float HALF_WINDOW_WIDTH = 0.5f;
	const float PANEL_SPACING = 300.0f;

	const int UP_POWER = 5;
	const float UP_MAX_HEALTH = 10.0f;
	const float UP_MAX_MANA = 10.0f;
}

SkillPanel::SkillPanel(GameObject* parent) : GameObject(parent, "SkillPanel"),
	homingSelected_(false),
	randomSelected_(false)
{
}

void SkillPanel::Initialize()
{
	LoadPanelInfo();
	RandomSelectPanel();

	SetDrawOrder(1);
	SetIsUIObject(true);

	nowSelecting_ = false;

	for (int i = 0;i < MAX_PANEL_NUM; ++i)
	{
		skillSelectPanels_[i].button_->Invisible();
	}

	player_ = dynamic_cast<Player*>(FindObject("Player"));
}

void SkillPanel::Update()
{
	if (nowSelecting_ && !randomSelected_)
	{
		RandomSelectPanel();
	}

	XMFLOAT3 mousePos = Input::GetMousePosition();

	for (int i = 0;i < SHOW_PANEL_NUM; ++i)
	{
		setPanels_[i].button_->Update();
		setPanels_[i].onButton_ = setPanels_[i].button_->GetOnButton();
		
		// 画面中央を基準に左右に配置
		float centerX = (float)Direct3D::screenWidth_ * HALF_WINDOW_WIDTH;
		float centerY = (float)Direct3D::screenHeight_ * HALF_WINDOW_WIDTH;
		float offsetX = (i - (SHOW_PANEL_NUM - 1) / 2.0f) * PANEL_SPACING; // 左右に等間隔で配置

		setPanels_[i].button_->SetButtonPosition(centerX + offsetX, centerY);
	}

	if (nowSelecting_)
	{
		for (int i = 0;i < SHOW_PANEL_NUM; ++i)
		{
			if (setPanels_[i].onButton_ && Input::IsMouseButtonDown(0))
			{
				switch (setPanels_[i].skillType)
				{
				case Homing:
					player_->ChangeMagicType();
					homingSelected_ = true;
					break;

				case PowerUp:
					player_->AddStrength(UP_POWER);
					break;

				case HealthUp:
					player_->AddMaxHealth(UP_MAX_HEALTH);
					break;

				case ManaUp:
					player_->AddMaxMana(UP_MAX_MANA);
					break;
				}

				randomSelected_ = false;
				// スキル選択後はパネルを非表示にする
				nowSelecting_ = false;
				Input::ConsumeMouseButtons(); // クリックを消費して次のフレームで再度処理されないようにする
			}
		}
	}
}

void SkillPanel::Draw()
{
	if (nowSelecting_)
	{
		// スキル選択中はパネルを描画
		for (int i = 0;i < SHOW_PANEL_NUM; ++i)
		{
			setPanels_[i].button_->Visible();
		}
	}
	else
	{
		// スキル選択中でない場合はパネルを非表示
		for (int i = 0;i < SHOW_PANEL_NUM; ++i)
		{
			setPanels_[i].button_->Invisible();
		}
	}
}

void SkillPanel::Release()
{
}

bool SkillPanel::GetPanelOnButton(int _index)
{
	if (_index < 0 || _index >= MAX_PANEL_NUM)
	{
		return false; // インデックスが範囲外の場合はfalseを返す
	}

	return skillSelectPanels_[_index].onButton_;
}

void SkillPanel::SetSelecting(bool _selecting)
{
	nowSelecting_ = _selecting;
	if (nowSelecting_)
	{
		randomSelected_ = false;
	}
}

void SkillPanel::LoadPanelInfo()
{
	for (int i = 0;i < MAX_PANEL_NUM; ++i)
	{
		SkillSelectPanelInfo info;
		info.skillType = static_cast<SkillType>(i + 1); // スキルの種類を順番に割り当てる（Noneは0なので+1）
		info.button_ = Instantiate<Button>(this);
		info.button_->SetCenter(true);
		info.button_->SetButtonImage(Image::Load("SkillPanel" + std::to_string(i) + ".png"));
		
		//// 画面中央を基準に左右に配置
		//float centerX = Direct3D::screenWidth_ * HALF_WINDOW_WIDTH;
		//float centerY = Direct3D::screenHeight_ * HALF_WINDOW_WIDTH;
		//float offsetX = (i - (MAX_PANEL_NUM - 1) / 2.0f) * PANEL_SPACING; // 左右に等間隔で配置

		//info.button_->SetButtonPosition(centerX + offsetX, centerY);

		info.onButton_ = false;
		skillSelectPanels_.push_back(info);
	}
}

void SkillPanel::RandomSelectPanel()
{
	setPanels_.clear();

	std::vector<SkillSelectPanelInfo> candidates;
	candidates.reserve(MAX_PANEL_NUM);

	for (int i = 0; i < MAX_PANEL_NUM; ++i)
	{
		if (homingSelected_ && skillSelectPanels_[i].skillType == Homing)
		{
			continue;
		}
		candidates.push_back(skillSelectPanels_[i]);
	}

	std::random_device rd_;
	std::mt19937 gen(rd_());
	std::shuffle(candidates.begin(), candidates.end(), gen);

	int pickCount = (std::min)(SHOW_PANEL_NUM, (int)candidates.size());
	for (int i = 0; i < pickCount; ++i)
	{
		setPanels_.push_back(candidates[i]);
	}

	randomSelected_ = true;
}
