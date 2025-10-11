#pragma once
#include <random>

class RandomNum {
public:
    RandomNum() : mt(rd()) {}

    // 0`max‚Ü‚Å‚Ì®”—”‚ğ•Ô‚·
    int GetRand(int max) {
        std::uniform_int_distribution<int> dist(0, max);
        return dist(mt);
    }

private:
    std::random_device rd;
    std::mt19937 mt;
};
