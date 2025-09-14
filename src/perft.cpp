#include "move.h"
#include "movegen.h"
#include "makemove.h"
#include "parser.h"
#include "perft.h"
#include "transposition.h"
#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

uint64_t perft(Position& pos, int depth, int ply) {
	if (depth == 0) {
		return 1;
	}

	uint64_t nodes = 0;
	MoveList move_list = gen_pseudo_moves(pos, false);

	for (int i = 0; i < move_list.size(); i++) {
		Move move = move_list.get(i);
		if (make_move(pos, move)) {
			nodes += perft(pos, depth - 1, ply + 1);
			undo_move(pos, move);
		}
	}

	return nodes;
}