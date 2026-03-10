#include "PausePanel.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include <assert.h>
#include "../Engine/Button.h"
#include "../Engine/Input.h"
#include "../Source/Player.h"
#include "../Engine/Text.h"

namespace
{
	const float HALF_WINDOW_WIDTH = 0.5f;
	const int RESUME_BUTTON_OFFSET_Y = -50;
	const int BACK_TITLE_BUTTON_OFFSET_Y = 120;

	const float TEXT_OFFSET_X_RATIO = 0.35f;         // 画面幅に対するテキストXオフセットの割合
	const float CENTER_OFFSET_X_RATIO = 0.15f;       // 画面幅に対するラベルと値の間隔割合
	const float LEVEL_TEXT_OFFSET_Y_RATIO = -0.22f;  // 画面高に対するレベルYオフセット割合
	const float STRENGTH_TEXT_OFFSET_Y_RATIO = -0.16f; // 画面高に対する攻撃力Yオフセット割合
	const float HEALTH_OFFSET_Y_DIFF_RATIO = 0.06f;  // 体力表示のY追加オフセット割合
	const float MANA_OFFSET_Y_DIFF_RATIO = 0.12f;    // マナ表示のY追加オフセット割合

	const float LEVEL_TEXT_SCALE_BASE = 1.7f;        // テキストの基準スケール
	const float LEVEL_TEXT_SCALE_BASE_HEIGHT = 1080.0f; // スケール基準画面高さ

	const float PANEL_SCALE = 0.3f;                  // パネル画像の最大スケール割合
	const float BUTTON_SCALE_X_RATIO = 0.20f;        // ボタン幅の画面幅に対する割合
	const float BUTTON_SCALE_Y_RATIO = 0.14f;        // ボタン高さの画面高に対する割合

	const int VALUE_SLASH_OFFSET = 100;               // 「/」の表示Xオフセット
	const int VALUE_MAX_OFFSET = 160;                // 最大値の表示Xオフセット
}

PausePanel::PausePanel(GameObject* parent) :
	GameObject(parent, "PausePanel"),
	panelImage_(-1),
	playerLevel_(1),
	playerStrength_(1),
	nowPaused_(false),
	resumeButton_(nullptr),
	BackTitleButton_(nullptr),
	onBTButton_(false),
	onResumeButton_(false),
	player_(nullptr)
{
}

void PausePanel::Initialize()
{
	panelImage_ = Image::Load("PausePanel.png");
	assert(panelImage_ != -1);

	Image::ResetRect(panelImage_);

	RECT rect = Image::GetRect(panelImage_);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);

	Image::SetPositionPixels(panelImage_, Direct3D::screenWidth_ * HALF_WINDOW_WIDTH,
		Direct3D::screenHeight_ * HALF_WINDOW_WIDTH, true);

	resumeButton_ = Instantiate<Button>(this);
	resumeButton_->SetCenter(true);
	resumeButton_->SetButtonImage(Image::Load("ResumeButton.png"));
	resumeButton_->SetButtonPosition(Direct3D::screenWidth_ * HALF_WINDOW_WIDTH,
		Direct3D::screenHeight_ * HALF_WINDOW_WIDTH + RESUME_BUTTON_OFFSET_Y);

	BackTitleButton_ = Instantiate<Button>(this);
	BackTitleButton_->SetCenter(true);
	BackTitleButton_->SetButtonImage(Image::Load("BackTitleButton.png"));
	BackTitleButton_->SetButtonPosition(Direct3D::screenWidth_ * HALF_WINDOW_WIDTH,
		Direct3D::screenHeight_ * HALF_WINDOW_WIDTH + BACK_TITLE_BUTTON_OFFSET_Y);

	player_ = dynamic_cast<Player*>(FindObject("Player"));
	levelText_ = new Text();
	levelText_->Initialize();
	levelText_->SetScale(LEVEL_TEXT_SCALE_BASE);

	nowPaused_ = false;

	SetDrawOrder(2);
	SetIsUIObject(true);

	resumeButton_->Invisible();
	BackTitleButton_->Invisible();
}

void PausePanel::Update()
{
	XMFLOAT3 mousePos = Input::GetMousePosition();

	resumeButton_->Update();
	onResumeButton_ = resumeButton_->GetOnButton();
	BackTitleButton_->Update();
	onBTButton_ = BackTitleButton_->GetOnButton();

	Image::SetPositionPixels(panelImage_, Direct3D::screenWidth_ * HALF_WINDOW_WIDTH,
		Direct3D::screenHeight_ * HALF_WINDOW_WIDTH, true);
	resumeButton_->SetButtonPosition(Direct3D::screenWidth_ * HALF_WINDOW_WIDTH,
		Direct3D::screenHeight_ * HALF_WINDOW_WIDTH + RESUME_BUTTON_OFFSET_Y);
	BackTitleButton_->SetButtonPosition(Direct3D::screenWidth_ * HALF_WINDOW_WIDTH,
		Direct3D::screenHeight_ * HALF_WINDOW_WIDTH + BACK_TITLE_BUTTON_OFFSET_Y);

	if (nowPaused_)
	{
		if (onResumeButton_ && Input::IsMouseButtonDown(0))
		{
			nowPaused_ = false;
			Input::ConsumeMouseButtons();
		}
	}
	playerLevel_ = player_->GetLevel();
	playerStrength_ = player_->GetStrength();
}

void PausePanel::Draw()
{
	if (!nowPaused_)
	{
		resumeButton_->Invisible();
		BackTitleButton_->Invisible();
		return;
	}

	const float centerX = Direct3D::screenWidth_ * HALF_WINDOW_WIDTH;
	const float centerY = Direct3D::screenHeight_ * HALF_WINDOW_WIDTH;

	const float textOffsetX = Direct3D::screenWidth_ * TEXT_OFFSET_X_RATIO;
	const float centerOffsetX = Direct3D::screenWidth_ * CENTER_OFFSET_X_RATIO;
	const float levelTextOffsetY = Direct3D::screenHeight_ * LEVEL_TEXT_OFFSET_Y_RATIO;
	const float strengthTextOffsetY = Direct3D::screenHeight_ * STRENGTH_TEXT_OFFSET_Y_RATIO;
	const float healthOffsetY = strengthTextOffsetY + Direct3D::screenHeight_ * HEALTH_OFFSET_Y_DIFF_RATIO;
	const float manaOffsetY = strengthTextOffsetY + Direct3D::screenHeight_ * MANA_OFFSET_Y_DIFF_RATIO;

	const float textScale = Direct3D::screenHeight_ / LEVEL_TEXT_SCALE_BASE_HEIGHT * LEVEL_TEXT_SCALE_BASE;
	levelText_->SetScale(textScale);

	RECT rect = Image::GetRect(panelImage_);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);

	float scaleX = Direct3D::screenWidth_ / w;
	float scaleY = Direct3D::screenHeight_ / h;
	float scale = (std::max)(scaleX, scaleY) * PANEL_SCALE;

	Image::SetSizePixels(panelImage_, w * scale, h * scale);
	Image::SetPositionPixels(panelImage_, centerX, centerY, true);
	Image::Draw(panelImage_);

	RECT rectButton = Image::GetRect(resumeButton_->GetButtonImage());
	float wResume = (float)(rectButton.right - rectButton.left);
	float hResume = (float)(rectButton.bottom - rectButton.top);

	float scaleButtonX = Direct3D::screenWidth_ / wResume * BUTTON_SCALE_X_RATIO;
	float scaleButtonY = Direct3D::screenHeight_ / hResume * BUTTON_SCALE_Y_RATIO;

	resumeButton_->SetButtonScale(scaleButtonX, scaleButtonY);
	BackTitleButton_->SetButtonScale(scaleButtonX, scaleButtonY);

	resumeButton_->Visible();
	BackTitleButton_->Visible();

	// レベル表示
	levelText_->Draw(centerX + textOffsetX - centerOffsetX, centerY + levelTextOffsetY, "LEVEL:");
	levelText_->Draw(centerX + textOffsetX, centerY + levelTextOffsetY, playerLevel_);

	// 攻撃力表示
	levelText_->Draw(centerX + textOffsetX - centerOffsetX, centerY + strengthTextOffsetY, "STRENGTH:");
	levelText_->Draw(centerX + textOffsetX, centerY + strengthTextOffsetY, playerStrength_);

	// 最大体力と現在の体力表示
	levelText_->Draw(centerX + textOffsetX - centerOffsetX, centerY + healthOffsetY, "HEALTH:");
	levelText_->Draw(centerX + textOffsetX, centerY + healthOffsetY, player_->GetHealth());
	levelText_->Draw(centerX + textOffsetX + VALUE_SLASH_OFFSET, centerY + healthOffsetY, "/");
	levelText_->Draw(centerX + textOffsetX + VALUE_MAX_OFFSET, centerY + healthOffsetY, player_->GetMaxHealth());

	// 最大マナと現在のマナ表示
	levelText_->Draw(centerX + textOffsetX - centerOffsetX, centerY + manaOffsetY, "MANA:");
	levelText_->Draw(centerX + textOffsetX, centerY + manaOffsetY, player_->GetMana());
	levelText_->Draw(centerX + textOffsetX + VALUE_SLASH_OFFSET, centerY + manaOffsetY, "/");
	levelText_->Draw(centerX + textOffsetX + VALUE_MAX_OFFSET, centerY + manaOffsetY, player_->GetMaxMana());
}

void PausePanel::Release()
{
}