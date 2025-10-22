#pragma once
#include <vector>

class EnemyBox;

class EnemyGenerator
{
public:
	EnemyGenerator();
	void ClearEnemies();
	void GenerateEnemies();

private:
	std::vector<EnemyBox> enemies_;
}