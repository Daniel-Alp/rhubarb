#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "move.h"
#include "makemove.h"
#include "zobrist.h"
#include <array>

#include <iostream>

bool make_move(Position &pos, const Move &move) {
	pos.undo_stack[pos.ply].en_passant_sq	= pos.en_passant_sq;
	pos.undo_stack[pos.ply].castling_rights = pos.castling_rights;
	pos.undo_stack[pos.ply].fifty_move_rule = pos.fifty_move_rule;
	pos.undo_stack[pos.ply].zobrist_key		= pos.zobrist_key;

	pos.history_stack[pos.history_ply] = pos.zobrist_key;

	const u32 from_sq = move.get_from_sq();
	const u32 to_sq = move.get_to_sq();
	
	if (pos.en_passant_sq != Square::NO_SQ) {
		pos.zobrist_key = hash_en_passant_sq(pos.zobrist_key, pos.en_passant_sq);
		pos.en_passant_sq = Square::NO_SQ;
	}
 	pos.zobrist_key = hash_castling_rights(pos.zobrist_key, pos.castling_rights);
	pos.castling_rights &= castling[from_sq];
	pos.castling_rights &= castling[to_sq];
	pos.zobrist_key = hash_castling_rights(pos.zobrist_key, pos.castling_rights);

	pos.fifty_move_rule++;

	if (move.is_castle()) {
		move_pce(pos, from_sq, to_sq);
		switch (to_sq) {
		case Square::G1:
			move_pce(pos, Square::H1, Square::F1);
			break;
		case Square::C1:
			move_pce(pos, Square::A1, Square::D1);
			break;
		case Square::G8:
			move_pce(pos, Square::H8, Square::F8);
			break;
		case Square::C8:
			move_pce(pos, Square::A8, Square::D8);
			break;
		}
	} else {
		const Piece cap_pce = move.get_cap_pce();
		if (cap_pce != Piece::NONE) {
			pos.fifty_move_rule = 0;
			if (move.is_en_passant()) {
				clear_pce(pos, to_sq ^ 8);
			} else {
				clear_pce(pos, to_sq);
			}
		}
		move_pce(pos, from_sq, to_sq);

		if (get_pce_type(pos.pces[to_sq]) == PieceType::PAWN) {
			pos.fifty_move_rule = 0;
			const Piece promo_pce = move.get_promo_pce();
			if (promo_pce != Piece::NONE) {
				clear_pce(pos, to_sq);
				add_pce(pos, promo_pce, to_sq);
			} else if (move.is_pawn_start()) {
				pos.en_passant_sq = static_cast<Square>(to_sq ^ 8);
				pos.zobrist_key = hash_en_passant_sq(pos.zobrist_key, pos.en_passant_sq);
			}
		}
	}

	const Color move_col = pos.side_to_move;
	pos.side_to_move = flip_col(pos.side_to_move);
	pos.zobrist_key = hash_side_to_move(pos.zobrist_key);

	pos.history_ply++;
	pos.ply++;

	if (sq_attacked(pos, get_lsb(pos.pce_bitboards[build_pce(PieceType::KING, move_col)]), pos.side_to_move)) {		
		undo_move(pos, move);
		return false;
	}

	return true;
}

void undo_move(Position &pos, const Move &move) {
	pos.history_ply--;
	pos.ply--;
	pos.side_to_move = flip_col(pos.side_to_move);

	const u32 from_sq = move.get_from_sq();
	const u32 to_sq = move.get_to_sq();

	if (move.is_castle()) {
		move_pce(pos, to_sq, from_sq);
		switch (to_sq) {
		case Square::G1:
			move_pce(pos, Square::F1, Square::H1);
			break;
		case Square::C1:
			move_pce(pos, Square::D1, Square::A1);
			break;
		case Square::G8:
			move_pce(pos, Square::F8, Square::H8);
			break;
		case Square::C8:
			move_pce(pos, Square::D8, Square::A8);
			break;
		}
	} else {
		const Piece promo_pce = move.get_promo_pce();
		if (promo_pce != Piece::NONE) {
			clear_pce(pos, to_sq);
			const Piece undo_promo_pce = build_pce(PieceType::PAWN, pos.side_to_move);
			add_pce(pos, undo_promo_pce, to_sq);
		}

		move_pce(pos, to_sq, from_sq);

		const Piece cap_pce = move.get_cap_pce();
		if (cap_pce != Piece::NONE) {
			if (move.is_en_passant()) {
				add_pce(pos, cap_pce, to_sq ^ 8);
			} else {
				add_pce(pos, cap_pce, to_sq);
			}
		}
	}

	pos.en_passant_sq	= pos.undo_stack[pos.ply].en_passant_sq;
	pos.castling_rights = pos.undo_stack[pos.ply].castling_rights;
	pos.fifty_move_rule = pos.undo_stack[pos.ply].fifty_move_rule;
	pos.zobrist_key		= pos.undo_stack[pos.ply].zobrist_key;
}