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
	rdn_ = new RandomNum();
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

		// 部屋の範囲
		int minX = _dng->mapRoom[i][0];
		int maxX = _dng->mapRoom[i][2];
		int minZ = _dng->mapRoom[i][1];
		int maxZ = _dng->mapRoom[i][3];

		// エリアにいる敵の数だけループ
		for (int j = 0;j < _dng->enemyCount; ++j)
		{
			// ランダムな位置を生成
			int randX = rdn_->GetRand(minX, maxX);
			int randZ = rdn_->GetRand(minZ, maxZ);

			float posX = randX * MAPTILE_SIZE;
			float posY = 0.0f;
			float posZ = randZ * MAPTILE_SIZE;

			_spawn.push_back(XMFLOAT3(posX, posY, posZ));
		}
	}
}

