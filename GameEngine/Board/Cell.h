#pragma once
#include <unordered_map>

enum class Cell {
    Empty, I, O, T, L, J, S, Z, GhostI, GhostO, GhostT, GhostL, GhostJ, GhostS, GhostZ
};

static const std::unordered_map<Cell, Cell> ghostMap = {
    {Cell::I, Cell::GhostI},
    {Cell::O, Cell::GhostO},
    {Cell::T, Cell::GhostT},
    {Cell::L, Cell::GhostL},
    {Cell::J, Cell::GhostJ},
    {Cell::S, Cell::GhostS},
    {Cell::Z, Cell::GhostZ}
};