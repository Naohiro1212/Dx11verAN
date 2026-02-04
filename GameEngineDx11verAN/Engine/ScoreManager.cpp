#include "ScoreManager.h"

// 静的メンバ変数の定義
int ScoreManager::score_ = 0;

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
