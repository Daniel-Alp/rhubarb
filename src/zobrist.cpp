#include "board.h"
#include "types.h"
#include "zobrist.h"

u64 key_side_to_move;
std::array<std::array<u64, 64>, 15> keys_pce{};
std::array<u64, 16> keys_castling_rights{};

void precompute_zobrist() {
	for (i32 pce_type = PieceType::PAWN; pce_type <= PieceType::KING; pce_type++) {
		for (i32 col = Color::WHITE; col <= Color::BLACK; col++) {
			for (i32 sq = 0; sq < 64; sq++) {
				keys_pce[build_pce(static_cast<PieceType>(pce_type), static_cast<Color>(col))][sq] = random_u64();
			}
		}
	}
	
	for (i32 sq = 0; sq < 64; sq++) {
		keys_pce[Piece::NONE][sq] = random_u64();
	}

	for (i32 castling_rights = 0; castling_rights < 16; castling_rights++) {
		keys_castling_rights[castling_rights] = random_u64();
	}

	key_side_to_move = random_u64();
}

u64 get_zobrist_key(Position& pos) {
	u64 zobrist_key = 0;
	for (i32 sq = 0; sq < 64; sq++) {
		const Piece pce = pos.pces[sq];
		if (pce != Piece::NONE) {
			zobrist_key ^= keys_pce[static_cast<i32>(pce)][sq];
		}
	}

	if (pos.en_passant_sq != Square::NO_SQ) {
		zobrist_key ^= keys_pce[static_cast<i32>(Piece::NONE)][static_cast<i32>(pos.en_passant_sq)];
	}
		
	zobrist_key ^= keys_castling_rights[pos.castling_rights];

	if (pos.side_to_move == Color::BLACK) {
		zobrist_key ^= key_side_to_move;
	}

	return zobrist_key;
}