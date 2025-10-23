#include "EnemyGenerator.h"
#include "DungeonData.h"

namespace
{
	const float MAPTILE_SIZE = 30.0f;
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

	// “G‚ğ¶¬‚·‚é•”‰®‚Ì’†SÀ•W‚ğŒvZ‚µ‚Ä“o˜^
	for (size_t i = 1; i < _dng->mapDivCount; ++i)
	{
		if (_dng->mapRoom[i].size() < 4)
		{
			continue;
		}

		// •”‰®‚Ì’†SÀ•W‚ğŒvZ
		float centerX = static_cast<float>(_dng->mapRoom[i][2] + _dng->mapRoom[i][0]) / 2.0f;
		float centerZ = static_cast<float>(_dng->mapRoom[i][3] + _dng->mapRoom[i][1]) / 2.0f;

		// À•W‚ğ“G‚ÌƒXƒ|[ƒ“ˆÊ’u‚Æ‚µ‚Ä“o˜^
		_spawn.push_back(XMFLOAT3(centerX * MAPTILE_SIZE, 0.0f, centerZ * MAPTILE_SIZE));
	}
}

