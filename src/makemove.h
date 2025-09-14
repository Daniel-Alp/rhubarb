#pragma once

#include "common.h"
#include "bitboard.h"
#include "evaluation.h"
#include "zobrist.h"

constexpr std::array<i32, 64> castling = {
	13, 15, 15, 15, 12, 15, 15, 14,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,	
	7,  15, 15, 15,  3, 15, 15, 11
};

inline void clear_pce(Position &pos, i32 sq) {
	const Piece pce = pos.pces[sq];
	const Color col = get_col(pce);
	const u64 sq_bb = get_sq_bitboard(sq);

	pos.zobrist_key = hash_pce(pos.zobrist_key, pce, sq);

	pos.phase_val -= phase_vals[pce];
	pos.material_midgame_val -= material_midgame_vals[pce];
	pos.material_endgame_val -= material_endgame_vals[pce];
	pos.psqt_midgame_val -= pce_psqts_midgame[pce][sq];
	pos.psqt_endgame_val -= pce_psqts_endgame[pce][sq];

	pos.pces[sq] = Piece::NONE;
	pos.pce_bitboards[pce] = clear_sq(pos.pce_bitboards[pce], sq_bb);
	pos.col_bitboards[col] = clear_sq(pos.col_bitboards[col], sq_bb);
	pos.all_bitboard = clear_sq(pos.all_bitboard, sq_bb);
}

inline void add_pce(Position &pos, Piece pce, i32 sq) {
	pos.pces[sq] = pce;
	const Color col = get_col(pce);
	const u64 sq_bb = get_sq_bitboard(sq);

	pos.zobrist_key = hash_pce(pos.zobrist_key, pce, sq);

	pos.phase_val += phase_vals[pce];
	pos.material_midgame_val += material_midgame_vals[pce];
	pos.material_endgame_val += material_endgame_vals[pce];
	pos.psqt_midgame_val += pce_psqts_midgame[pce][sq];
	pos.psqt_endgame_val += pce_psqts_endgame[pce][sq]; 

	pos.pces[sq] = pce;
	pos.pce_bitboards[pce] = set_sq(pos.pce_bitboards[pce], sq_bb);
	pos.col_bitboards[col] = set_sq(pos.col_bitboards[col], sq_bb);
	pos.all_bitboard = set_sq(pos.all_bitboard, sq_bb);
}

inline void move_pce(Position &pos, i32 from_sq, i32 to_sq) {
	const Piece pce = pos.pces[from_sq];
	const Color col = get_col(pce);
	const u64 from_sq_bb = get_sq_bitboard(from_sq);
	const u64 to_sq_bb = get_sq_bitboard(to_sq);

	pos.zobrist_key = hash_pce(pos.zobrist_key, pce, from_sq);
	pos.zobrist_key = hash_pce(pos.zobrist_key, pce, to_sq);

	pos.psqt_midgame_val -= pce_psqts_midgame[pce][from_sq];
	pos.psqt_endgame_val -= pce_psqts_endgame[pce][from_sq];
	pos.psqt_midgame_val += pce_psqts_midgame[pce][to_sq];
	pos.psqt_endgame_val += pce_psqts_endgame[pce][to_sq];

	pos.pces[from_sq] = Piece::NONE;
	pos.pces[to_sq] = pce;
	pos.pce_bitboards[pce] = move_sq(pos.pce_bitboards[pce], from_sq_bb, to_sq_bb);
	pos.col_bitboards[col] = move_sq(pos.col_bitboards[col], from_sq_bb, to_sq_bb);
	pos.all_bitboard = move_sq(pos.all_bitboard, from_sq_bb, to_sq_bb);
}

inline void undo_null_move(Position &pos) {
	pos.history_ply--;
	pos.ply--;
	pos.side_to_move = flip_col(pos.side_to_move);

	pos.en_passant_sq	= pos.undo_stack[pos.ply].en_passant_sq;
	pos.castling_rights = pos.undo_stack[pos.ply].castling_rights;
	pos.fifty_move_rule = pos.undo_stack[pos.ply].fifty_move_rule;
	pos.zobrist_key		= pos.undo_stack[pos.ply].zobrist_key;
}

inline void make_null_move(Position &pos) {
	pos.undo_stack[pos.ply].en_passant_sq	= pos.en_passant_sq;
	pos.undo_stack[pos.ply].castling_rights = pos.castling_rights;
	pos.undo_stack[pos.ply].fifty_move_rule = pos.fifty_move_rule;
	pos.undo_stack[pos.ply].zobrist_key		= pos.zobrist_key;

	pos.history_stack[pos.history_ply] = pos.zobrist_key;

	if (pos.en_passant_sq != Square::NO_SQ) {
		pos.zobrist_key = hash_en_passant_sq(pos.zobrist_key, pos.en_passant_sq);
		pos.en_passant_sq = Square::NO_SQ;
	}
	pos.fifty_move_rule = 0;

	const Color move_col = pos.side_to_move;
	pos.side_to_move = flip_col(pos.side_to_move);
	pos.zobrist_key = hash_side_to_move(pos.zobrist_key);

	pos.history_ply++;
	pos.ply++;
}

bool make_move(Position &pos, const Move &move);
void undo_move(Position &pos, const Move &move);