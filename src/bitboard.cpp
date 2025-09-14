#include "common.h"
#include "bitboard.h"
#include "board.h"
#include <array>
#include <iostream>

std::array<u64, 64> ray_nort;
std::array<u64, 64> ray_noea;
std::array<u64, 64> ray_east;
std::array<u64, 64> ray_soea;
std::array<u64, 64> ray_sout;
std::array<u64, 64> ray_sowe;
std::array<u64, 64> ray_west;
std::array<u64, 64> ray_nowe;

std::array<u64, 64> ray_nort_stop;
std::array<u64, 64> ray_noea_stop;
std::array<u64, 64> ray_east_stop;
std::array<u64, 64> ray_soea_stop;
std::array<u64, 64> ray_sout_stop;
std::array<u64, 64> ray_sowe_stop;
std::array<u64, 64> ray_west_stop;
std::array<u64, 64> ray_nowe_stop;

std::array<u64, 64> ray_bishop;
std::array<u64, 64> ray_rook;
std::array<u64, 64> ray_queen;

std::array<u64, 64> white_pawn_attacks;
std::array<u64, 64> black_pawn_attacks;
std::array<u64, 64> king_attacks;
std::array<u64, 64> knight_attacks;

void precompute_rays() {
	u64 nort = 0x0101010101010100;
	for (i32 sq = 0; sq < 64; sq++) {
		ray_nort[sq] = nort;
		ray_nort_stop[sq] = set_sq(nort, get_sq_bitboard(63));
		nort <<= 1;
	}

	u64 noea = 0x8040201008040200;
	for (i32 file = 0; file < 8; file++) {
		u64 noea_2 = noea;
		for (i32 rank = 0; rank < 8; rank++) {
			const i32 sq = get_sq(rank, file);
			ray_noea[sq] = noea_2;
			ray_noea_stop[sq] = set_sq(noea_2, get_sq_bitboard(63));
			noea_2 = shift_nort(noea_2);
		}
		noea = shift_east(noea);
	}

	u64 east = 0xFE;
	for (i32 file = 0; file < 8; file++) {
		u64 east_2 = east;
		for (i32 rank = 0; rank < 8; rank++) {
			const i32 sq = get_sq(rank, file);
			ray_east[sq] = east_2;
			ray_east_stop[sq] = set_sq(east_2, get_sq_bitboard(63));
			east_2 = shift_nort(east_2);
		}
		east = shift_east(east);
	}

	u64 nowe = 0x102040810204000;
	for (i32 file = 7; file >= 0; file--) {
		u64 nowe_2 = nowe;
		for (i32 rank = 0; rank < 8; rank++) {
			const i32 sq = get_sq(rank, file);
			ray_nowe[sq] = nowe_2;
			ray_nowe_stop[sq] = set_sq(nowe_2, get_sq_bitboard(63));
			nowe_2 = shift_nort(nowe_2);
		}
		nowe = shift_west(nowe);
	}

	u64 sout = 0x80808080808080;
	for (i32 sq = 63; sq >= 0; sq--) {
		ray_sout[sq] = sout;
		ray_sout_stop[sq] = set_sq(sout, get_sq_bitboard(0));
		sout >>= 1;
	}

	u64 sowe = 0x40201008040201;
	for (i32 file = 7; file >= 0; file--) {
		u64 sowe_2 = sowe;
		for (i32 rank = 7; rank >= 0; rank--) {
			const i32 sq = get_sq(rank, file);
			ray_sowe[sq] = sowe_2;
			ray_sowe_stop[sq] = set_sq(sowe_2, get_sq_bitboard(0));
			sowe_2 = shift_sout(sowe_2);
		}
		sowe = shift_west(sowe);
	}

	u64 west = 0x7F;
	for (i32 file = 7; file >= 0; file--) {
		u64 west_2 = west;
		for (i32 rank = 0; rank < 8; rank++) {
			const i32 sq = get_sq(rank, file);
			ray_west[sq] = west_2;
			ray_west_stop[sq] = set_sq(west_2, get_sq_bitboard(0));
			west_2 = shift_nort(west_2);
		}
		west = shift_west(west);
	}

	u64 soea = 0x2040810204080;
	for (i32 file = 0; file < 8; file++) {
		u64 soea_2 = soea;
		for (i32 rank = 7; rank >= 0; rank--) {
			const i32 sq = get_sq(rank, file);
			ray_soea[sq] = soea_2;
			ray_soea_stop[sq] = set_sq(soea_2, get_sq_bitboard(0));
			soea_2 = shift_sout(soea_2);
		}
		soea = shift_east(soea);
	}

	for (i32 sq = 0; sq < 64; sq++) {
		ray_bishop[sq] = ray_noea[sq] ^ ray_soea[sq] ^ ray_sowe[sq] ^ ray_nowe[sq];
		ray_rook[sq] = ray_nort[sq] ^ ray_east[sq] ^ ray_sout[sq] ^ ray_west[sq];
		ray_queen[sq] = ray_bishop[sq] ^ ray_rook[sq];
	}
}

void precompute_non_slider_attacks() {
	u64 king = 1;
	for (i32 sq = 0; sq < 64; sq++) {
		u64 attacks = shift_east(king) | shift_west(king) | king;
		attacks = (attacks | shift_nort(attacks) | shift_sout(attacks)) ^ king;
		king_attacks[sq] = attacks;
		king <<= 1;
	}

	u64 knight = 1;
	for (i32 sq = 0; sq < 64; sq++) {
		u64 east = shift_east(knight);
		u64 west = shift_west(knight);
		u64 attacks = shift_nort(shift_nort(east | west));
		attacks |= shift_sout(shift_sout(east | west));
		east = shift_east(east);
		west = shift_west(west);
		attacks |= shift_nort(east | west);
		attacks |= shift_sout(east | west);
		knight_attacks[sq] = attacks;
		knight <<= 1;
	}

	u64 pawn = 1;
	for (i32 sq = 0; sq < 64; sq++) {
		white_pawn_attacks[sq] = shift_noea(pawn) | shift_nowe(pawn);
		black_pawn_attacks[sq] = shift_soea(pawn) | shift_sowe(pawn);
		pawn <<= 1;
	}
}