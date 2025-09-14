#pragma once

#include "common.h"
#include <array>
#include <x86intrin.h>

extern std::array<u64, 64> ray_nort;
extern std::array<u64, 64> ray_noea;
extern std::array<u64, 64> ray_east;
extern std::array<u64, 64> ray_soea;
extern std::array<u64, 64> ray_sout;
extern std::array<u64, 64> ray_sowe;
extern std::array<u64, 64> ray_west;
extern std::array<u64, 64> ray_nowe;

extern std::array<u64, 64> ray_nort_stop;
extern std::array<u64, 64> ray_noea_stop;
extern std::array<u64, 64> ray_east_stop;
extern std::array<u64, 64> ray_soea_stop;
extern std::array<u64, 64> ray_sout_stop;
extern std::array<u64, 64> ray_sowe_stop;
extern std::array<u64, 64> ray_west_stop;
extern std::array<u64, 64> ray_nowe_stop;

extern std::array<u64, 64> ray_bishop;
extern std::array<u64, 64> ray_rook;
extern std::array<u64, 64> ray_queen;

extern std::array<u64, 64> white_pawn_attacks;
extern std::array<u64, 64> black_pawn_attacks;
extern std::array<u64, 64> knight_attacks;
extern std::array<u64, 64> king_attacks;

inline u64 rank_1 = 0x00000000000000FF;
inline u64 rank_4 = 0x00000000FF000000;
inline u64 rank_5 = 0x000000FF00000000;
inline u64 rank_8 = 0xFF00000000000000;

inline u64 sq_between_e1_h1 = 0x0000000000000060;
inline u64 sq_between_e1_a1 = 0x000000000000000E;
inline u64 sq_between_e8_h8 = 0x6000000000000000;
inline u64 sq_between_e8_a8 = 0x0E00000000000000;

void precompute_rays();
void precompute_non_slider_attacks();

inline u64 shift_nort(const u64 bitboard) {
	return bitboard << 8;
}

inline u64 shift_noea(const u64 bitboard) {
	return (bitboard << 9) & 0xfefefefefefefefe;
}

inline u64 shift_east(const u64 bitboard) {
	return (bitboard << 1) & 0xfefefefefefefefe;
}

inline u64 shift_soea(const u64 bitboard) {
	return (bitboard >> 7) & 0xfefefefefefefefe;
}

inline u64 shift_sout(const u64 bitboard) {
	return bitboard >> 8;
}

inline u64 shift_sowe(const u64 bitboard) {
	return (bitboard >> 9) & 0x7f7f7f7f7f7f7f7f;
}

inline u64 shift_west(const u64 bitboard) {
	return (bitboard >> 1) & 0x7f7f7f7f7f7f7f7f;
}

inline u64 shift_nowe(const u64 bitboard) {
	return (bitboard << 7) & 0x7f7f7f7f7f7f7f7f;
}

inline u64 get_sq_bitboard(const u32 sq) {
	return ((u64)1) << sq;
}

inline bool has_sq(const u64 bitboard, const u64 sq) {
	return bitboard & sq;
}

inline u64 set_sq(const u64 bitboard, const u64 sq) {
	return bitboard | sq;
}

inline u64 clear_sq(const u64 bitboard, const u64 sq) {
	return bitboard ^ sq;
}

inline u64 move_sq(const u64 bitboard, const u64 from_sq, const u64 to_sq) {
	return bitboard ^ from_sq ^ to_sq;
}

inline i32 get_lsb(const u64 bitboard) {
	return __builtin_ctzl(bitboard);
}

inline i32 get_msb(const u64 bitboard) {
	return __builtin_clzl(bitboard) ^ 63;
}

inline u64 clear_lsb(const u64 bitboard) {
	return bitboard & (bitboard - 1);
}