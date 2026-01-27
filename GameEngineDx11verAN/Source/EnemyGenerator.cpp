#include "EnemyGenerator.h"
#include "DungeonData.h"
#include "../Engine/RandomNum.h"
#include "../Engine/Global.h"
#include <algorithm>

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

EnemyGenerator::~EnemyGenerator()
{
	SAFE_DELETE(rdn_);
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

        // 部屋の中でランダムな位置にスポーンさせる
        float minX = static_cast<float>(std::min<size_t>(_dng->mapRoom[i][0], _dng->mapRoom[i][2]));
        float maxX = static_cast<float>(std::max<size_t>(_dng->mapRoom[i][0], _dng->mapRoom[i][2]));
        float minZ = static_cast<float>(std::min<size_t>(_dng->mapRoom[i][1], _dng->mapRoom[i][3]));
        float maxZ = static_cast<float>(std::max<size_t>(_dng->mapRoom[i][1], _dng->mapRoom[i][3]));

        float spawnX = rdn_->GetRandF(minX, maxX);
        float spawnY = 0.0f;
        float spawnZ = rdn_->GetRandF(minZ, maxZ);

        // 座標を敵のスポーン位置として登録
        _spawn.push_back(XMFLOAT3(spawnX * MAPTILE_SIZE, spawnY, spawnZ * MAPTILE_SIZE));
    }
}

