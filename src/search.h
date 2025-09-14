#pragma once

#include "board.h"
#include "constants.h"
#include "evaluation.h"
#include "movegen.h"
#include "move.h"
#include "transposition.h"
#include "timemanagement.h"
#include <algorithm>
#include <cmath>

extern std::array<std::array<i64, 64>, 15> history_table;
extern std::array<std::array<Move, 2>, 257> killer_table;
extern std::array<std::array<i32, 218>, 256> reduction_table;

struct SearchData {
	bool searching;
	i32 max_depth;
	u64 start_time;
	u64 time_allotted;
	u64 nodes;
	Move best_move_root;
};

void best_move(Position &pos, SearchData &search_data);

i32 negamax(Position &pos, SearchData &search_data, i32 alpha, i32 beta, i32 depth, i32 ply, bool allow_null);

i32 quiescence(Position &pos, SearchData &search_data, i32 alpha, i32 beta);

inline void precompute_reduction_table() {
	for (i32 depth = 1; depth < 256; depth++) {
		for (i32 num_moves = 1; num_moves < 218; num_moves++) {
			reduction_table[depth][num_moves] = std::max((std::log(depth) * std::log(num_moves)) / 2 + 1, static_cast<double>(2));
		}
	}
}

inline void clear_history_table() {
	for (i32 move_pce = Piece::NONE; move_pce <= Piece::BLACK_KING; move_pce++) {
		for (i32 to_sq = 0; to_sq < 64; to_sq++) {
			history_table[move_pce][to_sq] = 0;
		}
	}
}

inline void div_two_history_table() {
	for (i32 move_pce = 0; move_pce <= Piece::BLACK_KING; move_pce++) {
		for (i32 to_sq = 0; to_sq < 64; to_sq++) {
			history_table[move_pce][to_sq] /= 2;
		}
	}
}

inline void clear_killer_table() {
	for (i32 ply = 0; ply < 256; ply++) {
		killer_table[ply][0] = Move();
		killer_table[ply][1] = Move();
	}
}

inline i64 mvv_lva(PieceType cap_pce_type, PieceType move_pce_type) {
	return (static_cast<i64>(cap_pce_type) << 50) - static_cast<i64>(move_pce_type);
}

inline i64 score_move(const Move &move, const Move &hash_entry_best_move, std::array<Piece, 64> &pces, i32 ply) {
	if (move == hash_entry_best_move) {
		return ((i64)1) << 60;
	}
	if (move == killer_table[ply][1]) {
		return ((i64)1) << 41;
	}
	if (move == killer_table[ply][0]) {
		return ((i64)1) << 40;
	}

	const Piece move_pce = pces[move.get_from_sq()];
	const Piece cap_pce = move.get_cap_pce();
	const PieceType move_pce_type = get_pce_type(pces[move_pce]);
	
	if (cap_pce != Piece::NONE) {
		return mvv_lva(get_pce_type(cap_pce), move_pce_type);
	}

	const i32 from_sq = move.get_from_sq();
	const i32 to_sq = move.get_to_sq();

	return history_table[move_pce][to_sq] + pce_psqts_midgame[move_pce_type][to_sq] - pce_psqts_midgame[move_pce_type][from_sq];
}

inline Move get_next_move(MoveList &move_list, std::array<i64, MoveList::max_moves> &scores, i32 cur_move_index) {
	i32 best_move_index = cur_move_index;
	for (i32 i = cur_move_index + 1; i < move_list.size(); i++) {
		if (scores[i] > scores[best_move_index]) {
			best_move_index = i;
		}
	}
	move_list.swap(cur_move_index, best_move_index);
	std::swap(scores[cur_move_index], scores[best_move_index]);
	return move_list.get(cur_move_index);
}

inline bool time_up(SearchData &search_data) {
	search_data.nodes++;
	return (search_data.nodes & 2047) == 0 && get_current_time() - search_data.start_time > search_data.time_allotted;
}

inline bool repeated_pos(const Position &pos) {
	const i32 min_ply = std::max(pos.history_ply - pos.fifty_move_rule, ((i32)0));
	for (i32 ply = pos.history_ply - 2; ply >= min_ply; ply -= 2) {
		if (pos.zobrist_key == pos.history_stack[ply]) {
			return true;
		}
	}
	return false;
}