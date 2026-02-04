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
	static void AddScore(int delta);
	static int GetScore() { return score_; }
	static void Reset();

private:
	static int score_;
};