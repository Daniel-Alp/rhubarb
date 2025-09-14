#pragma once

#include "bitboard.h"
#include "move.h"
#include "board.h"
#include "attacks.h"
#include <array>

MoveList gen_pseudo_moves(const Position& pos, bool exclude_quiet);
void serialize_moves(const Position& pos, MoveList& move_list, const u64 targets, u64 attacks, i32 from_sq);
void serialize_pawn_promo(const Position& pos, MoveList& move_list, u64 to_sqs, const i32 dir);
void serialize_pawn_non_promo(const Position& pos, MoveList& move_list, u64 to_sqs, const i32 dir, const MoveFlag flag);
void gen_pawn_moves(const Position& pos, MoveList& move_list, const u64 targets, const Color col);
void gen_king_moves(const Position& pos, MoveList& move_list, const u64 targets, u64 king);
void gen_knight_moves(const Position& pos, MoveList& move_list, const u64 targets, u64 knights);
void gen_bishop_moves(const Position& pos, MoveList& move_list, const u64 targets, u64 bishops);
void gen_rook_moves(const Position& pos, MoveList& move_list, const u64 targets, u64 bishops);
void gen_queen_moves(const Position& pos, MoveList& move_list, const u64 targets, u64 bishops);
void gen_castling_moves(const Position& pos, MoveList& move_list);