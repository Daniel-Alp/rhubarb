#include "uci.h"
#include "zobrist.h"
#include "bench.h"

int main() {
	precompute_rays();
	precompute_non_slider_attacks();
	precompute_zobrist();

	precompute_pce_psqt(Piece::WHITE_PAWN, pawn_midgame_psqt, pawn_endgame_psqt);
	precompute_pce_psqt(Piece::BLACK_PAWN, pawn_midgame_psqt, pawn_endgame_psqt);

	precompute_pce_psqt(Piece::WHITE_KNIGHT, knight_midgame_psqt, knight_endgame_psqt);
	precompute_pce_psqt(Piece::BLACK_KNIGHT, knight_midgame_psqt, knight_endgame_psqt);

	precompute_pce_psqt(Piece::WHITE_BISHOP, bishop_midgame_psqt, bishop_endgame_psqt);
	precompute_pce_psqt(Piece::BLACK_BISHOP, bishop_midgame_psqt, bishop_endgame_psqt);

	precompute_pce_psqt(Piece::WHITE_ROOK, rook_midgame_psqt, rook_endgame_psqt);
	precompute_pce_psqt(Piece::BLACK_ROOK, rook_midgame_psqt, rook_endgame_psqt);

	precompute_pce_psqt(Piece::WHITE_QUEEN, queen_midgame_psqt, queen_endgame_psqt);
	precompute_pce_psqt(Piece::BLACK_QUEEN, queen_midgame_psqt, queen_endgame_psqt);

	precompute_pce_psqt(Piece::WHITE_KING, king_midgame_psqt, king_endgame_psqt);
	precompute_pce_psqt(Piece::BLACK_KING, king_midgame_psqt, king_endgame_psqt);

	precompute_reduction_table();

	// Position pos;
	// std::vector<std::string> cmd_sections = split_string("position startpos", ' ');
	// uci_position_command(cmd_sections, pos);
	// cmd_sections = split_string("perft 6", ' ');
	// uci_perft_command(cmd_sections, pos);
	// uci_loop();
	run_bench();
}