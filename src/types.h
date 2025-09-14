#pragma once

#include "common.h"
#include <array>

enum Color : u32 {
	WHITE,
	BLACK
};

enum PieceType : u32 {
	PAWN = 1,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING
};

enum Piece : u32{
	NONE = 0,
	WHITE_PAWN = 1,
	WHITE_KNIGHT = 2,
	WHITE_BISHOP = 3,
	WHITE_ROOK = 4,
	WHITE_QUEEN = 5,
	WHITE_KING = 6,
	BLACK_PAWN = 9,
	BLACK_KNIGHT = 10,
	BLACK_BISHOP = 11,
	BLACK_ROOK = 12,
	BLACK_QUEEN = 13,
	BLACK_KING = 14
};

enum CastlingRights : u32 {
	WHITE_SHORT = 1,
	WHITE_LONG	= 2,
	BLACK_SHORT = 4,
	BLACK_LONG	= 8
};

Piece symbol_to_pce(const char symbol);
char pce_to_symbol(const Piece piece);

inline Color get_col(const Piece piece) {
	return static_cast<Color>(piece >> 3);
}

inline PieceType get_pce_type(const Piece piece) {
	return static_cast<PieceType>(piece & 7);
}

inline Piece build_pce(const PieceType pce_type, const Color col) {
	return static_cast<Piece>(pce_type + (col << 3));
}
