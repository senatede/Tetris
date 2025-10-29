#pragma once
#include <string>

struct LeaderboardEntry {
    std::string playerName;
    long long score;

    bool operator<(const LeaderboardEntry& other) const {
        return score > other.score;
    }
};