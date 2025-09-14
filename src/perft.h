#pragma once

#include "board.h"
#include <cstdint>

uint64_t perft(Position& pos, int depth, int ply);