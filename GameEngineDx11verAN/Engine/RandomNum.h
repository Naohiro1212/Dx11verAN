#pragma once
#include <random>

class RandomNum {
public:
    RandomNum() : mt(rd()) {}

    // 0`max‚Ü‚Å‚Ì®”—”‚ð•Ô‚·
    int GetRand(int _max) {
        return GetRand(0, _max);
    }

    // min`max‚Ì®”—”‚ð•Ô‚·
    int GetRand(int min, int max) {
        if (min > max)
        {
            std::swap(min, max);
        }
        std::uniform_int_distribution<int> dist(min, max);
        return dist(mt);
    }

private:
    std::random_device rd;
    std::mt19937 mt;
};
