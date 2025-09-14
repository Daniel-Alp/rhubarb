#include "common.h"
#include "bitboard.h"
#include "board.h"
#include "evaluation.h"
#include "parser.h"
#include "types.h"
#include "zobrist.h"
#include <array>
#include <iostream>
#include <string>
#include <vector>

std::string start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

Position load_from_fen(const std::string &fen_string) {
	Position pos{};

	const std::vector<std::string> fen_sections = split_string(fen_string, ' ');
	const std::string piece_placement_section = fen_sections[0];
	const std::string side_to_move_section	  = fen_sections[1];
	const std::string castling_rights_section = fen_sections[2];
	const std::string en_passant_sq_section	  = fen_sections[3];
	const std::string fifty_move_rule_section = fen_sections[4];

	for (i32 sq = 0; sq < 64; sq++) {
		pos.pces[sq] = Piece::NONE;
	}
	i32 rank = 7;
	i32 file = 0;
	for (char symbol : piece_placement_section) {
		if (symbol == '/') {
			rank--;
			file = 0;
		}
		else if (isdigit(symbol)) {
			file += symbol - '0';
		}
		else {
			const i32 sq = get_sq(rank, file);
			const u64 sq_bb = get_sq_bitboard(sq);
			const Piece pce = symbol_to_pce(symbol);
			const Color col = get_col(pce);

			pos.all_bitboard = set_sq(pos.all_bitboard, sq_bb);
			pos.col_bitboards[col] = set_sq(pos.col_bitboards[col], sq_bb);
			pos.pce_bitboards[pce] = set_sq(pos.pce_bitboards[pce], sq_bb);
			pos.pces[sq] = pce;

			pos.material_midgame_val += material_midgame_vals[pce];
			pos.material_endgame_val += material_endgame_vals[pce];
			pos.phase_val += phase_vals[pce];
			pos.phase_val = std::min(pos.phase_val, 24);
			pos.psqt_midgame_val += pce_psqts_midgame[pce][sq];
			pos.psqt_endgame_val += pce_psqts_endgame[pce][sq];

			file++;
		}
	}

	if (side_to_move_section == "w") {
		pos.side_to_move = Color::WHITE;
	}
	else {
		pos.side_to_move = Color::BLACK;
	}

	if (castling_rights_section.find('K') != std::string::npos) {
		pos.castling_rights |= CastlingRights::WHITE_SHORT;
	}
	if (castling_rights_section.find('Q') != std::string::npos) {
		pos.castling_rights |= CastlingRights::WHITE_LONG;
	}
	if (castling_rights_section.find('k') != std::string::npos) {
		pos.castling_rights |= CastlingRights::BLACK_SHORT;
	}
	if (castling_rights_section.find('q') != std::string::npos) {
		pos.castling_rights |= CastlingRights::BLACK_LONG;
	}

	if (en_passant_sq_section == "-") {
		pos.en_passant_sq = Square::NO_SQ;
	}
	else {
		const u32 rank = en_passant_sq_section[1] - '1';
		const u32 file = en_passant_sq_section[0] - 'a';
		pos.en_passant_sq = static_cast<Square>(get_sq(rank, file));
	}

	pos.fifty_move_rule = std::stoi(fifty_move_rule_section);

	pos.zobrist_key = get_zobrist_key(pos);

	return pos;
}