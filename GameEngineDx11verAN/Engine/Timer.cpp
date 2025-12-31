#include "Timer.h"
#include <sstream>
#include <iomanip>

namespace
{
	// 内部共有状態（ファイルスコープの静的変数）
	static bool running = false;
	static float elapsedSeconds = 0.0f;
}

namespace Timer
{
	void Initialize()
	{
		running = false;
		elapsedSeconds = 0.0f;
	}

	void Start()
	{
		running = true;
	}

	void Stop()
	{
		running = false;
	}

	void Reset()
	{
		running = false;
		elapsedSeconds = 0.0f;
	}

	void Restart()
	{
		elapsedSeconds = 0.0f;
		running = true;
	}

	void Update()
	{
		if (running)
		{
			elapsedSeconds += GameTime::DeltaTime();
		}
	}

	float GetSeconds()
	{
		return elapsedSeconds;
	}

	int GetMilliSeconds()
	{
		return static_cast<int>(elapsedSeconds * 1000.0f);
	}

	std::string ToString()
	{
		int totalMs = GetMilliSeconds();
		int ms = totalMs % 1000;
		int totalSec = totalMs / 1000;
		int sec = totalSec % 60;
		int min = totalSec / 60;

		std::ostringstream oss;
		oss << std::setfill('0') << std::setw(2) << min << ":"
			<< std::setfill('0') << std::setw(2) << sec << "."
			<< std::setfill('0') << std::setw(3) << ms;
		return oss.str();
	}

	bool IsRunning()
	{
		return running;
	}
}