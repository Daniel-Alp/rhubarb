#include "common.h"
#include "move.h"
#include "movegen.h"
#include "makemove.h"
#include "parser.h"
#include "perft.h"
#include "transposition.h"
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

u64 perft(Position &pos, i32 depth, i32 ply) {
	if (depth == 0) {
		return 1;
	}

	u64 nodes = 0;
	MoveList move_list = gen_pseudo_moves(pos, false);

	for (i32 i = 0; i < move_list.size(); i++) {
		Move move = move_list.get(i);
		if (make_move(pos, move)) {
			nodes += perft(pos, depth - 1, ply + 1);
			undo_move(pos, move);
		}
	}

	return nodes;
}