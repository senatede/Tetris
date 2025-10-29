#pragma once
#include "LeaderboardEntry.h"

#include <string>
#include <iostream>
#include <algorithm>


class Leaderboard {
    std::vector<LeaderboardEntry> entries;
    size_t maxEntries;
    std::string filepath;

public:
    Leaderboard(size_t max, const std::string &filepath);
    Leaderboard(const Leaderboard&) = delete;
    Leaderboard& operator=(const Leaderboard&) = delete;

    static Leaderboard& getInstance(size_t max = 5, const std::string &filepath = "leaderboard");

    bool isAGoodScore(long long score) const;
    bool isANewRecord(long long score) const;

    void addEntry(const std::string& name, long long score);

    std::vector<LeaderboardEntry> getEntries() const;

    void save() const;
    void load();

    void printLeaderboard() const;
};