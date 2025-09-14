#include "common.h"
#include "attacks.h"
#include "board.h"
#include "constants.h"
#include "evaluation.h"
#include "makemove.h"
#include "move.h"
#include "movegen.h"
#include "search.h"
#include <algorithm>
#include <iostream>

std::array<std::array<i64, 64>, 15> history_table;
std::array<std::array<Move, 2>, 257> killer_table;
std::array<std::array<i32, 218>, 256> reduction_table;

void best_move(Position& pos, SearchData& search_data) {
	div_two_history_table();
	clear_killer_table();
	search_data.nodes = 0;
	search_data.best_move_root = Move();
	search_data.searching = true;

	const i32 min_depth_aspiration = 6;

	Move best_move_root_prev = Move();
	i32 score_prev;

	for (i32 depth = 1; depth < search_data.max_depth; depth++) {
		pos.ply = 0;
		i32 score;

		if (depth <= min_depth_aspiration) {
			score = negamax(pos, search_data, -mate_score, mate_score, depth, 0, false);
			score_prev = score;
			best_move_root_prev = search_data.best_move_root;

			if (!search_data.searching) {
				break;
			}
		}
		else {
			i32 delta = 30;
			i32 alpha = std::max(score_prev - delta, -mate_score);
			i32 beta = std::min(score_prev + delta, mate_score);

			while (true) {
				score = negamax(pos, search_data, alpha, beta, depth, 0, false);

				if (!search_data.searching) {
					break;
				}

				if (score <= alpha) {
					alpha = std::max(alpha - delta, -mate_score);
					if (alpha < -800) {
						alpha = -mate_score;
					}
				}
				else if (score >= beta) {
					beta = std::min(beta + delta, mate_score);
					if (beta > 800) {
						beta = mate_score;
					}
				}
				else {
					break;
				}

				delta *= 2;
			}
			score_prev = score;
			best_move_root_prev = search_data.best_move_root;

			if (!search_data.searching) {
				break;
			}
		}
		std::cout << "info score " << score << std::endl; 
	}
	search_data.searching = false;
	std::cout << "bestmove " << best_move_root_prev.to_str() << std::endl;
}


i32 negamax(Position& pos, SearchData& search_data, i32 alpha, i32 beta, i32 depth, i32 ply, bool allow_null) {
	if (time_up(search_data)) {
		search_data.searching = false;
		return 0;
	}

	if (pos.ply >= search_data.max_depth) {
		return evaluate(pos);
	}

	const bool root_node = (ply == 0);
	const bool pv_node = (alpha != beta - 1);

	if (!root_node && (pos.fifty_move_rule >= 100 || repeated_pos(pos))) {
		return 0;
	}

	const HashEntry hash_entry = hash_table.get(pos.zobrist_key);
	const bool matching_key = (hash_entry.zobrist_key == pos.zobrist_key);

	if (!pv_node && matching_key && hash_entry.depth >= depth) {
		i32 retrieved_score = hash_table.hash_table_to_score(hash_entry.score, ply);

		if (hash_entry.hash_flag == HashFlag::EXACT
			|| (hash_entry.hash_flag == HashFlag::BETA && (retrieved_score >= beta))
			|| (hash_entry.hash_flag == HashFlag::ALPHA && (retrieved_score <= alpha))) {
			return retrieved_score;
		}
	}

	const i32 king_sq = get_lsb(pos.pce_bitboards[build_pce(PieceType::KING, pos.side_to_move)]);
	const bool in_check = sq_attacked(pos, king_sq, flip_col(pos.side_to_move));

	if (depth <= 0 && !in_check) {
		return quiescence(pos, search_data, alpha, beta);
	}

	if (!pv_node && !in_check) {
		const i32 static_eval = evaluate(pos);
		if (static_eval - depth * 100 >= beta && depth < 9) {
			return static_eval;
		}

		if (allow_null && depth >= 3 && pos.phase_val > 0 && static_eval >= beta) {
			make_null_move(pos);
			const i32 reduction = 2 + depth / 3;
			const i32 score = -negamax(pos, search_data, -beta, -beta + 1, depth - 1 - reduction, ply + 1, false);
			undo_null_move(pos);
			if (score >= beta) {
				return score;
			}
		}
	}
	
	MoveList move_list = gen_pseudo_moves(pos, false);
	i32 num_legal_moves = 0;

	Move hash_entry_best_move = Move();
	if (matching_key) {
		hash_entry_best_move = hash_entry.best_move;
	}
	std::array<i64, MoveList::max_moves> scores{};
	for (i32 i = 0; i < move_list.size(); i++) {
		scores[i] = score_move(move_list.get(i), hash_entry_best_move, pos.pces, ply);
	}

	i32 best_score = -mate_score;
	Move best_move = Move();

	const i32 orig_alpha = alpha;
	i32 score;

	killer_table[ply + 1][0] = Move();
	killer_table[ply + 1][1] = Move();

	for (i32 i = 0; i < move_list.size(); i++) {
		const Move move = get_next_move(move_list, scores, i);
		const Piece move_pce = pos.pces[move.get_from_sq()];
			
		if (!make_move(pos, move)) {
			continue;
		}

		num_legal_moves++;

		if (num_legal_moves > 1) {
			i32 reduction = reduction_table[depth][num_legal_moves];
			reduction -= history_table[move_pce][move.get_to_sq()] / 16384;
			if (in_check) {
				reduction--;
			}

			if (num_legal_moves >= 3 + 2 * pv_node
				&& depth >= 3
				&& move.is_quiet()
				&& reduction > 0) {

				if (depth - 1 - reduction <= 0) {
					reduction = depth - 2;
				}

				score = -negamax(pos, search_data, -alpha - 1, -alpha, depth - 1 - reduction, ply + 1, true);
			}
			else {
				score = alpha + 1;
			}

			if (score > alpha) {
				score = -negamax(pos, search_data, -alpha - 1, -alpha, depth - 1, ply + 1, true);
				if (score > alpha && score < beta) {
					score = -negamax(pos, search_data, -beta, -alpha, depth - 1, ply + 1, true);
				}
			}
		}
		else {
			score = -negamax(pos, search_data, -beta, -alpha, depth - 1, ply + 1, true);
		}

		undo_move(pos, move);

		if (!search_data.searching) {
			return 0;
		}

		if (score > best_score) {
			best_score = score;
			best_move = move;

			if (score > alpha) {
				if (root_node) {
					search_data.best_move_root = move;
				}

				alpha = score;
				if (score >= beta) {
					if (!move.is_quiet()) {
						break;
					}
					
					history_table[move_pce][move.get_to_sq()] += depth * depth;
					for (i32 j = 0; j < i; j++) {
						const Move penalized_move = move_list.get(j);
						if (!penalized_move.is_quiet()) {
							continue;
						}
						const Piece penalized_move_pce = pos.pces[penalized_move.get_from_sq()];
						history_table[penalized_move_pce][penalized_move.get_to_sq()] -= depth * depth;
					}

					if (killer_table[ply][1] != move) {
						killer_table[ply][0] = killer_table[ply][1];
						killer_table[ply][1] = move;
					}

					break;
				}
			}
		}
	}

	if (num_legal_moves == 0) {
		if (in_check) {
			return -mate_score + ply;
		}
		else {
			return 0;
		}
	}

	i32 recorded_score = hash_table.score_to_hash_table(best_score, ply);

	HashFlag hash_flag;
	if (best_score >= beta) {
		hash_flag = HashFlag::BETA;
	}
	else if (best_score > orig_alpha) {
		hash_flag = HashFlag::EXACT;
	}
	else {
		hash_flag = HashFlag::ALPHA;
	}
	hash_table.record(pos.zobrist_key, depth, recorded_score, hash_flag, best_move);

	return best_score;
}

i32 quiescence(Position& pos, SearchData& search_data, i32 alpha, i32 beta) {
	if (time_up(search_data)) {
		search_data.searching = false;
		return 0;
	}

	i32 best_score = evaluate(pos);

	if (best_score > alpha) {
		alpha = best_score;
		if (best_score >= beta) {
			return best_score;
		}
	}

	MoveList move_list = gen_pseudo_moves(pos, true);
	std::array<i64, MoveList::max_moves> scores{};
	for (i32 i = 0; i < move_list.size(); i++) {
		scores[i] = score_move(move_list.get(i), Move(), pos.pces, 255);
	}

	for (i32 i = 0; i < move_list.size(); i++) {
		const Move move = get_next_move(move_list, scores, i);

		if (!make_move(pos, move)) {
			continue;
		}

		i32 score = -quiescence(pos, search_data, -beta, -alpha);
		undo_move(pos, move);
		if (!search_data.searching) {
			return 0;
		}

		if (score > best_score) {
			best_score = score;

			if (score > alpha) {
				alpha = score;

				if (score >= beta) {
					break;
				}
			}
		}
	}

	return best_score;
}