#pragma once
#include "GameTime.h"
#include <string>

// Image と同様の名前空間形式で公開するタイマー
namespace Timer
{
	// 初期化（必要なら呼ぶ）
	void Initialize();

	// 開始 / 停止 / リセット / 再開始
	void Start();
	void Stop();
	void Reset();
	void Restart();

	// 毎フレーム呼ぶ（シーンか SceneManager の Update から）
	void Update();

	// 取得
	float GetSeconds();          // 経過秒（float）
	int GetMilliSeconds();      // ミリ秒（int）
	std::string ToString();     // mm:ss.ms 形式
	bool IsRunning();
}