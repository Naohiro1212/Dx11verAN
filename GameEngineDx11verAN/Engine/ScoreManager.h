#pragma once
#include "../Engine/GameObject.h"

class ScoreManager : public GameObject
{
public:
	ScoreManager(GameObject* parent);
	~ScoreManager();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Release() override;

	// ÉXÉRÉAëÄçÏ
	void AddScore(int delta);
	int GetScore() const { return score_; }
	void Reset();

private:
	int score_;
};