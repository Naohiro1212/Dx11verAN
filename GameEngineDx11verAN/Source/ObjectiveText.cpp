#include "ObjectiveText.h"
#include "../Engine/Text.h"
#include "../Source/DungeonManager.h"

namespace
{
	const float textPosX_ = 100.0f;
	const float textPosY_ = 250.0f; // 体力バーの下
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
            objectiveText_->Draw(textPosX_, textPosY_, "All enemies are defeat!");
        }
    }
    else
    {
        if (objectiveText_)
        {
            objectiveText_->Draw(textPosX_, textPosY_, "Enemies alive!");
        }
    }
}

void ObjectiveText::Release()
{
}
