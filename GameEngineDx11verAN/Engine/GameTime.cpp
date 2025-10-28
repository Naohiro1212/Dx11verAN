#include "GameTime.h"
#include <chrono>

namespace {
    using clock = std::chrono::steady_clock;
    clock::time_point sPrev;
    float sDelta = 0.0f;
    bool sInitialized = false;
}

void GameTime::Initialize() {
    sPrev = std::chrono::steady_clock::now();
    sDelta = 0.0f;
    sInitialized = true;
}

void GameTime::Update() {
    if (!sInitialized) Initialize();

    auto now = std::chrono::steady_clock::now();
    sDelta = std::chrono::duration<float>(now - sPrev).count();
    sPrev = now;

    // 安全のためのクランプ（任意）：負値防止・上限0.1秒（最低10FPS相当）
    if (sDelta < 0.0f) sDelta = 0.0f;
    if (sDelta > 0.1f) sDelta = 0.1f;
}

float GameTime::DeltaTime() {
    return sDelta;
}