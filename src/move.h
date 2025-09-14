#pragma once

#include "common.h"
#include "types.h"
#include "board.h"
#include <array>
#include <string>

constexpr u32 null_move_val = 0;

enum MoveFlag : u32 {
	NO_FLAG	   = 0b00000000000000000000000,
	PAWN_START = 0b00100000000000000000000,
	CASTLE	   = 0b01000000000000000000000,
	EN_PASSANT = 0b10000000000000000000000
};

class Move {
private:
	u32 val;
public:
	inline Move() : val(null_move_val) {}

	inline Move(u32 from_sq, u32 to_sq, Piece capture_pce, Piece promo_pce, MoveFlag flag) {
		val = (from_sq | (to_sq << 6) | (capture_pce << 12) | (promo_pce << 16) | flag);
	}

	inline bool operator == (const Move &move) const {
		return val == move.val;
	}

	inline bool operator != (const Move &move) const {
		return val != move.val;
	}

	inline u32 get_from_sq() const {
		return val & 0b111111;
	}

	inline u32 get_to_sq() const {
		return (val >> 6) & 0b111111;
	}

	inline Piece get_cap_pce() const {
		return static_cast<Piece>((val >> 12) & 0b1111);
	}

	inline Piece get_promo_pce() const {
		return static_cast<Piece>((val >> 16) & 0b1111);
	}

	inline bool is_pawn_start() const {
		return val & static_cast<u32>(MoveFlag::PAWN_START);
	}

	inline bool is_castle() const {
		return val & static_cast<u32>(MoveFlag::CASTLE);
	}

	inline bool is_en_passant() const {
		return val & static_cast<u32>(MoveFlag::EN_PASSANT);
	}

	inline bool is_quiet() const {
		return get_cap_pce() == Piece::NONE && get_promo_pce() == Piece::NONE;
	}

	inline std::string to_str() const {
		const u32 from_sq = get_from_sq();
		const i32 from_rank = get_rank(from_sq);
		const i32 from_file = get_file(from_sq);
		const u32 to_sq = get_to_sq();
		const i32 to_rank = get_rank(to_sq);
		const i32 to_file = get_file(to_sq);

		std::string move_str;
		move_str.push_back('a' + from_file);
		move_str.push_back('1' + from_rank);
		move_str.push_back('a' + to_file);
		move_str.push_back('1' + to_rank);

		const Piece promo_pce = get_promo_pce();

		if (promo_pce != Piece::NONE) { //DA!!!
			move_str.push_back(tolower(pce_to_symbol(promo_pce)));
		}

		return move_str;
	}
};

class MoveList {
public:
	static constexpr i32 max_moves = 218;
	inline i32 size() const {
		return m_size;
	}

	inline void push_back(const Move move) {
		moves[m_size++] = move;
	}

	inline Move get(const i32 i) const {
		return moves[i];
	}

	inline void swap(const i32 i1, const i32 i2) {
		std::swap(moves[i1], moves[i2]);
	}
private:
	i32 m_size = 0;
	std::array<Move, max_moves> moves{};
};