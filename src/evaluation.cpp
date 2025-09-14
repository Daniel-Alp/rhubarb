#include "common.h"
#include "evaluation.h"

std::array<std::array<i32, 64>, 15> pce_psqts_midgame;
std::array<std::array<i32, 64>, 15> pce_psqts_endgame;

void precompute_pce_psqt(const Piece piece, const std::array<i32, 64> &pcetype_psqt_midgame, const std::array<i32, 64> &pcetype_psqt_endgame) {
	const Color col = get_col(piece);
	if (col == Color::WHITE) {
		for (i32 sq = 0; sq < 64; sq++) {
			pce_psqts_midgame[piece][sq] = pcetype_psqt_midgame[sq];
			pce_psqts_endgame[piece][sq] = pcetype_psqt_endgame[sq];
		}
	} else {
		for (i32 sq = 0; sq < 64; sq++) {
			pce_psqts_midgame[piece][sq] = pcetype_psqt_midgame[sq ^ 56] * -1;
			pce_psqts_endgame[piece][sq] = pcetype_psqt_endgame[sq ^ 56] * -1;
		}
	}
}

i32 evaluate(const Position &pos) {
	i32 midgame_eval = pos.material_midgame_val + pos.psqt_midgame_val;
	i32 endgame_eval = pos.material_endgame_val + pos.psqt_endgame_val;
	const i32 phase = std::min(24, pos.phase_val);

	i32 eval = (midgame_eval * phase + endgame_eval * (24 - phase)) / 24;

	if (pos.side_to_move == Color::BLACK) {
		eval *= -1;
	}

	const i32 tempo_bonus = 20;

	return eval + tempo_bonus;
}