#pragma once

#include "common.h"
#include "board.h"
#include <array>

inline u64 seed = 1070372;

extern u64 key_side_to_move;
extern std::array<std::array<u64, 64>, 15> keys_pce;
extern std::array<u64, 16> keys_castling_rights;

void precompute_zobrist();
u64 get_zobrist_key(Position &pos);

//From StockFish
inline u64 random_u64() {
	seed ^= seed >> 12;
	seed ^= seed << 25;
	seed ^= seed >> 27;
	seed *= (u64)2685821657736338717;
	return seed;
}
inline u64 hash_pce(u64 zobrist_key, Piece pce, const i32 sq) {
	return zobrist_key ^ keys_pce[pce][sq];
}
inline u64 hash_castling_rights(u64 zobrist_key, const i32 castling_rights) {
	return zobrist_key ^ keys_castling_rights[castling_rights];
}
inline u64 hash_side_to_move(u64 zobrist_key) {
	return zobrist_key ^ key_side_to_move;
}
inline u64 hash_en_passant_sq(u64 zobrist_key, Square sq) {
	return zobrist_key ^ keys_pce[Piece::NONE][sq];
}