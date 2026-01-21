#include "ObjectiveText.h"
#include "../Engine/Text.h"
#include "../Source/DungeonManager.h"
#include "../Source/Player.h"
#include "../Source/Portal.h"

namespace
{
	const float TEXTPOS_X = 100.0f;
	const float TEXTPOS_Y = 250.0f; // 体力バーの下

    const float NEXTAREA_TEXTPOS_X = 100.0f;
	const float NEXTAREA_TEXTPOS_Y = 300.0f;
}

ObjectiveText::ObjectiveText(GameObject* parent) : GameObject(parent), objectiveText_(nullptr), dungeonManager_(nullptr)
{
}

void ObjectiveText::Initialize()
{
	objectiveText_ = new Text();
	objectiveText_->Initialize();

	SetDrawOrder(1);

	// UIなのでポーズ時も描画されるようにする
	SetIsUIObject(true);

	dungeonManager_ = dynamic_cast<DungeonManager*>(GetParent()->FindChildObject("DungeonManager"));
	portal_ = dynamic_cast<Portal*>(GetParent()->FindChildObject("Portal"));
	assert(dungeonManager_ != nullptr);
}

void ObjectiveText::Update()
{
}

void ObjectiveText::Draw()
{
    if (dungeonManager_->GetEnemyCount() == 0)
    {
        if (objectiveText_)
        {
            objectiveText_->Draw(TEXTPOS_X, TEXTPOS_Y, "All enemies are defeat!");
        }
    }
    else
    {
        if (objectiveText_)
        {
            objectiveText_->Draw(TEXTPOS_X, TEXTPOS_Y, "Enemies alive!");
        }
    }

    // ポータルが近く、ポータルが起動している場合表示する
    if(dungeonManager_->GetNearPortal() && portal_->GetActive() == true)
    {
        if (objectiveText_)
        {
            objectiveText_->Draw(NEXTAREA_TEXTPOS_X, NEXTAREA_TEXTPOS_Y, "Press E to Next Area!");
        }
	}
    else
    {
        if (dungeonManager_->GetEnemyCount() == 0)
        {
            objectiveText_->Draw(NEXTAREA_TEXTPOS_X, NEXTAREA_TEXTPOS_Y, "Go to The Portal!");
        }
    }
}

void ObjectiveText::Release()
{
}
