#include "EnemyGenerator.h"
#include "DungeonData.h"

namespace
{
	// マップタイル1マスのサイズ
	const float MAPTILE_SIZE = 30.0f;
	// 区域・部屋の座標を表す配列の要素数
	const size_t AREA_COORD_COUNT = 4;
}

EnemyGenerator::EnemyGenerator()
{
}

void EnemyGenerator::GenerateEnemies(DungeonMap_Info* const _dng, std::vector<std::vector<MapData_RL>>& _maprl, std::vector<XMFLOAT3>& _spawn)
{
	if (!_dng)
	{
		return;
	}

	// 敵を生成する部屋の中心座標を計算して登録
	for (size_t i = 1; i < _dng->mapDivCount; ++i)
	{
		if (_dng->mapRoom[i].size() < AREA_COORD_COUNT)
		{
			continue;
		}

		// 部屋の中心座標を計算
		float centerX = static_cast<float>(_dng->mapRoom[i][2] + _dng->mapRoom[i][0]) / 2.0f;
		float centerY = 0.0f; // Y座標は固定
		float centerZ = static_cast<float>(_dng->mapRoom[i][3] + _dng->mapRoom[i][1]) / 2.0f;

		// 座標を敵のスポーン位置として登録
		_spawn.push_back(XMFLOAT3(centerX * MAPTILE_SIZE, centerY, centerZ * MAPTILE_SIZE));
	}
}

