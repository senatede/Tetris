#include "Leaderboard.h"

#include <vector>
#include <fstream>

Leaderboard::Leaderboard(const size_t max, const std::string &filepath) : maxEntries(max), filepath(filepath) {
    load();
}

Leaderboard& Leaderboard::getInstance(const size_t max, const std::string &filepath) {
    static Leaderboard instance(max, filepath);
    return instance;
}

bool Leaderboard::isAGoodScore(const long long score) const {
    return entries.size() < maxEntries || score >= entries.back().score;
};

bool Leaderboard::isANewRecord(const long long score) const {
    if (entries.empty()) return false;
    return score >= entries.front().score;
}

void Leaderboard::addEntry(const std::string& name, const long long score) {
    entries.push_back({name, score});
    std::sort(entries.begin(), entries.end());
    if (entries.size() > maxEntries) {
        entries.resize(maxEntries);
    }
}

std::vector<LeaderboardEntry> Leaderboard::getEntries() const {
    return entries;
}

void Leaderboard::save() const {
    std::ofstream out(filepath);
    for (const auto& [playerName, score] : getEntries()) {
        out << playerName << ";" << score << "\n";
    }
}

void Leaderboard::load() {
    std::ifstream in(filepath);
    if (!in) return;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        size_t sep = line.find(';');
        if (sep == std::string::npos) continue;

        std::string name = line.substr(0, sep);
        long long score = std::stoll(line.substr(sep + 1));

        addEntry(name, score);
    }
}