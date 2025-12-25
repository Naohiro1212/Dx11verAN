#include "ScoreManager.h"

ScoreManager::ScoreManager(GameObject* parent) : GameObject(parent, "ScoreManager")
{
}

ScoreManager::~ScoreManager()
{
}

void ScoreManager::Initialize()
{
    score_ = 0;
}

void ScoreManager::Update()
{
}

void ScoreManager::Draw()
{
}

void ScoreManager::Release()
{
}

// スコアを増減させる
void ScoreManager::AddScore(int delta)
{
    score_ += delta;
}

// スコアをリセットする
void ScoreManager::Reset()
{
	score_ = 0;
}
